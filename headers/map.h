#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <atomic>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

#include "global.h"
#include "node.h"

#define SEQCST      std::memory_order_seq_cst

typedef std::vector<std::pair<int, int>> Range;

class Map
{
    private:
        Node* root;
        std::atomic<int> counter;

        /* Tree: core functions */
        void deleteNode(Node* nodeToDelete);
        Node* insertNode(Node*& rootNode, Node* newNode);
        void rebalanceTree(Node*& rootNode, Node*& newNode);

        /* Helper functions */

        /* Tree: deleteNode helpers */
        void deleteFix(Node*& rootNode, Node*& node);
        Node* findValue(int valueToFind, Node* rootNode);
        bool isLeaf(const Node* node);
        void transplantNode(Node* nodeToReplace, Node* transplantNode);
        
        /* Tree: rebalanceTree helpers */
        void adjustKeys(Node* rootNode, std::atomic<int>& currentKey);
        void leftRotate(Node*& rootNode, Node*& pivotNode);
        void rightRotate(Node*& rootNode, Node*& pivotNode);

        /* Tree: API helper functions */
        Node* findNode(int keyToFind, Node* rootNode);
        void getRangeHelper(Node* rootNode, int start, int end, Range& result);
        Node* minimum(Node* node);

        /* Tree: Destructor helper */
        void destroyTree(Node*& node);

        /* Concurrency helpers */
        int fai(std::atomic<int>& x, int amount, std::memory_order MEM);

    public:
        /* Constructors & Destructors */
        Map();
        ~Map();

        /* API Functions */
        /* Modifiers */
        void clear(void);
        void put(int value);
        void remove(int value);

        /* Accessors */
        int at(const int index);
        Range getRange(int start, int end);
        int size(void);
        void printKVPairs(void);
        
        /* Operator overloading */
        int operator[](const int index);
};

#endif /* MAP_H */