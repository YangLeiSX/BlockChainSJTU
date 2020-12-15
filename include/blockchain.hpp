/**
 * @file blockchain.hpp
 * @author yanglei (yangleisx@sjtu.edu.cn)
 * @brief Definition of BlockChain
 * @version 0.1
 * @date 2020-12-09
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef BLOCKCHAIN_HPP_
#define BLOCKCHAIN_HPP_

#include <map>
#include <vector>
#include <utility>

#include "./main.h"
#include "./block.h"

/**
 * @brief Definition of Block Chain
 * 
 */
class BlockChain{
 private:
    // blocks in the chain
    std::vector<Block> content;
    // map from block id to order in the chain
    std::map<blk_id, int> cont_map;

 public:
    // add a new block
    void growth(Block blk) {
        cont_map.insert(std::pair<blk_id, int>(blk.idx, content.size()));
        content.emplace_back(blk);
    }
    // remove a block from tail
    Block shorten() {
        Block back = content.back();
        blk_id id = back.idx;
        content.pop_back();
        cont_map.erase(id);
        return back;
    }
    bool empty() {return content.empty();}
    int size() {return content.size();}
    bool exist(blk_id id) {return cont_map.find(id) != cont_map.end();}
    const Block& getByOrder(int i) {return content[i];}
    const Block& operator[](int i) {return content[i];}
    const Block& getBack() {return content.back();}
    ~BlockChain(){
        content.clear();
        cont_map.clear();
    }
};

#endif  // BLOCKCHAIN_HPP_
