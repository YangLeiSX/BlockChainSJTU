/**
 * @file main.h
 * @author yanglei (yangliesx@sjtu.edu.cn)
 * @brief Parameters in Simulation
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

// System parameters
#define MAX_PLAYER 20  // 20
#define MAX_ROUND 50    // 50
#define MAX_CTR 1024    // 1024
#define INIT_D 15       // 15

// Fork Attack
#define FORKER_RATE 0.1
#define HONEST_RATE (1 - FORKER_RATE)
#define IS_FORKER(idx) (forkers.find(idx) != forkers.end())
#define FORK_ROUND 0.3
#define BEGIN_FORK(round) (round >= static_cast<int>(MAX_ROUND * FORK_ROUND))

// Selfish Mining
#define SELFISH_RATE 0.1
#define NORMAL_RATE (1 - SELFISH_RATE)
#define SELFISH_LEN 3

// Macros for visualization
#define OUTPUT_PATH "./result.csv"

// Type Definition
typedef uint32_t blk_id;
typedef uint16_t usr_id;
typedef uint16_t nonce_t;

#endif  // MAIN_H_
