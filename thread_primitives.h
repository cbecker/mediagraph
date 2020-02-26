// Copyright (c) 2012-2013, Aptarism SA.
//
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the University of California, Berkeley nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// julien.pilet@aptarism.com, 2012.
#ifndef THREAD_PRIMITIVES_H
#define THREAD_PRIMITIVES_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

class ConditionVariablePrivate;
class MutexPrivate;

class Mutex {
public:
  Mutex() = default;
  void lock() { mutex_.lock(); }
  bool tryLock() { return mutex_.try_lock(); }
  void unlock() { mutex_.unlock(); }

  std::mutex mutex_;
};

//! Aquire a lock on the mutex passed to the constructor.
//! The lock is maintained while the ScopedLock object is alive.
//! The lock is released uppon destruction.
class ScopedLock {
public:
  ScopedLock(Mutex *mutex) : mutex_(mutex) { mutex->lock(); }
  ~ScopedLock() { mutex_->unlock(); }

private:
  ScopedLock(const ScopedLock &) {}
  ScopedLock &operator=(const ScopedLock &) { return *this; }
  Mutex *mutex_;
};

class ConditionVariable {
public:
  ConditionVariable() = default;

  void signal() { _condvar.notify_one(); }
  void wakeAll() { _condvar.notify_all(); }
  void wait(Mutex *mutex) {
    std::unique_lock<std::mutex> ulock(mutex->mutex_, std::defer_lock);
    _condvar.wait(ulock);
  }

private:
  std::condition_variable _condvar;
};

class Thread {
public:
  Thread() = default;
  ~Thread() {
    if (thread_.joinable()) {
      thread_.detach();
    }
  }

  bool start(int (*func)(void *), void *ptr) {

    if (isRunning()) {
      return false;
    }

    // we may need to wait for the thread to finish, as it may be still
    // terminating (flag running_ can be set before thread ends)
    if (thread_.joinable())
      thread_.join();

    // Wrap the function in a lambda to signal when the thread has finished
    // processing
    std::packaged_task<int(void *, std::weak_ptr<bool>)> task(
        [func](void *p, std::weak_ptr<bool> runningFlag) {
          const int retVal = func(p);
          if (auto r = runningFlag.lock()) {
            *r = false;
          }

          return retVal;
        });
    thread_ret_future_ = task.get_future();

    *running_ = true;
    thread_ =
        std::thread(std::move(task), ptr, std::weak_ptr<bool>(this->running_));

    return true;
  }

  bool isRunning() const { return *running_; }

  // Returns the thread exit value, as returned by 'func' above.
  // returns -1 if the thread has never been started.
  int waitForTermination() {
    if (thread_.joinable()) {
      thread_.join();
    }

    // make sure we call .get() once. If called > 1 time it will throw an
    // exception
    return thread_ret_future_.valid() ? thread_ret_future_.get() : -1;
  }

  static void setCurrentName(const char *name) { (void)name; }

private:
  std::thread thread_;
  std::future<int>
      thread_ret_future_; // holds future return value of thread fn.

  // the thread may outlive this class (see destructor's detach()), so we use a
  // shared_ptr here
  std::shared_ptr<bool> running_ = std::make_shared<bool>(false);
};

#endif // THREAD_PRIMITIVES_H
