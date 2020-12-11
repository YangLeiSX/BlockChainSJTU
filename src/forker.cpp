/**
 * @file forker.cpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "../include/forker.h"

extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::mutex display;
extern bool running;
extern int roundIdx;
extern std::default_random_engine randeng;
extern std::set<usr_id> forkers;


void Forker::operator()() {
    // displayWelcome();

    // System Running
    while (running) {
        // Wait for a Round
        roundFlag[idx - MAX_PLAYER].wait();

        synchro(recvQueues[idx - MAX_PLAYER]);

        // Begin Mining
        ctr = 0;
        while (ctr++ < MAX_CTR) {
            mining();
        }
    }

    // displayBlockChain();
    saveBlockChain();
}


void Forker::displayWelcome() {
    display.lock();
    std::cout << "\tForker " << static_cast<int>(idx)
        << " Running!" << std::endl;
    display.unlock();
}

void Forker::saveBlockChain() {
    display.lock();
    std::ofstream fout;
    fout.open(std::string(OUTPUT_PATH).c_str(), std::ios::app);
    fout << static_cast<int>(idx) << ",F,";
    for (int i = 0; i < chain.size(); i++) {
        fout << std::hex << std::setw(2 * sizeof(blk_id)) << std::setfill('0')
            << chain.getByOrder(i).idx << std::dec << ",";
        fout << chain.getByOrder(i).miner << ",";
    }
    fout << "\b" << std::endl;
    fout.close();
    display.unlock();
}

void Forker::synchro(std::vector<Block> &recv) {
    broadcast.lock();
    while (!recv.empty()) {
        Block new_blk = recv.front();
        if (chain.empty()) {
            //! initialize block chain
            chain.growth(new_blk);
        } else if (BEGIN_FORK(roundIdx) &&
            !IS_FORKER(new_blk.miner)) {
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        } else if (new_blk.prev == chain.getBack().idx &&
            new_blk.height == chain.getBack().height + 1 &&
            new_blk.miner != idx) {
            //! legal block
            chain.growth(new_blk);
        } else if (new_blk.height > chain.getBack().height) {
            //! need merge
            merge(new_blk.prev);
            chain.growth(new_blk);
        } else {
            //! store for merge
            // redundancy[new_blk.idx] = new_blk;
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        }
        recv.erase(recv.begin());
    }
    broadcast.unlock();
}
