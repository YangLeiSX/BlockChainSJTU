/**
 * @file block.h
 * @author YangLei (yangleisx@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef BLOCK_H_
#define BLOCK_H_

#include "./main.h"

struct Block {
    blk_id idx;         // index of the block
    blk_id prev;        // index of previous block
    nonce_t nonce;      // nonce for PoW
    uint16_t d;          // difficulty of PoW
    usr_id miner;       // miner
    uint16_t ctr;       // mining times
    uint16_t height;    // block height in chain
    uint16_t timestamp; // generate time in round
};

#endif  // BLOCK_H_
