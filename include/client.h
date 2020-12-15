/**
 * @file client.hpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief Base Class for Client in the System
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

/**
 * @brief Base Class of Client in the System
 * 
 */
class Client {
 protected:
    // Client Index
    usr_id idx;
    // Mining Counter
    uint16_t ctr;
    // Counter of new Blocks in the system
    uint16_t seen;
    // Blockchain of client
    BlockChain chain;
    // Redundant information for synchronization
    std::map<blk_id, Block> redundancy;

    // Auxiliary Function for Synchronization
    void synchro(std::vector<Block> &recv);
    void merge(blk_id p);

    // Auxiliary Function for Block Generation
    bool mining();
    bool checkNonce(nonce_t nonce, uint16_t d);
    void publishBlock(uint16_t ctr, nonce_t nonce);

    // Auxiliary Function for Visualization
    void displayWelcome();
    void displayNewBlock();
    void displayBlockChain();
    void saveBlockChain();

 public:
    Client() {
        idx = 0;
        ctr = 0;
        seen = 0;
    }
    explicit Client(usr_id _idx) : idx(_idx), ctr(0), seen(0) {}
    ~Client() {redundancy.clear();}
    void operator()();
};

#endif  // CLIENT_H_
