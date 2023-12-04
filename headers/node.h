#ifndef NODE_H
#define NODE_H

enum RBColor : int { red, black };

struct Node
{
    int key;
    int value;

    Node* parent;
    Node* left;
    Node* right;
    RBColor color;

    Node(int k, int v)
    {
        // Set key/value
        key = k;
        value = v;

        // Initialize pointers to null
        parent = nullptr;
        left = nullptr;
        right = nullptr;

        // Default color is black
        color = black;
    }

    ~Node()
    {
        // Parent nodes manage their children's memory
        delete left;
        delete right;
    }
};

#endif /* NODE_H */