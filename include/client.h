/**
 * @file client.hpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include <vector>
#include <map>
#include <random>
#include <utility>

#include <fstream>
#include <iostream>
#include <iomanip>

#include "./main.h"
#include "./block.h"
#include "./blockchain.hpp"
#include "./semaphore.h"

class Client {
 protected:
    // Client Index
    usr_id idx;
    uint16_t ctr;
    // Blockchain of client
    BlockChain chain;
    // Redundant information for synchronization
    std::map<blk_id, Block> redundancy;

    void synchro(std::vector<Block> &recv);
    void merge(blk_id p);
    bool mining();

    bool checkNonce(nonce_t nonce, uint16_t d);
    void publishBlock(uint16_t ctr, nonce_t nonce);

    void displayWelcome();
    void displayNewBlock();
    void displayBlockChain();
    void saveBlockChain();

 public:
    Client() {
        idx = 0;
        ctr = 0;
    }
    explicit Client(usr_id _idx) : idx(_idx), ctr(0) {}
    ~Client() {redundancy.clear();}
    void operator()();
};

#endif  // CLIENT_H_
