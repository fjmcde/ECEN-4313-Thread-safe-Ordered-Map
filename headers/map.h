#ifndef MAP_H
#define MAP_H

#include <stack>
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
        void insertNode(Node* newNode);
        void deleteNode(Node* nodeToDelete);
        void transplantNode(Node* nodeToReplace, Node* transplantNode);
        void deleteFix(Node* node);
        bool isLeaf(const Node* node);
        Node* minimum(Node* node);
        void adjustKeys(Node* rootNode, int& currentKey);


        /* Tree: balancing functions */
        void rebalanceTree(Node* newNode);
        void rightRotate(Node* pivotNode);
        void leftRotate(Node* pivotNode);

        /* Tree: helper functions */
        Node* findNode(int keyToFind, Node* rootNode);
        int getNumNodes(const Node* rootNode);
        void postOrderClear(Node* rootNode);

    public:
        Map();
        ~Map();

        /* Public insertion and deletion */
        void put(int value);
        void remove(int index);

        /* Public Accessors */
        int& at(const int index);
        Range getRange(int start, int end);

        int size(void);
        void clear(void);

        int& operator[](const int index);
};

#endif /* MAP_H */