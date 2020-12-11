/**
 * @file main_forker.cpp
 * @author YangLei (YangLeiSX@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <thread>
#include <vector>
#include <set>
#include <mutex>

#include "../include/main.h"
#include "../include/block.h"
#include "../include/semaphore.h"
#include "../include/forker.h"
#include "../include/honest.h"

// Round Synchronization Flag
semaphore roundFlag[MAX_PLAYER];
// Block Broadcast
std::mutex broadcast;
std::vector<std::vector<Block>> recvQueues;
// Display information
std::mutex display;
// System On/Off
bool running = true;
int roundIdx;
std::default_random_engine randeng;
std::set<usr_id> forkers;

static std::vector<std::thread> thread_list;

// Check Round Flags
bool isRoundReady() {
    bool res = true;
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (roundFlag[i].check() == 1)
            res = false;
    }
    return res;
}

int main(int argc, char const *argv[]) {
    display.lock();
    std::cout << "Forker System Startup!" << std::endl;
    display.unlock();

    // Generate Initialize Block
    Block initBlock;
    initBlock.idx = UINT32_MAX;
    initBlock.prev = UINT32_MAX;
    initBlock.nonce = UINT16_MAX;
    initBlock.d = INIT_D;
    initBlock.miner = UINT16_MAX;
    initBlock.ctr = 0;
    initBlock.height = 0;
    initBlock.timestamp = 0;

    for (int i = 0; i < MAX_PLAYER; i++) {
        std::vector<Block> recv;
        recv.push_back(initBlock);
        recvQueues.push_back(recv);
    }

    // Generate Clients
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (i <= MAX_PLAYER * HONEST_RATE) {
            Honest honest(static_cast<usr_id>(i));
            thread_list.emplace_back(honest);
        } else {
            Forker forker(static_cast<usr_id>(i + MAX_PLAYER));
            thread_list.emplace_back(forker);
            forkers.insert(i + MAX_PLAYER);
        }
    }

    // Iterate the Round
    roundIdx = 0;
    while (roundIdx++ < MAX_ROUND) {
        // display.lock();
        // std::cout << "Round " << roundIdx << std::endl;
        // display.unlock();

        while (!isRoundReady()) {}

        if (roundIdx == MAX_ROUND) {
            // System Finish
            running = false;
        }

        // Ready for next round
        for (int i = 0; i < MAX_PLAYER; i++)
            roundFlag[i].signal();
    }

    // Clear all Players
    while (!thread_list.empty()) {
        thread_list.back().join();
        thread_list.pop_back();
    }

    display.lock();
    std::cout << "System END.\n";
    display.unlock();

    return 0;
}
