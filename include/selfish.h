/**
 * @file selfish.h
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief Definition of Attacker in Selfish Mining Attack
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SELFISH_H_
#define SELFISH_H_

#include <set>
#include <vector>

#include "./client.h"

/**
 * @brief Definition of Attacker in Selfish Mining Attach
 * Inherit from Class Client
 * 
 */
class Selfish : public Client {
 protected:
    // Counter of Successful Attack
    int win;
    // Store Reserved Blocks
    std::vector<Block> reserve;

    // Auxiliary Function for Synchronization in Secret Channel
    void SecretSynchro(std::vector<Block> &recv);
    void SecretMerge(blk_id target);
    // Overloaded Function
    void synchro(std::vector<Block> &recv);
    bool mining();

    void publishBlock(uint16_t ctr, nonce_t nonce);
    void displayBlockChain();
    void saveBlockChain();

 public:
    Selfish() : Client() {
       win = 0;
    }
    explicit Selfish(usr_id _idx) : Client(_idx) {
       win = 0;
    }
    ~Selfish() {redundancy.clear();}
    void operator()();
};

#endif  // SELFISH_H_
