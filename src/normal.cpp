/**
 * @file normal.cpp
 * @author YangLei (YangLeiSX@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <set>

#include "../include/normal.h"

extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::mutex display;
extern bool running;
extern int roundIdx;
extern std::default_random_engine randeng;
extern std::set<usr_id> selfisher;

void Normal::operator()() {
    // displayWelcome();

    // System Running
    while (running) {
        // Wait for a Round
        roundFlag[idx].wait();

        synchro(recvQueues[idx]);

        // Begin Mining
        ctr = 0;
        while (ctr++ < MAX_CTR && !mining()) {}
        // while (ctr++ < MAX_CTR) {}
    }

    // displayBlockChain();
    saveBlockChain();
}

void Normal::saveBlockChain() {
    display.lock();
    std::ofstream fout;
    fout.open(std::string(OUTPUT_PATH).c_str(), std::ios::app);
    fout << static_cast<int>(idx) << ",N,";
    for (int i = 0; i < chain.size(); i++) {
        fout << std::hex << std::setw(2 * sizeof(blk_id)) << std::setfill('0')
            << chain.getByOrder(i).idx << std::dec << ",";
        fout << chain.getByOrder(i).miner << ",";
    }
    fout << "\b" << std::endl;
    fout.close();
    display.unlock();
}
