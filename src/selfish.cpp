/**
 * @file selfish.cpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief Implementation of Class Selfish
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <algorithm>

#include "../include/selfish.h"

// Shared Global Variables
extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::map<usr_id, std::vector<Block>> selfishChannel;
extern std::mutex display;
extern bool running;
extern int roundIdx;
extern std::set<usr_id> selfisher;

/**
 * @brief Overloaded Operator() for Callable Class
 * 
 */
void Selfish::operator()() {
    // Display Information of the Thread
    // displayWelcome();

    // System Running
    while (running) {
        // Wait for a Round
        roundFlag[idx - MAX_PLAYER].wait();

        // Synchronization on Secrect Channel
        SecretSynchro(selfishChannel[idx]);
        // Synchronization
        synchro(recvQueues[idx - MAX_PLAYER]);

        // Begin Mining
        ctr = 0;
        while (ctr++ < MAX_CTR) {
            mining();
        }
    }

    // Display and Save all Blocks
    // displayBlockChain();
    saveBlockChain();
}

/**
 * @brief Mining for a New Block
 * 
 * @return true  Successfully Get a Block
 * @return false Not Get
 */
bool Selfish::mining() {
    // calculate hash
    std::hash<blk_id> hash_box;
    nonce_t nonce = static_cast<nonce_t>(
        hash_box(chain.getBack().idx + ctr + rand() % 65535));
    // check hash
    if (checkNonce(nonce, chain.getBack().d)) {
        // Generate and Publish a Block
        publishBlock(ctr, nonce);
        // displayNewBlock();
        return true;
    }
    return false;
}

/**
 * @brief Save Information of Blocks to File 
 * 
 */
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


/**
 * @brief Generate and Publish the Block
 * 
 * @param ctr Mining times
 * @param nonce Nonce for new Block
 */
void Selfish::publishBlock(uint16_t ctr, nonce_t nonce) {
    // Generate New Block
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

    // Check if Satisfy Attack Condition
    if (reserve.size() < SELFISH_LEN - 1) {
        // Not Succeed
        broadcast.lock();
        // Broadcast on Secret Channel
        std::map<usr_id, std::vector<Block>>::iterator itr =
            selfishChannel.begin();
        while (itr != selfishChannel.end()) {
            itr->second.push_back(newBlock);
            itr++;
        }
        broadcast.unlock();
    } else {
        // Successfully Attack
        win++;
        broadcast.lock();
        // Broadcast on Public Channel
        for (int i = 0; i < MAX_PLAYER; i++) {
            for (Block b : reserve) {
                recvQueues[i].push_back(b);
            }
            recvQueues[i].push_back(newBlock);
        }
        broadcast.unlock();
        // Clear Reserved Blocks
        reserve.clear();
    }
}

/**
 * @brief Synchronization the Blocks on Secret Channel
 * 
 * @param recv Receove Queue of the Client on Secret Channel
 */
void Selfish::SecretSynchro(std::vector<Block> &recv) {
    broadcast.lock();
    while (!recv.empty()) {
        // Sequential Traversal
        Block new_blk = recv.front();
        if (reserve.empty()) {
            // initialize block chain
            reserve.push_back(new_blk);
        } else if (new_blk.prev == reserve.back().idx &&
            new_blk.height == reserve.back().height + 1) {
            // legal block
            reserve.push_back(new_blk);
        } else if (new_blk.height > reserve.back().height) {
            // need merge
            SecretMerge(new_blk.prev);
            reserve.push_back(new_blk);
        } else {
            // store for merge
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        }
        recv.erase(recv.begin());
    }
    broadcast.unlock();
}

/**
 * @brief Merge fork of Reserved Blocks
 * 
 * @param target Previous Block's ID to Merge
 */
void Selfish::SecretMerge(blk_id target) {
    // Get Previous Block
    Block cadidate = redundancy[target];
    // Found Merge Point
    if (std::count_if(reserve.begin(), reserve.end(),
        [cadidate](Block b)->bool{return b.idx == cadidate.prev;}) > 0) {
        // Clear Useless Block
        while (!reserve.empty() &&  reserve.back().idx != cadidate.prev) {
            Block back = reserve.back();
            reserve.pop_back();
            redundancy.insert(std::pair<blk_id, Block>(back.idx, back));
        }
        // Merge One Block
        reserve.push_back(cadidate);
        redundancy.erase(cadidate.idx);
    } else if (!reserve.empty()) {
        // Recursively Find Merge Point
        merge(cadidate.prev);
    }
}

/**
 * @brief Synchronization the Blocks
 * 
 * @param recv Receove Queue of the Client
 */
void Selfish::synchro(std::vector<Block> &recv) {
    broadcast.lock();
    // Sequential Traversal
    while (!recv.empty()) {
        Block new_blk = recv.front();
        if (chain.empty()) {
            // initialize block chain
            chain.growth(new_blk);
        } else if (new_blk.prev == chain.getBack().idx &&
            new_blk.height == chain.getBack().height + 1) {
            // legal block
            reserve.clear();
            chain.growth(new_blk);
        } else if (new_blk.height > chain.getBack().height) {
            // need merge
            reserve.clear();
            merge(new_blk.prev);
            chain.growth(new_blk);
        } else {
            // store for merge
            redundancy.insert(
                std::pair<blk_id, Block>(new_blk.idx, new_blk));
        }
        recv.erase(recv.begin());
    }
    broadcast.unlock();
}
