/**
 * [2024] SubSeaPulse SRL 
 * All Rights Reserved.
 * @file chunk.h
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Chunk of data
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

#ifndef H_CHUNK
#define H_CHUNK

#include <vector>
#include <memory>
#include <chrono>
#include <concurrent-queue.h>

/**
 * Struct with modulated samples to be transmitted with a 16-bits DAC
 */
struct Samples {
  std::vector<int16_t> samples; /**< structure to store the samples*/
  
  size_t sample_size; /**< Size of stored samples*/
  
  /**
   * Constructor to set first allocation size
   * @param init_allocation size as number of samples
   */
  Samples(size_t init_allocation)
    : samples(init_allocation, 0x00)
    , sample_size(0)
  {}
};

/**
 * MAC header POD struct. 
 * 0 is reserved for broadcast
 */
struct MacHdr {
  uint8_t src_addr  : 4;
  uint8_t dest_addr : 4;
};

const unsigned int BROADCAST_ADDRESS = 0;

/**
 * This class represents a chunk
 */
class Chunk{
private:
  std::size_t chunk_size; /**< chunk size in bytes */
  std::vector<char> data_chunk; /**< structure to store the data*/  
  std::vector<char> chunk_hdr; /**< structure to store the serialized packet header*/
  size_t hdr_rev_offset; /**< revese offset to understand where the data shoudl be serialized*/
  bool modulated; /**< set to true if already modulated */ 
  std::shared_ptr<Samples> samples; /**< contains the modulated samples, or nullptr */ 
public:
  static const size_t DEFAULT_SIZE; /**< default chunk size in bytes*/
  static const size_t MAX_HEADER_SIZE; /**< maximum header size in bytes*/
  /**
   * Default constructor
   * @param max_bytes maximum chunk size in bytes
   */
  Chunk(std::size_t max_bytes = 512);

  /**
   * Destructor
   */
  ~Chunk();

  /**
   * Get the maximum size
   * @return the max size
   */
  std::size_t getMaxSize() const;

  /**
   * Get the number of bytes stored in the chunk
   * @return number of bytes in the chunk
   */
  std::size_t getSize() const;

  /**
   * Check if the buffer is full or not
   * @return true if full
   */
  bool isFull() const;

  /**
   * Access the data stored in the chunk
   * 
   * return the data stored in the vector
   */
  char* data();

  /**
   * set the number of bytes stored in the chunk
   * @param sz number of element placed in the chunk
   */
  void setSize(std::size_t sz);  

  /**
   * set the modulated samples
   * @param samp the modulated samples
   */
  void setModulatedSamples(std::shared_ptr<Samples> samp);  

  /**
   * reset the modulated samples and set the modulated flag to false
   */
  void resetModulatedSamples();  

  /**
   * get if the chunk has been modulaed or not
   * @return true if it is modulated
   */
  bool isModulated() const;

  /**
   * get the modulated samples, if any
   * @return the modulated samples, if any, or nullptr
   */
  std::shared_ptr<Samples> getModulatedSamples() const;

  /**
   * insert in the header buffer a header that can be either a plain old data header struct or a buffer
   * @param hdr pointer to the header struct or a buffer
   * @param size size of the header be inserted
   * @return true if serialization succeeds
   */
  bool addHeader(const void* hdr, size_t size);

  /**
   * deserialize a plain old data header struct from the header buffer
   * @param hdr pointer to the header struct
   * @param size size of the header struct to be seralized
   * @return true if serialization succeeds
   */
  bool deserializeHeader(void* hdr, size_t size);

  /**
   * get the header of the chunk
   * @return the chunk header
   */
  const char* getHeader() const;

  /**
   * get the header size
   * @return the header size
   */
  size_t getHeaderSize() const;

  /**
   * set the reception time and the flag valid to true
   * @param rx_t reception time
   */
  void setRxTime(
    const std::chrono::time_point<std::chrono::high_resolution_clock>& rx_t);

  /**
   * get the reception time if valid
   * @param rx_t output parameter to get the reception time
   * @return true if the reception time is valid
   */
  bool getRxTime(
    std::chrono::time_point<std::chrono::high_resolution_clock>& rx_t) const;

};

using ConcBuffer = ConcQueue<std::shared_ptr<Chunk>>;

#endif // H_CONC_BUFFER
