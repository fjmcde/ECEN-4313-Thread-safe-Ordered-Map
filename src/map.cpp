#include "../headers/map.h"

Map::Map()
{
    root = nullptr;
    counter = 0;
}


Map::~Map()
{
    destroyTree(root);
    root = nullptr;
    counter = 0;
}


/************************************
 *        Private Functions         *
 ************************************/

void Map::deleteNode(Node* nodeToDelete)
{
    if(nodeToDelete == nullptr)
    {
        return;
    }

    Node* successor = nullptr;
    Node* actualReplacementNode = nullptr;
    Node* replacementNode = nodeToDelete;
    RBColor replacementColor = replacementNode->color;

    // Case 1: Node has no children
    if(isLeaf(nodeToDelete))
    {
        successor = nullptr;
    }
    // Case 2: Node has at most one child
    else if(nodeToDelete->left == nullptr)
    {
        successor = nodeToDelete->right;
    }
    else if(nodeToDelete->right == nullptr)
    {
        successor = nodeToDelete->left;
    }
    // Case 3: Node has both children
    else
    {
        successor = minimum(nodeToDelete->right);
        replacementColor = successor->color;
        actualReplacementNode = successor->right;

        if(successor->parent != nodeToDelete)
        {
            transplantNode(successor, successor->right);
            successor->right = nodeToDelete->right;
            successor->right->parent = successor;
        }

        // Transplant successor node
        transplantNode(nodeToDelete, successor);
        successor->left = nodeToDelete->left;
        successor->left->parent = successor;
        successor->color = nodeToDelete->color;
    }

    transplantNode(nodeToDelete, successor);


    if(successor != nullptr)
    {
        counter--;
    }

    if(replacementColor == black)
    {
        deleteFix(successor, actualReplacementNode);
    }

    delete nodeToDelete;

    int currentKey = 0;
    adjustKeys(root, currentKey);
}


Node* Map::insertNode(Node* rootNode, Node* newNode)
{
    if(rootNode == nullptr)
    {
        return newNode;
    }

    Node* current = rootNode;
    Node* parent = nullptr;

    while(current != nullptr)
    {
        // Lock the current node
        // current->nodeLock.lock();

        parent = current;

        if(newNode->value < current->value)
        {
            current = current->left;
        }
        else if(newNode->value > current->value)
        {
            current = current->right;
        }
        else
        {
            // Duplicate values aren't handled yet

            // unlock before returning to prevent deadlock
            // current->nodeLock.unlock();
            return rootNode;
        }

        // Unlock for hand-over-hand locking
        // current->nodeLock.unlock();
    }

    if(parent == nullptr)
    {
        rootNode = newNode;
    }
    else if(newNode->value < parent->value)
    {
        parent->left = newNode;
    }
    else
    {
        parent->right = newNode;
    }

    newNode->parent = parent;

    // Unlock node
    // newNode->nodeLock.unlock();

    return rootNode;
}


void Map::rebalanceTree(Node*& rootNode, Node*& newNode)
{
    Node *parentNode = nullptr;
    Node *grandparentNode = nullptr;
 
    while((newNode != rootNode) && (newNode->color != black) && (newNode->parent->color == red))
    {
        parentNode = newNode->parent;
        grandparentNode = newNode->parent->parent;
 
        // Path A: Parent of newNode is left child of grandparent of newNode
        if(parentNode == grandparentNode->left)
        {
            Node *uncleNode = grandparentNode->right;

            //Case 1: The uncle of newNode is also red
            if(uncleNode != nullptr && uncleNode->color == red)
            {
                grandparentNode->color = red;
                parentNode->color = black;
                uncleNode->color = black;
                newNode = grandparentNode;
            }
            else
            {
                // Case 2: newNode is right child of its parent
                if(newNode == parentNode->right)
                {
                    leftRotate(rootNode, parentNode);
                    newNode = parentNode;
                    parentNode = newNode->parent;
                }
 
                //Case 3: newNode is left child of its parent
                rightRotate(rootNode, grandparentNode);
                std::swap(parentNode->color, grandparentNode->color);
                newNode = parentNode;
            }
        }
        //Path B: Parent of newNode is right child of grandparent of newNode
        else
        {
            Node *uncleNode = grandparentNode->left;
 
            //Case 1: The uncle of newNode is also red
            if((uncleNode != nullptr) && (uncleNode->color == red))
            {
                grandparentNode->color = red;
                parentNode->color = black;
                uncleNode->color = black;
                newNode = grandparentNode;
            }
            else
            {
                // Case 2: newNode is left child of its parent
                if(newNode == parentNode->left)
                {
                    rightRotate(rootNode, parentNode);
                    newNode = parentNode;
                    parentNode = newNode->parent;
                }
 
                //Case 3: newNode is right child of its parent
                leftRotate(rootNode, grandparentNode);
                std::swap(parentNode->color, grandparentNode->color);
                newNode = parentNode;
            }
        }
    }
 
    rootNode->color = black;
    
    // Update the keys after rebalancing
    int currentKey = 0;
    adjustKeys(rootNode, currentKey);
}


void Map::deleteFix(Node*& rootNode, Node*& node)
{
    while((node != nullptr) && (node != root) && (node->color == black))
    {
        Node* siblingNode = nullptr;

        // Node is a left child
        if(node == node->parent->left)
        {
            siblingNode = node->parent->right;

            // Case 1: sibling is red
            if(siblingNode->color == red)
            {
                siblingNode->color = black;
                node->parent->color = red;
                leftRotate(rootNode, node->parent);
                siblingNode = node->parent->right;
            }

            // Case 2: sibling's children are black
            if((siblingNode->left->color == black) && (siblingNode->right->color == black))
            {
                siblingNode->color = red;
                node = node->parent;
            }
            else
            {
                // Case 3: Left child is red; right child is black
                if(siblingNode->right->color == black)
                {
                    siblingNode->left->color = black;
                    siblingNode->color = red;
                    rightRotate(rootNode, siblingNode);
                    siblingNode = node->parent->right;
                }

                // Case 4: Right child is red
                siblingNode->color = node->parent->color;
                node->parent->color = black;
                siblingNode->right->color = black;
                leftRotate(rootNode, node->parent);
                node = root;
            }
        }
        // Node is a right child
        else
        {
            siblingNode = node->parent->left;
            
            // Case 1: sibling is red
            if(siblingNode->color == red)
            {
                siblingNode->color = black;
                node->parent->color = red;
                rightRotate(rootNode, node->parent);
                siblingNode = node->parent->left;
            }

            // Case 2: sibling's children are black
            if((siblingNode->right->color == black) && (siblingNode->left->color == black))
            {
                siblingNode->color = red;
                node = node->parent;
            }
            else
            {
                // Case 3: right child is red; left child is black
                if(siblingNode->left->color == black)
                {
                    siblingNode->right->color = black;
                    siblingNode->color = red;
                    leftRotate(rootNode, siblingNode);
                    siblingNode = node->parent->left;
                }

                // Case 4: left child is red
                siblingNode->color = node->parent->color;
                node->parent->color = black;
                siblingNode->left->color = black;
                rightRotate(rootNode, node->parent);
                node = root;
            }
        }
    }

    if(node != nullptr)
    {
        node->color = black;
    }
}


Node* Map::findValue(int valueToFind, Node* rootNode)
{
    if(rootNode == nullptr || valueToFind == rootNode->value)
    {
        return rootNode;
    }

    if(valueToFind < rootNode->value)
    {
        return findValue(valueToFind, rootNode->left);
    }
    else
    {
        return findValue(valueToFind, rootNode->right);
    }
}


bool Map::isLeaf(const Node* node)
{
    // A node is a leaf is if has no child nodes
    if(node->left == nullptr && node->right == nullptr)
    {
        return true;
    }

    return false;
}


void Map::transplantNode(Node* nodeToReplace, Node* transplantNode)
{
    // Transplant node to root
    if(nodeToReplace->parent == nullptr)
    {
        root = transplantNode;
    }
    // Transplant node to left child
    else if(nodeToReplace == nodeToReplace->parent->left)
    {
        nodeToReplace->parent->left = transplantNode;
    }
    // Transplant node to right child
    else
    {
        nodeToReplace->parent->right = transplantNode;
    }

    // Update parent
    if(transplantNode != nullptr)
    {
        transplantNode->parent = nodeToReplace->parent;
    }
}


void Map::adjustKeys(Node* rootNode, int& currentKey)
{
    if(rootNode != nullptr)
    {
        adjustKeys(rootNode->left, currentKey);
        rootNode->key = currentKey++;
        adjustKeys(rootNode->right, currentKey); 
    }
}


void Map::leftRotate(Node*& rootNode, Node*& pivotNode)
{
    Node* rightChild = pivotNode->right;
 
    pivotNode->right = rightChild->left;
 
    if(pivotNode->right != nullptr)
    {
        pivotNode->right->parent = pivotNode;
    }
 
    rightChild->parent = pivotNode->parent;
 
    if(pivotNode->parent == nullptr)
    {
        rootNode = rightChild;
    }
 
    else if(pivotNode == pivotNode->parent->left)
    {
        pivotNode->parent->left = rightChild;
    }
    else
    {
        pivotNode->parent->right = rightChild;
    }

    rightChild->left = pivotNode;
    pivotNode->parent = rightChild;

    // Update the keys after rebalancing
    int currentKey = 0;
    adjustKeys(rootNode, currentKey);
}


void Map::rightRotate(Node*& rootNode, Node*& pivotNode)
{
    Node* leftChild = pivotNode->left;
 
    pivotNode->left = leftChild->right;
 
    if(pivotNode->left != nullptr)
        pivotNode->left->parent = pivotNode;
 
    leftChild->parent = pivotNode->parent;
 
    if(pivotNode->parent == nullptr)
        rootNode = leftChild;
 
    else if(pivotNode == pivotNode->parent->left)
        pivotNode->parent->left = leftChild;
 
    else
        pivotNode->parent->right = leftChild;
 
    leftChild->right = pivotNode;
    pivotNode->parent = leftChild;

    // Update the keys after rebalancing
    int currentKey = 0;
    adjustKeys(rootNode, currentKey);
}


Node* Map::findNode(int keyToFind, Node* rootNode)
{
    // Check the root node
    if(rootNode == nullptr || keyToFind == rootNode->key)
    {
        return rootNode;
    }

    // Determine path to check
    if(keyToFind < rootNode->key)
    {
        return findNode(keyToFind, rootNode->left);
    }
    else
    {
        return findNode(keyToFind, rootNode->right);
    }
}


int Map::getNumNodes(const Node* rootNode)
{
    // RB Tree is empty
    if(rootNode == nullptr)
    {
        return 0;
    }

    // Calculate the number of nodes in each path
    // NOTE: This would be a prime candidate for concurrency;
    //       One thread per path using fork/join parallelism.
    int leftSize = getNumNodes(rootNode->left);
    int rightSize = getNumNodes(rootNode->right);

    // Return total number of nodes (plus 1 for the root node)
    return leftSize + rightSize + 1;
}


void Map::getRangeHelper(Node* rootNode, int start, int end, Range& result)
{
    if (rootNode == nullptr) {
        return;
    }

    // In-order traversal
    getRangeHelper(rootNode->left, start, end, result);

    // Check if the current node's key is within the specified range
    if((rootNode->value >= start) && (rootNode->value <= end))
    {
        result.push_back(std::make_pair(rootNode->key, rootNode->value));
    }

    getRangeHelper(rootNode->right, start, end, result);
}


Node* Map::minimum(Node* node)
{
    // Find the minimum node in subtree
    while(node->left != nullptr)
    {
        node = node->left;
    }

    return node;
}


void Map::destroyTree(Node* node)
{
    if(node != nullptr)
    {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}


/************************************
 *         Public Functions         *
 ************************************/

void Map::clear(void)
{
    destroyTree(root);
    root = nullptr;
    counter = 0;
}


void Map::put(int value)
{
    Node* newNode = new Node(++counter, value);
    root = insertNode(root, newNode);

    int currentKey = 0;
    adjustKeys(root, currentKey);
}


void Map::remove(int value)
{
    Node* nodeToDelete = findValue(value, root);

    if(nodeToDelete != nullptr)
    {
        deleteNode(nodeToDelete);
        counter--;
    }
}


int& Map::at(const int index)
{
    Node* n = findNode(index, root);

    if(n != nullptr)
    {
        return n->value;
    }
    else
    {
        throw std::out_of_range("Map::get(): Value not found");
    }
    
}


Range Map::getRange(int start, int end)
{
    Range result;
    getRangeHelper(root, start, end, result);
    return result;
}


int Map::size(void)
{
    // return getNumNodes(root);
    return counter;
}


int& Map::operator[](const int index)
{
    return at(index);
}
