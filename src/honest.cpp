/**
 * @file honest.cpp
 * @author YangLei (YangLeiSX@sjtu.edu.cn)
 * @brief Implementation of Class Honest
 * @version 0.1
 * @date 2020-12-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <set>

#include "../include/honest.h"

// Shared Global Variables
extern semaphore roundFlag[];
extern std::mutex broadcast;
extern std::vector<std::vector<Block>> recvQueues;
extern std::mutex display;
extern bool running;
extern int roundIdx;
extern std::default_random_engine randeng;
extern std::set<usr_id> forkers;

/**
 * @brief Overloaded Operator() for Callable Class
 * 
 */
void Honest::operator()() {
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

    // Check Fork Attack Result
    checkFork();

    // Display and Save all Blocks
    // displayBlockChain();
    saveBlockChain();
}

/**
 * @brief Check Result of Fork Attack
 * 
 */
void Honest::checkFork() {
    int _forkLen = 0;
    for (int i = 0; i < chain.size(); i++) {
        // Block Generated after Fork Attack Begin
        if (BEGIN_FORK(chain.getByOrder(i).timestamp)) {
            if (IS_FORKER(chain.getByOrder(i).miner)) {
                if (i != 0 && IS_FORKER(chain.getByOrder(i-1).miner)) {
                    // In Fork Sequence
                    _forkLen++;
                } else {
                    // Get in Fork Sequence
                    _forkLen = 1;
                }
            } else {
                if (i != 0 && IS_FORKER(chain.getByOrder(i-1).miner)) {
                    // Get out Fork Sequence
                    forkLen.push_back(_forkLen);
                    _forkLen = 0;
                }
            }
        }
    }
}

/**
 * @brief Display Information of Blocks in BlockChain
 * 
 */
void Honest::displayBlockChain() {
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
void Honest::saveBlockChain() {
    display.lock();
    std::ofstream fout;
    fout.open(std::string(OUTPUT_PATH).c_str(), std::ios::app);
    fout << static_cast<int>(idx) << ",";
    for (int _forkLen : forkLen)
        fout << _forkLen << "|";
    fout << "\b,";
    for (int i = 0; i < chain.size(); i++) {
        fout << std::hex << std::setw(2 * sizeof(blk_id)) << std::setfill('0')
            << chain.getByOrder(i).idx << std::dec << ",";
        fout << chain.getByOrder(i).miner << ",";
    }
    fout << "\b" << std::endl;
    fout.close();
    display.unlock();
}
