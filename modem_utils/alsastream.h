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

/**
 * @file alsastream.h
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Class for interfacing with a PCM audio interface with the ALSA dev API. 
 */


#ifndef ALSASTREAM_H
#define ALSASTREAM_H

#include <alsa/asoundlib.h>

#include <vector>
#include <chunk.h>
#include <string>

/**
 * Singleton class used to create an alsa audio stream
 * to and from the default audio interface,
 * i.e. speakers/headphones
 */
class AlsaStream {

public:

	/** 
	 * Class constructor.
	 */
	AlsaStream();

	AlsaStream(const char * alsa_pcm, unsigned int rate);

	/** 
	 * Class destructor.
	 */
	~AlsaStream();

	/** 
	 * Initialization of PCM handles.
	 * 
	 * @return err from ALSA
	 */
	int init();
	
	/** 
	 * Set hardware parameters for specified handle.
	 * 
	 * @return err from ALSA
	 * 
	 * @param handle snd_pcm_t Handle of the PCM to be configured
	 */
	int set_hw_params(snd_pcm_t* handle);

	/** 
	 * Set software parameters for specified handle.
	 * 
	 * @return err from ALSA
	 * 
	 * @param handle snd_pcm_t Handle of the PCM to be configured
	 */
	int set_sw_params(snd_pcm_t *handle);

	/** 
	 * Transmit the provided samples through the PCM.
	 * These are sent on the ALSA ring buffer in bunch of period_size,
	 * and the function handles if the data size is not a multiple of
	 * period_size.
	 * The settings require the samples to be of 16bit format,
	 * where endianness is determined by the specific machine.
	 * 
	 * @return frames sent successfully
	 * 
	 * @param data const std::vector<uint16_t>& Data to be sent
	 */
	int transmit(const std::vector<int16_t> &data);

	/**
	 * Unload the PCM receiving ring buffer of the samples
	 * that have been recorded and are still pending.
	 * 
	 * @return std::shared_ptr<Chunk> Smart pointer of the samples unloaded.
	 * 
	 * @param num_rec_symbols Number of symbols for a single capture
	 */
	std::shared_ptr<Samples> receive(int num_rec_symbols);

	/**
	 * Unloads the PCM receiving ring buffer without storing its content
	 * in a local vector.
	 * 
	 * @return bool True if operations of closing interface, dropping samples
	 * and re-opening interface, are successful.
	 */
	bool clear_old_rx_data();
	
	static const std::string pcm_name; /**< Name of the audio interface to be used.*/

private:

	/**
	 * Generate a string containing the provided message and
	 * the error definition of the err (int) given in the function parameters.
	 * 
	 * @return log_message const std::string Concat of message and error corresponding to err.
	 * 
	 * @param message const char * Text string message.
	 * @param err int error int number .
	 */
	static const std::string alsastream_error(const char * message, int err);


	unsigned int channels; /**< Number of channels.*/

	std::string device_name; /**< PCM name used for ALSA.*/

	int dir; /**< Variable used in ALSA functions.*/

	unsigned int rate; /**< Sampling frequency to be used on the PCM.*/

	unsigned int rrate; /**< Sampling frequency assigned by ALSA.*/

	size_t total_frames_sent; /**< Total frames sent from the obj initialization.*/

	unsigned int buffer_time; /**< Size of the ring buffer, expressed in us.*/

	unsigned int period_time; /**< Size of a single PCM period, expressed in us.*/

	int resample; /**< Enable ALSA resampling.*/

	snd_pcm_t* handle_tx; /**< ALSA handle for transmission.*/

	snd_pcm_t* handle_rx; /**< ALSA handle for reception.*/

	snd_pcm_format_t format; /**< Format used for a single sample.*/

	snd_pcm_uframes_t buffer_size; /**< Size of the ring buffer, expressed in bits.*/

	snd_pcm_uframes_t period_size; /**< Size of a single PCM period, expressed in us.*/

	snd_pcm_hw_params_t* hw_params; /**< Struct for hardware parameters.*/

	snd_pcm_sw_params_t* sw_params; /**< Struct for software parameters.*/

};

#endif
