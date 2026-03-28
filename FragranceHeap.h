//
// Created by nick spallone on 3/26/26.
//

#ifndef PERFUMESCRAPER_FRAGRANCEHEAP_H
#define PERFUMESCRAPER_FRAGRANCEHEAP_H

#include <vector>
#include "Fragrance.h"

// min-heap for maintaining top 5 closest fragrance recommendations
class FragranceHeap {
public:
    struct FragNode {
        int score;
        Fragrance* frag;

        FragNode(int s, Fragrance* f) : score(s), frag(f) {}

        bool operator>(const FragNode& second) const {
            return score > second.score;
        }
    };

    // helpers
    bool empty();
    int size();

    void push(int score, Fragrance* frag);

    FragNode top();
    void pop();

private:
    std::vector<FragNode> heap;

    // helpers to calculate indices of related nodes/elements
    int parent(int ind);
    int leftChild(int ind);
    int rightChild(int ind);

    // to maintain heap properties
    void heapifyUp(int childInd);
    void heapifyDown(int childInd);
};

#endif //PERFUMESCRAPER_FRAGRANCEHEAP_H