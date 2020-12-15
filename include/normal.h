/**
 * @file normal.h
 * @author YangLei (YangLeiSX@sjtu.edu.cn)
 * @brief Definition of Normal Client in Selfish Mining Attack
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef NORMAL_H_
#define NORMAL_H_

#include <vector>

#include "./client.h"

/**
 * @brief Definition of Normal Client in Selfish Mining Attack.
 * Inherit from Class Client
 * 
 */
class Normal : public Client {
 protected:
    // Overloaded Function
    void saveBlockChain();

 public:
    Normal() : Client() {}
    explicit Normal(usr_id _idx) : Client(_idx) {}
    ~Normal() {redundancy.clear();}
    void operator()();
};

#endif  // NORMAL_H_
