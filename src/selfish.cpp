/**
 * @file selfish.cpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <algorithm>

#include "../include/selfish.h"

extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::map<usr_id, std::vector<Block>> selfishChannel;
extern std::mutex display;
extern bool running;
extern int roundIdx;
extern std::set<usr_id> selfisher;

void Selfish::operator()() {
    // displayWelcome();

    // System Running
    while (running) {
        // Wait for a Round
        roundFlag[idx - MAX_PLAYER].wait();

        SecretSynchro(selfishChannel[idx]);
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

bool Selfish::mining() {
    // calculate hash
    std::hash<blk_id> hash_box;
    nonce_t nonce = static_cast<nonce_t>(
        hash_box(chain.getBack().idx + ctr + rand() % 65535));
    // check hash
    if (checkNonce(nonce, chain.getBack().d)) {
        publishBlock(ctr, nonce);
        // displayNewBlock();
        return true;
    }
    return false;
}

void Selfish::saveBlockChain() {
    display.lock();
    std::ofstream fout;
    fout.open(std::string(OUTPUT_PATH).c_str(), std::ios::app);
    fout << static_cast<int>(idx) << ",";
    fout << win << ",";
    for (int i = 0; i < chain.size(); i++) {
        fout << std::hex << std::setw(2 * sizeof(blk_id)) << std::setfill('0')
            << chain.getByOrder(i).idx << std::dec << ",";
        fout << chain.getByOrder(i).miner << ",";
    }
    fout << "\b" << std::endl;
    fout.close();
    display.unlock();
}

void Selfish::publishBlock(uint16_t ctr, nonce_t nonce) {
    Block newBlock;
    blk_id nidx = 0;
    while (nidx == 0) nidx = rand();
    newBlock.idx = nidx;
    newBlock.prev = reserve.empty() ?
        chain.getBack().idx : reserve.back().idx;
    newBlock.nonce = nonce;
    newBlock.d = chain.getBack().d;
    newBlock.miner = idx;
    newBlock.ctr = ctr;
    newBlock.height = reserve.empty()?
        chain.getBack().height + 1 : reserve.back().height + 1;
    newBlock.timestamp = roundIdx;

    if (reserve.size() < SELFISH_LEN - 1) {
        broadcast.lock();
        std::map<usr_id, std::vector<Block>>::iterator itr =
            selfishChannel.begin();
        while (itr != selfishChannel.end()) {
            itr->second.push_back(newBlock);
            itr++;
        }
        broadcast.unlock();
    } else {
        win++;
        broadcast.lock();
        for (int i = 0; i < MAX_PLAYER; i++) {
            for (Block b : reserve) {
                recvQueues[i].push_back(b);
            }
            recvQueues[i].push_back(newBlock);
        }
        broadcast.unlock();
        reserve.clear();
    }
}

void Selfish::SecretSynchro(std::vector<Block> &recv) {
    broadcast.lock();
    while (!recv.empty()) {
        Block new_blk = recv.front();
        if (reserve.empty()) {
            //! initialize block chain
            reserve.push_back(new_blk);
        } else if (new_blk.prev == reserve.back().idx &&
            new_blk.height == reserve.back().height + 1) {
            //! legal block
            reserve.push_back(new_blk);
        } else if (new_blk.height > reserve.back().height) {
            //! need merge
            SecretMerge(new_blk.prev);
            reserve.push_back(new_blk);
        } else {
            //! store for merge
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        }
        recv.erase(recv.begin());
    }
    broadcast.unlock();
}

void Selfish::SecretMerge(blk_id target) {
    Block cadidate = redundancy[target];
    if (std::count_if(reserve.begin(), reserve.end(),
        [cadidate](Block b)->bool{return b.idx == cadidate.prev;}) > 0) {
        while (!reserve.empty() &&  reserve.back().idx != cadidate.prev) {
            Block back = reserve.back();
            reserve.pop_back();
            redundancy.insert(std::pair<blk_id, Block>(back.idx, back));
        }
        reserve.push_back(cadidate);
        redundancy.erase(cadidate.idx);
    } else if (!reserve.empty()) {
        merge(cadidate.prev);
    }
}

void Selfish::synchro(std::vector<Block> &recv) {
    broadcast.lock();
    while (!recv.empty()) {
        Block new_blk = recv.front();
        if (chain.empty()) {
            //! initialize block chain
            chain.growth(new_blk);
        } else if (new_blk.prev == chain.getBack().idx &&
            new_blk.height == chain.getBack().height + 1) {
            //! legal block
            reserve.clear();
            chain.growth(new_blk);
        } else if (new_blk.height > chain.getBack().height) {
            //! need merge
            reserve.clear();
            merge(new_blk.prev);
            chain.growth(new_blk);
        } else {
            //! store for merge
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        }
        recv.erase(recv.begin());
    }
    broadcast.unlock();
}
