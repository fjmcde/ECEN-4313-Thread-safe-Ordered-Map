#ifndef MAP_H
#define MAP_H

#include "node.h"

class Map
{
    private:
        Node* root;
        int counter;

        /* Tree: Node functions */
        void insertNode(Node* n);
        void deleteNode(Node* n);

        /* Tree: balancing functions */
        void rebalanceTree(Node* newNode);
        void leftRotate(Node* pivotNode);
        void rightRotate(Node* pivotNode);

        /* Tree deletion: helper functions */
        void findNode(int valueToFind);

    public:
        Map();
        ~Map();

        int get(int key);
        void put(int value);
        void remove(int key);
};

#endif /* MAP_H */