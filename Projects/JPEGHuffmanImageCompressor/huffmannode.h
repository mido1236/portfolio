//
// Created by ahmed on 2025-03-03.
//

#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

struct HuffmanNode {
    int channelValue;
    int freq;
    unique_ptr<HuffmanNode> left, right;

    HuffmanNode(const int pixel, const int freq): channelValue(pixel), freq(freq), left(nullptr), right(nullptr) {}

    HuffmanNode(const int pixel, const int freq, HuffmanNode *left, HuffmanNode *right): channelValue(pixel),
        freq(freq),
        left(left), right(right) {}

    [[nodiscard]] bool isLeaf() const { return left == nullptr && right == nullptr; }
};

// Compare function for priority queue
struct Compare {
    bool operator()(const unique_ptr<HuffmanNode> &l, const unique_ptr<HuffmanNode> &r) {
        return l->freq > r->freq;
    }
};
#endif //HUFFMANNODE_H
