/**
 * @file honest.h
 * @author YangLei (YangLeiSX@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef HONEST_H_
#define HONEST_H_

#include <vector>

#include "./client.h"

class Honest : public Client {
 protected:
    std::vector<int> forkLen;
    void displayBlockChain();
    void saveBlockChain();

    void checkFork();

 public:
    Honest() : Client() {}
    explicit Honest(usr_id _idx) : Client(_idx) {}
    ~Honest() {redundancy.clear();}
    void operator()();
};

#endif  // HONEST_H_
