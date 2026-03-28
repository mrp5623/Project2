//
// Created by nick spallone on 3/26/26.
//

#include "FragranceHeap.h"

bool FragranceHeap::empty() {
    return heap.empty();
}

int FragranceHeap::size() {
    return heap.size();
}

int FragranceHeap::parent(int ind) {
    return (ind - 1) / 2;
}

int FragranceHeap::leftChild(int ind) {
    return 2 * (ind + 1);
}

int FragranceHeap::rightChild(int ind) {
    return 2 * (ind + 2);
}

void FragranceHeap::heapifyUp(int childInd) {
    while (heap[parent(childInd)].score >= 0 && heap[parent(childInd)].score > heap[childInd].score) {
        std::swap(heap[childInd], heap[parent(childInd)]);
        childInd = parent(childInd);
    }
}

void FragranceHeap::heapifyDown(int childInd) {
    int leftInd = leftChild(childInd);
    int rightInd = rightChild(childInd);
    int min = childInd;

    if (leftInd < heap.size() &&
        heap[leftInd].score < heap[min].score) {
        min = leftInd;
    }
    if (rightInd < heap.size() &&
        heap[rightInd].score < heap[min].score) {
        min = rightInd;
    }

    if (min != childInd) {
        std::swap(heap[childInd], heap[min]);
        heapifyDown(min);
    }
}

void FragranceHeap::push(int score, Fragrance* frag) {
    heap.push_back(FragNode(score, frag));
    heapifyUp(heap.size() - 1);
}

FragranceHeap::FragNode FragranceHeap::top() {
    if (empty()) {
        throw std::runtime_error("FragranceHeap is empty");
    }
    return heap[0];
}

void FragranceHeap::pop() {
    if (empty()) {
        throw std::runtime_error("FragranceHeap is empty");
    }

    heap[0] = heap.back();
    heap.pop_back();
    heapifyDown(0);
}