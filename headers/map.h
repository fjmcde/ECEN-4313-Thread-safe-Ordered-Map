#ifndef MAP_H
#define MAP_H

#include <utility>
#include <vector>

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

        /* Tree: balancing functions */
        void rebalanceTree(Node* newNode);
        void rightRotate(Node* pivotNode);
        void leftRotate(Node* pivotNode);

        /* Tree: helper functions */
        Node* findNode(int keyToFind, Node* rootNode);
        int getNumNodes(Node* rootNode);
        void postOrderClear(Node* rootNode);

    public:
        Map();
        ~Map();

        /* Public insertion and deletion */
        void put(int value);
        void remove(int index);

        /* Public Accessors */
        int get(int index);
        Range getRange(int start, int end);

        int size(void);
        void clear(void);
};

#endif /* MAP_H */