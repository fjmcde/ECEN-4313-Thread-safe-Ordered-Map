#ifndef NODE_H
#define NODE_H

#ifdef DEBUG
    #define DEBUG_NODE(node)                                                                                                                                                    \
    {                                                                                                                                                                           \
        printf("\nAddr: %p, parent: %p, left: %p, right: %p, key: %d, value: %d, color: %d\n", node, node->parent, node->left, node->right, node->key, node->value, node->color); \
    }
#else
    #define DEBUG_NODE(...) do{}while(0)
#endif


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
};

#endif /* NODE_H */