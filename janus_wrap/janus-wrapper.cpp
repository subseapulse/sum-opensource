/**
 * [2024] SubSeaPulse SRL 
 * All Rights Reserved.
 */

// This is free software: you can redistribute it and/or modify it        *
// under the terms of the GNU General Public License version 3 as         *
// published by the Free Software Foundation.                             *
//                                                                        *
// This program is distributed in the hope that it will be useful, but    *
// WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS     *
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for       *
// more details.                                                          *
//                                                                        *
// You should have received a copy of the GNU General Public License      *
// along with this program. If not, see <http://www.gnu.org/licenses/>.   *

#include <janus-wrapper.h>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <janus/dump.h>

const unsigned int JanusWrapper::JANUS_RED_SIZE = 1024;
const unsigned int JanusWrapper::MAX_SIZE_TO_READ = 1250000; // computed esperimentally for 32 byte payload

JanusWrapper::JanusWrapper()
:
  params(nullptr),
  simple_tx(nullptr),
  state_tx(nullptr),
  simple_rx(nullptr),
  state_rx(nullptr),
  carrier_sensing(nullptr),
  rx_buffer(nullptr),
  active(false),
  demod_thread(),
  mutex_demod(),
  cv_demod(),
  demod_data_available(0)
{
  params = janus_parameters_new();
  initDefaultParams();
}


JanusWrapper::~JanusWrapper()
{
  active = false;
  cv_demod.notify_one();
  if(demod_thread.joinable())
    demod_thread.join();
  janus_parameters_free(params);
  if(simple_tx)
    janus_simple_tx_free(simple_tx);
  if(simple_rx)
    janus_simple_rx_free(simple_rx);
  if(state_tx)
    janus_tx_state_free(state_tx);
  if(state_rx)
    janus_rx_state_free(state_rx);
  if(carrier_sensing)
    janus_carrier_sensing_free(carrier_sensing);
}

bool 
JanusWrapper::janusSetup() 
{
  simple_tx = janus_simple_tx_new(params);
  simple_rx = janus_simple_rx_new(params);
  state_tx = janus_tx_state_new((params->verbose > 1));
  state_rx = janus_rx_state_new(params);
  carrier_sensing = janus_carrier_sensing_new(janus_simple_rx_get_rx(simple_rx));
  active = true;
  demod_thread = std::thread(&JanusWrapper::performDemodulation,this);
  return true;
}

void 
JanusWrapper::initDefaultParams()
{
  params->verbose = 0;
  params->pset_id = 0;
  params->pset_center_freq = 11520;
  params->pset_bandwidth = 4160;
  params->stream_channel = 0;
  params->stream_channel_count = 1;
  params->stream_passband = 1;
  params->wut = 0;
  //params->stream_driver = "alsa";
  params->stream_driver = "fifo";
  //params->stream_driver_args = "default"; 
  params->stream_driver_args = "2000000"; 
  params->stream_fs = 44100;
  params->stream_format = "S16";
  params->stream_passband = 1;
  params->stream_amp = JANUS_REAL_CONST(0.95);
  params->stream_mul = 1;
  params->doppler_correction = 1;
  params->doppler_max_speed = JANUS_REAL_CONST(5.0);
  params->compute_channel_spectrogram = 1;
  params->detection_threshold = JANUS_REAL_CONST(2.5);
  params->colored_bit_prob = 0;
  params->cbp_high2medium  = JANUS_REAL_CONST(0.2);
  params->cbp_medium2low   = JANUS_REAL_CONST(0.35);
}
void 
JanusWrapper::initPacket(janus_packet_t packet)
{
  janus_packet_set_mobility(packet, 0);
  janus_packet_set_tx_rx(packet, 1);
  janus_packet_set_forward(packet, 0);
  janus_packet_set_class_id(packet, 16);
  janus_packet_set_app_type(packet, 0);
  janus_packet_set_validity(packet, 1); 
}
bool
JanusWrapper::setCarrierFrequency(float carr_freq)
{
  if(carr_freq != 0.0) {
    params->pset_center_freq = carr_freq;
    return true;
  }
  return false;
}

bool
JanusWrapper::setBandwidth(float bw)
{
  if(bw > 0.0 && bw <= params->pset_center_freq) 
  {
    params->pset_bandwidth = bw;
    return true;
  }
  return false;
}

void 
JanusWrapper::setVerbose(int verbose)
{
  params->verbose = verbose;
}
void
JanusWrapper::setSamplingFrequency(int sampling_frequency_)
{
  if(sampling_frequency_ != 0.0) {
    params->stream_fs = sampling_frequency_;
  }
}


bool
JanusWrapper::setRxBuffer(std::shared_ptr<ConcBuffer> buffer)
{
  if(buffer == nullptr) {
    return false;
  }

  rx_buffer = buffer;
  return true;
}

void
JanusWrapper::setTxGain(float gain)
{
  if(gain > 0.0f && gain < 1.0f)
    params->stream_amp = JANUS_REAL_CONST(gain);
  else
    return;
}

void JanusWrapper::pushRxChunk(std::shared_ptr<Chunk> rx_chunk)
{
  rx_buffer->push(rx_chunk);
}

bool 
JanusWrapper::modulate(std::shared_ptr<Chunk> chunk)
{
  if(chunk->getSize() == 0) {
    return false;
  } 
  janus_packet_t packet = janus_packet_new(params->verbose);
  initPacket(packet);
  char data[Chunk::MAX_HEADER_SIZE + chunk->getSize()] = {0};
  memcpy(data,chunk->getHeader(), Chunk::MAX_HEADER_SIZE);
  memcpy(data + Chunk::MAX_HEADER_SIZE,chunk->data(), chunk->getSize());

  int cargo_error = janus_packet_set_cargo(
    packet, (janus_uint8_t*)data, Chunk::MAX_HEADER_SIZE + chunk->getSize());
  if (cargo_error == JANUS_ERROR_CARGO_SIZE)
  {
    janus_packet_free(packet);
    return false;
  }
  janus_packet_encode_application_data(packet);
  janus_packet_set_validity(packet, 2);
  if(janus_simple_tx_execute(simple_tx, packet, state_tx) < 0) {
    janus_packet_free(packet);
    return false;
  }
  
  janus_tx_state_dump(state_tx);
  janus_packet_dump(packet);

  janus_ostream_t ostream = janus_simple_tx_get_ostream(simple_tx);
  std::shared_ptr<Samples> samples = std::make_shared<Samples>(
    JanusWrapper::MAX_SIZE_TO_READ);
  int red_size = ostream->read(ostream,samples->samples.data(),
    JanusWrapper::MAX_SIZE_TO_READ);
  if(red_size < 0) {
    janus_packet_free(packet);
    return false;
  }
  samples->samples.resize(red_size);
  samples->sample_size = red_size;//actual_size;
  chunk->setModulatedSamples(samples);
  janus_packet_free(packet);
  return true;

}
bool 
JanusWrapper::demodulate(std::shared_ptr<Samples> samples)
{
  janus_istream_t istream = janus_simple_rx_get_istream(simple_rx);
  std::unique_lock<std::mutex> lk(mutex_demod);
  istream->write(istream,samples->samples.data(),samples->sample_size);
  demod_data_available += samples->sample_size;
  if(demod_data_available >= 10) {
    cv_demod.notify_one();
  }
  lk.unlock();  
  return true;
}

void JanusWrapper::performDemodulation()
{
  unsigned queried_detection_time = 0;
  janus_packet_t packet_rx = janus_packet_new(params->verbose);
  janus_real_t time;
  initPacket(packet_rx);

  while(active.load()) {
    std::unique_lock<std::mutex> lck{mutex_demod};
    cv_demod.wait(lck,[&](){
      return demod_data_available >= JanusWrapper::JANUS_RED_SIZE || !(active.load());
    }); 
    int rv = janus_simple_rx_execute(simple_rx, packet_rx, state_rx);
    
    if (rv < 0) // error in reading
    {
      lck.unlock();
      if (rv == JANUS_ERROR_OVERRUN)
      {
        // error happens
      }
    }
    else if (rv > 0)
    {
      demod_data_available -= JanusWrapper::JANUS_RED_SIZE;
      if(demod_data_available < 0) {
        demod_data_available = 0;
      }
      lck.unlock();

      if (janus_packet_get_validity(packet_rx) &&
            janus_packet_get_cargo_error(packet_rx) == 0)
      {
        unsigned int _payload_len = janus_packet_get_cargo_size(packet_rx);
        std::shared_ptr<Chunk> rx_chunk = std::make_shared<Chunk>(_payload_len);
        if(rx_chunk->getSize() <= _payload_len) {
          char hdr[Chunk::MAX_HEADER_SIZE] = {0};
          memcpy(hdr, reinterpret_cast<char *>(
            janus_packet_get_cargo(packet_rx)), Chunk::MAX_HEADER_SIZE);
          memcpy(rx_chunk->data(), reinterpret_cast<char *>(
            janus_packet_get_cargo(packet_rx)) + Chunk::MAX_HEADER_SIZE, 
          _payload_len - Chunk::MAX_HEADER_SIZE);
          rx_chunk->setSize(_payload_len - Chunk::MAX_HEADER_SIZE);
          rx_chunk->addHeader(hdr,sizeof(MacHdr));
          pushRxChunk(rx_chunk);
        }
      } else if (janus_packet_get_cargo_error(packet_rx) != 0) {
        // ERROR janus_packet_get_cargo_error
      } else if (!janus_packet_get_validity(packet_rx)) {
        //Janus RX:: ERROR janus_packet_get_validity
      }
      janus_packet_reset(packet_rx);
      queried_detection_time = 0;
      janus_carrier_sensing_reset(carrier_sensing);
    }
    else
    {     
      demod_data_available -= JanusWrapper::JANUS_RED_SIZE;  
      if(demod_data_available < 0) {        
        demod_data_available = 0;
      }
      lck.unlock();
      if(janus_simple_rx_has_detected(simple_rx) 
        && !queried_detection_time)
      {
        queried_detection_time = 1;
      }
      if (janus_carrier_sensing_execute(carrier_sensing, &time) > 0) {
        janus_carrier_sensing_window_power(carrier_sensing);
        janus_carrier_sensing_background_power(carrier_sensing);
      }
    }
  }
  janus_packet_free(packet_rx);
}
