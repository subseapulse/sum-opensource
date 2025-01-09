/**
 * [2024] SubSeaPulse SRL. 
 * All Rights Reserved.
 * @file concurrent-queue.h
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Thread safe queue
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

#ifndef H_CONC_QUEUE
#define H_CONC_QUEUE

#include <queue>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

/**
 * This template class represents a thread safe queue 
 */
template<typename T>
class ConcQueue {
protected:
  std::queue<T> queue; /**< queue to be accessed in thread safe way */
  mutable std::mutex mutex_q; /**< mutex for the queue */
  /** condition variable to wait for data in the queue */
  std::condition_variable cv_q; 
  std::atomic<bool> exit_flag; /**< set to true to force quit*/

public:
  // type value for the template
  // this makes the type T accessible also from the outside,
  // using the alias value_type
  using value_type = T;
  
  /**
   * Default constructor
   */
  ConcQueue() :
    queue{}, 
    mutex_q{},
    cv_q{},
    exit_flag{false}
  {

  }

  /**
   * Destructor
   */
  virtual ~ConcQueue()
  {
    exit_flag.store(true);
    cv_q.notify_all();
  }

  /**
  * Free the queue and force stop.
  * @param force_exit allow the user to chose from clearing with and without 
  * forcing the waiting functions to exit.
  */
  void clear(bool force_exit)
  { 
    std::unique_lock<std::mutex> lk(mutex_q);
    if(force_exit)
    {
      exit_flag.store(force_exit);
    }
    while(!queue.empty())
    {
      queue.pop();
    }
    if(force_exit)
    {
      lk.unlock();
      cv_q.notify_one();
    }
  }

  /**
  * Get the size of the queue.
  * 
  * @return the size of the queue.
  */
  size_t size() const
  {
    std::unique_lock<std::mutex> lk(mutex_q);
    return queue.size();
  }

  /**
   * Push data to queue
   * @param element to push 
   * @return true if succeeds
   */
  virtual bool push(const T& element) 
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    queue.push(element);
    lck.unlock();
    exit_flag.store(false);
    cv_q.notify_one();
    return true;
  }

  /**
   * Pop data from queue, if available, and get the front
   * @param element output parameter where to store the front of the queue, 
   *        if available
   * @return false if the queue is empty
   */
  virtual bool pop(T& element) 
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    if(queue.empty()) {
      return false;
    }
    element = queue.front();
    queue.pop();
    return true;
  }

  /**
   * Get the front element from queue, if available
   * @param element output parameter where to store the front of the queue, 
   *        if available
   * @return false if the queue is empty
   */
  bool front(T& element) const
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    if(queue.empty()) {
      return false;
    }
    element = queue.front();
    return true;
  }

  /**
   * Wait and pop data from queue, when available, and get the front
   * @param element output parameter where to store the front of the queue, 
   *        if available
   * @return false if the operation is interrupted with forceExit() method
   */
  virtual bool wPop(T& element) 
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    cv_q.wait(lck,[&](){
      return !queue.empty() || exit_flag;
    });
    if(queue.empty()) {
      return false;
    }
    element = queue.front();
    queue.pop();
    return true;
  }

  /**
   * Wait and front data from queue, when available
   * @param element output parameter where to store the front of the queue, 
   *        if available
   * @return false if the queue is empty after the forceExit() method is called
   */
  bool wFront(T& element) 
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    cv_q.wait(lck,[&](){
      return !queue.empty() || exit_flag;
    });
    if(queue.empty()) {
      return false;
    }
    element = queue.front();
    return true;
  }

  /**
   * Wait up to a max timeout to pop data from queue, if available, 
   * and to get the front
   * @param element output parameter where to store the front of the queue, 
   *        if available
   * @param t_o maximum timeout to wait, in milliseconds
   * @return false if the queue is empty after timeout or forceExit()
   */
  virtual bool wTPop(T& element, std::chrono::milliseconds t_o) 
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    cv_q.wait_for(lck,t_o,[&](){
      return !queue.empty() || exit_flag;
    });
    if(queue.empty()) {
      return false;
    }
    element = queue.front();
    queue.pop();
    return true;
  }


  /**
   * Wait up to a max timeout to front data from queue, if available
   * @param element output parameter where to store the front of the queue, 
   *        if available
   * @param t_o maximum timeout to wait, in milliseconds
   * @return false if the queue is empty after timeout or forceExit()
   */
  bool wTFront(T& element, std::chrono::milliseconds t_o) 
  {
    std::unique_lock<std::mutex> lck{mutex_q};
    cv_q.wait_for(lck,t_o,[&](){
      return !queue.empty() || exit_flag;
    });
    if(queue.empty()) {
      return false;
    }
    element = queue.front();
    return true;
  }


  /**
   * Force all blocked methods to exit
   */
  void forceExit() 
  {
    exit_flag.store(true);
    cv_q.notify_all();
  }

};



#endif // H_CONC_QUEUE
