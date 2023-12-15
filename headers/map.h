#ifndef MAP_H
#define MAP_H

#include <stdexcept>
#include <utility>
#include <vector>

#include "global.h"
#include "node.h"


typedef std::vector<std::pair<int, int>> Range;

class Map
{
    private:
        Node* root;
        int counter;

        /* Tree: Node functions */
        Node* insertNode(Node* rootNode, Node* newNode);
        void deleteNode(Node* nodeToDelete);
        void transplantNode(Node* nodeToReplace, Node* transplantNode);
        void deleteFix(Node*& rootNode, Node*& node);
        bool isLeaf(const Node* node);
        Node* minimum(Node* node);
        void adjustKeys(Node* rootNode, int& currentKey);


        /* Tree: balancing functions */
        void rebalanceTree(Node*& rootNode, Node*& newNode);
        void rightRotate(Node*& rootNode, Node*& pivotNode);
        void leftRotate(Node*& rootNode, Node*& pivotNode);

        /* Tree: helper functions */
        Node* findNode(int keyToFind, Node* rootNode);
        Node* findValue(int valueToFind, Node* rootNode);
        int getNumNodes(const Node* rootNode);
        void postOrderClear(Node* rootNode);
        void destroyTree(Node* node);

    public:
        Map();
        ~Map();

        /* API Functions */
        void put(int value);
        void remove(int value);
        int& at(const int index);
        // Range getRange(int start, int end);
        int size(void);
        void clear(void);

        /* Operator overloading */
        int& operator[](const int index);
};

#endif /* MAP_H */