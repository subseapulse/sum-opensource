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

#include "tcp-stream.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>


TcpStream::TcpStream(int port_n) :
list_sck(-1),
data_sck(-1),
mutex_sock()
{
  bindTCPListenServer(port_n, list_sck);
}

bool TcpStream::bindTCPListenServer(int recv_port, int& socklist)
{
  if ((socklist = socket(AF_INET, SOCK_STREAM, 0)) < 0) {    
    return false;
  }

  int iSetOption(1);
  setsockopt(socklist, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
        sizeof(iSetOption)); // TO AVOID TO WAIT SOCKET TO BE FREED WHEN RESTART PROCESS
  struct sockaddr_in my_addr;
  memset((char*)&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(recv_port);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(socklist, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
    return false;
  }

  if (listen(socklist, 5) < 0) {
    return false;
  }
  return true;
}

bool TcpStream::acceptTCP(int socklist, int& socketfd)
{
  while ((socketfd = accept(socklist, (struct sockaddr*)NULL, NULL)) < 0) {
    if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
      return false;
    }
  }
  return true;
}

TcpStream::~TcpStream()
{
  quit();
}

void TcpStream::quit()
{
  if(list_sck>=0) {
    shutdown(list_sck, SHUT_RDWR);
    close(list_sck);
    list_sck = -1;
  }
  if(data_sck>=0) {
    shutdown(data_sck, SHUT_RDWR);
    close(data_sck);
    data_sck = -1;
  }
}

bool
TcpStream::tx(std::shared_ptr<Chunk> ck)
{
  if(data_sck < 0) { 
    acceptConnection();  
  } 
  if(data_sck >= 0) {    
    int total_sent_size = 0;
    while(total_sent_size < ck->getSize()) {
      int sent_size = send(data_sck,ck->data()+total_sent_size,
        ck->getSize()-total_sent_size,0);
      if(sent_size < 0) {
        close(data_sck);
        data_sck = -1;
        return false;
      }
      total_sent_size += sent_size;
    } 
    return true;
  }
  return false;
}

bool
TcpStream::rx(std::shared_ptr<Chunk> ck)
{
  if(data_sck < 0) { 
    acceptConnection();
  } 
  if(data_sck >= 0) {   
    int rx_size = recv(data_sck,ck->data(),ck->getMaxSize(),0);
    if(rx_size <= 0) {
      close(data_sck);
      data_sck = -1;
      return false;
    }
    ck->setSize(rx_size);
    return true;
  }
  return false;
}

bool
TcpStream::tx(const std::string& msg)
{
  if (data_sck < 0) {
    acceptConnection();
  }
  if (data_sck >= 0) {
    int total_sent_size = 0;
    while ((uint)total_sent_size < msg.size()) {

      int sent_size = send(data_sck, msg.c_str() + total_sent_size,
          msg.size() - total_sent_size, 0);

      if (sent_size < 0) {
        close(data_sck);
        data_sck = -1;
        return false;
      }
      total_sent_size += sent_size;
    }
    return true;
  }
  return false;
}

bool
TcpStream::rx(std::string& msg)
{
  if (data_sck < 0) {
    acceptConnection();
  }
  if (data_sck >= 0) {
    int rx_size = recv(data_sck, &msg[0], msg.size(), 0);

    if (rx_size <= 0) {
      close(data_sck);
      data_sck = -1;
      return false;
    }

    msg.resize(rx_size);
    return true;
  }
  return false;
}

bool TcpStream::acceptConnection()
{
  std::unique_lock<std::mutex> lk(mutex_sock);
  if(data_sck >= 0) { return true;}
  if(acceptTCP(list_sck,data_sck)){ 
    return true;
  }
  return false;
}
