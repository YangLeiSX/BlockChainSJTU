/**
 * @file honest.h
 * @author YangLei (YangLeiSX@sjtu.edu.cn)
 * @brief Definition of Honest Client in Fork Attack
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

/**
 * @brief Definition of Honest Client in Fork Attack.
 * Inherit from Class Client
 * 
 */
class Honest : public Client {
 protected:
    // Record Successful Fork Attack
    std::vector<int> forkLen;
    // Overloaded Function
    void displayBlockChain();
    void saveBlockChain();
    // Record Fork Attack
    void checkFork();

 public:
    Honest() : Client() {}
    explicit Honest(usr_id _idx) : Client(_idx) {}
    ~Honest() {redundancy.clear();}
    void operator()();
};

#endif  // HONEST_H_
