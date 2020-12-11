/**
 * @file client.cpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "../include/client.h"

extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::mutex display;
extern bool running;
extern int roundIdx;

void Client::operator()() {
    // displayWelcome();

    // System Running
    while (running) {
        // Wait for a Round
        roundFlag[idx].wait();

        synchro(recvQueues[idx]);

        // Begin Mining
        ctr = 0;
        while (ctr++ < MAX_CTR) {
            if (mining()) break;
        }
    }

    // displayBlockChain();
    saveBlockChain();
}

bool Client::mining() {
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

bool Client::checkNonce(nonce_t nonce, uint16_t d) {
    nonce_t bit = 1 << (sizeof(bit) * 8 - 1);
    nonce_t mask = 0;
    for (int i = 0; i < d; i++) mask |= bit >> i;
    return !(mask & nonce);
}

void Client::publishBlock(uint16_t ctr, nonce_t nonce) {
    Block newBlock;
    blk_id nidx = 0;
    while (nidx == 0) nidx = rand();
    newBlock.idx = nidx;
    newBlock.prev = chain.getBack().idx;
    newBlock.nonce = nonce;
    newBlock.d = chain.getBack().d;
    newBlock.miner = idx;
    newBlock.ctr = ctr;
    newBlock.height = chain.getBack().height + 1;
    newBlock.timestamp = roundIdx;

    chain.growth(newBlock);
    broadcast.lock();
    for (int i = 0; i < MAX_PLAYER; i++) {
        recvQueues[i].push_back(newBlock);
    }
    broadcast.unlock();
}

void Client::displayWelcome() {
    display.lock();
    std::cout << "\tPlayer " << static_cast<int>(idx)
        << " Running!" << std::endl;
    display.unlock();
}

void Client::displayNewBlock() {
    display.lock();
    std::cout << "----Got a Block #"
        << std::hex << std::setw(2 * sizeof(blk_id))
        << std::setfill('0') << chain.getBack().idx << " ( user: " << std::dec
        << static_cast<int>(idx) << " , ctr: "
        << static_cast<int>(ctr) << " , height "
        << static_cast<int>(chain.getBack().height) << " )"
        << " in ROUND #"
        << static_cast<int>(chain.getBack().timestamp) << "---\n";
    display.unlock();
}

void Client::displayBlockChain() {
    display.lock();
    std::cout << "The Block Chain View from User "
        << static_cast<int>(idx) << ": \n\t";
    for (int i = 0; i < chain.size(); i++) {
        std::cout << " | "
            << std::hex << std::setw(2 * sizeof(blk_id)) << std::setfill('0')
            << chain.getByOrder(i).idx << std::dec;
    }
    std::cout << " |" << std::endl;
    display.unlock();
}

void Client::saveBlockChain() {
    display.lock();
    std::ofstream fout;
    fout.open(std::string(OUTPUT_PATH).c_str(), std::ios::app);
    fout << static_cast<int>(idx) << ",";
    for (int i = 0; i < chain.size(); i++) {
        fout << std::hex << std::setw(2 * sizeof(blk_id)) << std::setfill('0')
            << chain.getByOrder(i).idx << std::dec << ",";
        fout << chain.getByOrder(i).miner << ",";
    }
    fout << "\b" << std::endl;
    fout.close();
    display.unlock();
}

void Client::synchro(std::vector<Block> &recv) {
    broadcast.lock();
    while (!recv.empty()) {
        Block new_blk = recv.front();
        if (chain.empty()) {
            //! initialize block chain
            chain.growth(new_blk);
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
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        }
        recv.erase(recv.begin());
    }
    broadcast.unlock();
}

void Client::merge(blk_id target) {
    Block cadidate = redundancy[target];
    if (chain.exist(cadidate.prev)) {
        while (chain.getBack().idx != cadidate.prev) {
            Block back = chain.shorten();
            // redundancy[back.idx] = back;
            redundancy.insert(std::pair<blk_id, Block>(back.idx, back));
        }
        chain.growth(cadidate);
        redundancy.erase(cadidate.idx);
    } else if (cadidate.prev != 0) {
        merge(cadidate.prev);
    }
}
