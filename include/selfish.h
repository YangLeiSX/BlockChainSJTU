/**
 * @file selfish.h
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief 
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

class Selfish : public Client {
 protected:
    int win;
    void SecretSynchro(std::vector<Block> &recv);
    void SecretMerge(blk_id target);

    void synchro(std::vector<Block> &recv);
    // void merge(blk_id target);
    bool mining();

    void publishBlock(uint16_t ctr, nonce_t nonce);
    void displayBlockChain();
    void saveBlockChain();

    std::vector<Block> reserve;

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
