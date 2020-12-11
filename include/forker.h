/**
 * @file forker.h
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef FORKER_H_
#define FORKER_H_

#include <vector>
#include <set>

#include "./client.h"

class Forker : public Client {
 protected:
    void synchro(std::vector<Block> &recv);

    void displayWelcome();
    void saveBlockChain();

 public:
    Forker() : Client() {}
    explicit Forker(usr_id _idx) : Client(_idx) {}
    ~Forker() {redundancy.clear();}
    void operator()();
};
#endif  // FORKER_H_
