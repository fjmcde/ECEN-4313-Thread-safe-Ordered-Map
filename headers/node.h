#ifndef NODE_H
#define NODE_H

#include <mutex>

#ifdef DEBUG
    #define DEBUG_NODE(node)                                                                                                                                                    \
    {                                                                                                                                                                           \
        printf("\nAddr: %p, parent: %p, left: %p, right: %p, key: %d, value: %d, color: %d\n", node, node->parent, node->left, node->right, node->key, node->value, node->color); \
    }
#else
    #define DEBUG_NODE(...) do{}while(0)
#endif

#define SEQCST      std::memory_order_seq_cst

enum RBColor : int { red, black };

struct Node
{
    // nodeLock provides more fine-grain locking over 
    // using a single-global lock for the whole tree
    std::mutex nodeLock;

    std::atomic<int> key;
    std::atomic<int> value;

    Node* parent;
    Node* left;
    Node* right;
    RBColor color;

    Node(int k, int v)
    {
        // Set key/value
        key.store(k, SEQCST);
        value.store(v);

        // Initialize pointers to null
        parent = nullptr;
        left = nullptr;
        right = nullptr;

        // Default color is black
        color = black;
    }
};

#endif /* NODE_H */