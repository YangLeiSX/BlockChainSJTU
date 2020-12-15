/**
 * @file client.cpp
 * @author Yang Lei (yangleisx@sjtu.edu.cn)
 * @brief Implementation of Class Client
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "../include/client.h"

// Shared Global Variables
extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::mutex display;
extern bool running;
extern int roundIdx;

/**
 * @brief Overloaded Operator() for Callable Class
 * 
 */
void Client::operator()() {
    // Display Informatino of the Thread
    // displayWelcome();

    // System Running
    while (running) {
        // Wait for a Round
        roundFlag[idx].wait();

        // Synchronization
        synchro(recvQueues[idx]);

        // Begin Mining
        ctr = 0;
        while (ctr++ < MAX_CTR) {
            if (mining()) break;
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
bool Client::mining() {
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
 * @brief Check if the Nonce Satisfy the Difficulty Requirement
 * Left d Bits in Nonce should be 0.
 * 
 * @param nonce Nonce for a New Block
 * @param d Difficulty Requirement
 * @return true  Satisfy the Requirement
 * @return false Not Satisfy
 */
bool Client::checkNonce(nonce_t nonce, uint16_t d) {
    nonce_t bit = 1 << (sizeof(bit) * 8 - 1);
    nonce_t mask = 0;
    for (int i = 0; i < d; i++) mask |= bit >> i;
    return !(mask & nonce);
}

/**
 * @brief Generate and Publish the Block
 * 
 * @param ctr Mining times
 * @param nonce Nonce for new Block
 */
void Client::publishBlock(uint16_t ctr, nonce_t nonce) {
    // Generate New Block
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
    // Publish the New Block
    broadcast.lock();
    for (int i = 0; i < MAX_PLAYER; i++) {
        recvQueues[i].push_back(newBlock);
    }
    broadcast.unlock();
}

/**
 * @brief Display Information of Class
 * 
 */
void Client::displayWelcome() {
    display.lock();
    std::cout << "\tPlayer " << static_cast<int>(idx)
        << " Running!" << std::endl;
    display.unlock();
}

/**
 * @brief Display Information of New Block
 * 
 */
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

/**
 * @brief Display Information of Blocks in BlockChain
 * 
 */
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

/**
 * @brief Save Information of Blocks to File 
 * 
 */
void Client::saveBlockChain() {
    display.lock();
    std::ofstream fout;
    fout.open(std::string(OUTPUT_PATH).c_str(), std::ios::app);
    fout << static_cast<int>(idx) << ",";
    fout << seen << ",";
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
 * @brief Synchronization the Blocks
 * 
 * @param recv Receove Queue of the Client
 */
void Client::synchro(std::vector<Block> &recv) {
    broadcast.lock();
    while (!recv.empty()) {
        // Sequential Traversal
        Block new_blk = recv.front();
        seen++;
        if (chain.empty()) {
            // initialize block chain
            chain.growth(new_blk);
        } else if (new_blk.prev == chain.getBack().idx &&
            new_blk.height == chain.getBack().height + 1 &&
            new_blk.miner != idx) {
            // legal block
            chain.growth(new_blk);
        } else if (new_blk.height > chain.getBack().height) {
            // need merge
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

/**
 * @brief Merge fork of Chains
 * 
 * @param target Previous Block's ID to Merge
 */
void Client::merge(blk_id target) {
    // Get Previous Block
    Block cadidate = redundancy[target];
    // Found Merge Point
    if (chain.exist(cadidate.prev)) {
        // Clear Useless Block
        while (chain.getBack().idx != cadidate.prev) {
            Block back = chain.shorten();
            redundancy.insert(std::pair<blk_id, Block>(back.idx, back));
        }
        // Merge One Block
        chain.growth(cadidate);
        redundancy.erase(cadidate.idx);
    // Not Found Merger Point
    } else if (cadidate.prev != 0) {
        // Recursively Find
        merge(cadidate.prev);
    }
}
