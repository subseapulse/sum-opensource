/**
 * [2024] SubSeaPulse SRL 
 * All Rights Reserved.
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Class TcpStream tcp socket handler.
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

#ifndef H_TCP_STREAM
#define H_TCP_STREAM

#include "chunk.h"
#include <memory>
#include <mutex>
#include <string>

/**
 * Class TcpStream used for TCP socket RAII
 */
class TcpStream
{
public:
  /**
   * Class contructor.
   * @param port_n port number of the listener socket.
   */
  TcpStream(int port_n);

  /**
   * Class destructor.
   */
  ~TcpStream();

  /**
   * Close all sockets and interrupt pending operations.
   */
  void quit();

  /**
   * Method that transmits a chunk.
   * @param ck chunk to be transmitted
   * @return true if the chunk has been transmitted correctly.
   */
  bool tx(std::shared_ptr<Chunk> ck);

  /**
   * Method that receives data from socket and stores it to a chunk.
   * @param ck chunk where to store the received data
   * @return true if data has been received correctly.
   */
  bool rx(std::shared_ptr<Chunk> ck);

  /**
   * Method that transmits a string
   * @param msg string to be sent over the socket
   * @return true if the string has been transmitted correctly
   */
  bool tx(const std::string& msg);

  /**
   * Method that receives data from socket and stores it to a string
   * It is responsibility of the caller to allocate a string large enough
   * to collect all the needed data, from the TCP stream. A string can be
   * allocated in various ways; here are some examples:
   * - std::string s(100, '0');
   * - std::string s; std::string::size_type capacity(100); s.reserve(capacity);
   * - std::string s = "00000000000";
   *
   * @param msg string to store the received data to
   * @return true if data has been received correctly
   */
  bool rx(std::string& msg);

  /**
   * Method that accept a connection from a client.
   * @return true if connection succeeds.
   */
  bool acceptConnection();

  /**
   * Method that returns whether the selected TcpStream object
   * is connected to a client or not.
   * @return true is TcpStream is connected to a client
   */
  bool isConnected() const
  {
      return (data_sck >= 0);
  };

  /**
   * Method that binds the listening TCP socket. Used when opening a TCP stream
   * and when a dropped connection on the other side is detected.
   * @param recv_port Port on which to accept TCP connections.
   * @param socklist output parameter TCP listener Socket file descriptor
   * @return true if the listening socket is bind and open.
   */
  bool bindTCPListenServer(int recv_port, int& socklist);

  /**
   * Method that puts in place a listening TCP socket. Used when opening a TCP stream
   * and when a dropped connection on the other side is detected.
   * @param socklist TCP listener Socket file descriptor
   * @param socketfd output parameter socket file descriptor
   * @return true if the listening returns with a correctly established connection
   */
  bool acceptTCP(int socklist, int& socketfd);

private:
  int list_sck; /**< socket listener file descriptor*/
  int data_sck; /**< data socket file descriptor*/
  std::mutex mutex_sock; /**< mutex for the data socket */

};

#endif /* H_TCP_STREAM */
