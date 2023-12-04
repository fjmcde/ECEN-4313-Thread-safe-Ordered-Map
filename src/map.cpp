#include "../headers/map.h"

Map::Map()
{
    root = nullptr;
}


Map::~Map()
{
    delete root;
}


/************************************
 *        Private Functions         *
 ************************************/

void Map::insertNode(Node* newNode)
{
    // Insert newNode at the root of the tree if
    // the tree is empty
    if(root == nullptr)
    {
        newNode->parent = nullptr;
        newNode->color = black;
        root = newNode;

        return;
    }
    
    Node* current = root;
    while(current != nullptr)
    {
        // Insert into the left path
        // NOTE: Should this compare keys or values?
        if(newNode->value < root->value)
        {
            // Check for left leaf node
            if(current->left == nullptr)
            {
                newNode->parent = current;
                current->left = newNode;
                break;
            }
            // Continue traversing
            else
            {
                current = current->left;
            }
        }
        // Insert into the right path
        else
        {
            // Check for right leaf node
            if(current->right == nullptr)
            {
                newNode->parent = current;
                current->right = newNode;
                break;
            }
            // Continue traversing
            else
            {
                current = current->right;
            }
        }
    }

    newNode->color = red;
    rebalanceTree(newNode);
}


void Map::rebalanceTree(Node* newNode)
{
    while(newNode->parent->color == red)
    {
        // Case 1: Node's parent is the left child of its grandparent
        if(newNode->parent == newNode->parent->parent->left)
        {
            Node* uncle = newNode->parent->parent->right;

            // Case 3: Parent and uncle are both red
            if(uncle->color == red)
            {
                newNode->parent->color = black;
                uncle->color = black;
                newNode->parent->parent->color = red;
                newNode = newNode->parent->parent;
            }
            else
            {
                // Case 4: Node is the right child of the left child of the grandparent
                if(newNode == newNode->parent->right)
                {
                    newNode = newNode->parent;
                    leftRotate(newNode);
                }

                // Case 5: Node is the left child of the left child of the grandparent
                newNode->parent->color = black;
                newNode->parent->parent->color = red;
                rightRotate(newNode->parent->parent);
            }
        }
        // Symmetric cases for a right-leaning tree
        else
        {
            Node* uncle = newNode->parent->parent->left;

            // Case 3 (Symmetric): Parent and uncle are both red
            if(uncle->color == red)
            {
                newNode->parent->color = black;
                uncle->color = black;
                newNode->parent->parent->color = red;
                newNode = newNode->parent->parent;
            }
            else
            {
                // Case 5 (Symmetric): Node is the left child of the left child of the grandparent
                if(newNode == newNode->parent->left)
                {
                    newNode = newNode->parent;
                    rightRotate(newNode);
                }

                newNode->parent->color = black;
                newNode->parent->parent->color = red;
                leftRotate(newNode->parent->parent);
            }
        }
    }

    // Root is always black
    root->color = black;
}


void Map::rightRotate(Node* pivotNode)
{
    // Check if rotation is not needed
    if (pivotNode == nullptr || pivotNode->left == nullptr)
    {
        return;
    }

    Node* leftChild = pivotNode->left;

    // Update leftChild's right subtree
    Node* newLeftChild = leftChild->right;
    leftChild->right = pivotNode;
    pivotNode->parent = leftChild;

    // Update parent pointers
    leftChild->parent = pivotNode->parent;
    if(pivotNode->parent == nullptr)
    {
        root = leftChild;
    }
    else if(pivotNode == pivotNode->parent->left)
    {
        pivotNode->parent->left = leftChild;
    }
    else
    {
        pivotNode->parent->right = leftChild;
    }

    // Attach the original right subtree of leftChild
    pivotNode->left = newLeftChild;
    if(newLeftChild != nullptr)
    {
        newLeftChild->parent = pivotNode;
    }
}


void Map::leftRotate(Node* pivotNode)
{
    // Check if rotation is not needed
    if (pivotNode == nullptr || pivotNode->right == nullptr)
    {
        return;
    }

    Node* rightChild = pivotNode->right;

    // Update rightChild's left subtree
    Node* newRightChild = rightChild->left;
    rightChild->left = pivotNode;
    pivotNode->parent = rightChild;

    // Update parent pointers
    rightChild->parent = pivotNode->parent;
    if(pivotNode->parent == nullptr)
    {
        root = rightChild;
    }
    else if(pivotNode == pivotNode->parent->left)
    {
        pivotNode->parent->left = rightChild;
    }
    else
    {
        pivotNode->parent->right = rightChild;
    }

    // Attach the original left subtree of rightChild
    pivotNode->right = newRightChild;
    if(newRightChild != nullptr)
    {
        newRightChild->parent = pivotNode;
    }
}


void Map::deleteNode(Node* n)
{
    
}


/************************************
 *         Public Functions         *
 ************************************/

int Map::get(int key)
{

}


void Map::put(int value)
{
    Node* newNode = new Node(counter, value);
    insertNode(newNode);
    counter++;
}


void Map::remove(int key)
{

}