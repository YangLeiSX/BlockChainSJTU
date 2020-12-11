/**
 * @file semaphore.h
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief Semaphore for data transfer
 * @version 0.1
 * @date 2020-12-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

class semaphore {
 public:
    // construction function
    explicit semaphore(int value = 1): count(value), wakeups(0) {}
    // semWait（or operation P）
    void wait() {
        std::unique_lock<std::mutex> lock(m);
        if (--count < 0) {
            condition.wait(lock, [&]()->bool{return wakeups > 0;});
            --wakeups;
        }
    }
    // semSignal（or operation V）
    void signal() {
        std::lock_guard<std::mutex> lock(m);
        if (++count <= 0) {
            ++wakeups;
            condition.notify_one();
        }
    }

    int check() {
        return count;
    }

 private:
    int count;
    int wakeups;
    std::mutex m;
    std::condition_variable condition;
};  // class semaphore

#endif  // SEMAPHORE_H_
