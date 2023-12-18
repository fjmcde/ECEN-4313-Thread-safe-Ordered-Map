#include "../headers/map.h"

Map::Map()
{
    this->root = nullptr;
    fai(this->counter, 0 - this->size(), SEQCST);
}


Map::~Map()
{
    DEBUG_PRINT("Map::%s()\n", __FUNCTION__);
    this->clear();
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

    nodeToDelete->nodeLock.lock();
    
    Node* successor = nullptr;
    Node* actualReplacementNode = nullptr;

    // Case 1: Node has no children
    if(isLeaf(nodeToDelete))
    {
        successor = nullptr;
    }
    // Case 2: Node has at most one child
    else if(nodeToDelete->left == nullptr)
    {
        nodeToDelete->right->nodeLock.lock();
        successor = nodeToDelete->right;
    }
    else if(nodeToDelete->right == nullptr)
    {
        nodeToDelete->left->nodeLock.lock();
        successor = nodeToDelete->left;
    }
    // Case 3: Node has both children
    else
    {
        nodeToDelete->left->nodeLock.lock();
        nodeToDelete->right->nodeLock.lock();

        // Traverse the right subtree of the node we want to delete, to find
        // the smallest value in that subtree. That smallest value will be the
        // node which replaces the node we want to delete. The minimum value in
        // the right subtree is greater than the node to delete, but smaller
        // than the right subtree root.
        successor = minimum(nodeToDelete->right);
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

    if(nodeToDelete->color == black)
    {
        deleteFix(successor, actualReplacementNode);
    }

    if(nodeToDelete->left != nullptr)
    {
        nodeToDelete->left->nodeLock.unlock();
    }
    if(nodeToDelete->right != nullptr)
    {
        nodeToDelete->right->nodeLock.unlock();
    }

    nodeToDelete->nodeLock.unlock();
    delete nodeToDelete;
}


Node* Map::insertNode(Node*& rootNode, Node* newNode)
{
    if(rootNode == nullptr)
    {
        return newNode;
    }

    rootNode->nodeLock.lock();

    Node* current = rootNode;
    Node* parent = nullptr;

    while(current != nullptr)
    {
        parent = current;

        if(newNode->value < fai(current->value, 0, SEQCST))
        {
            current = current->left;
        }
        else if(newNode->value > fai(current->value, 0, SEQCST))
        {
            current = current->right;
        }
        else
        {
            // Duplicate values aren't handled yet

            // unlock before returning to prevent deadlock
            rootNode->nodeLock.unlock();
            return rootNode;
        }
    }

    newNode->nodeLock.lock();

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
    newNode->nodeLock.unlock();
    rootNode->nodeLock.unlock();

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
    std::atomic<int> currentKey = 0;
    rootNode->nodeLock.lock();
    adjustKeys(rootNode, currentKey);
    rootNode->nodeLock.unlock();
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
            if((siblingNode != nullptr) && (siblingNode->color == red))
            {
                siblingNode->color = black;
                node->parent->color = red;
                leftRotate(rootNode, node->parent);
                siblingNode = node->parent->right;
            }

            // Case 2: sibling's children are black
            if(((siblingNode == nullptr) ||
               (siblingNode->left == nullptr) || (siblingNode->left->color == black)) &&
               ((siblingNode->right == nullptr) || (siblingNode->right->color == black)))
            {
                if(siblingNode != nullptr)
                {
                    siblingNode->color = red;
                    node = node->parent;
                }
            }
            else
            {
                // Case 3: Left child is red; right child is black
                if((siblingNode->right == nullptr) || (siblingNode->right->color == black))
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
            if((siblingNode != nullptr) && (siblingNode->color == red))
            {
                siblingNode->color = black;
                node->parent->color = red;
                rightRotate(rootNode, node->parent);
                siblingNode = node->parent->left;
            }

            // Case 2: sibling's children are black
            if(((siblingNode == nullptr) ||
               ((siblingNode->right == nullptr) || (siblingNode->right->color == black))) &&
               (((siblingNode->left == nullptr) || (siblingNode->left->color == black))))
            {
                if(siblingNode != nullptr)
                {
                    siblingNode->color = red;
                    node = node->parent;
                }
            }
            else
            {
                // Case 3: right child is red; left child is black
                if((siblingNode->left == nullptr) || (siblingNode->left->color == black))
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
    while((rootNode != nullptr) && (valueToFind != fai(rootNode->value, 0, SEQCST)))
    {
        if(valueToFind < fai(rootNode->value, 0, SEQCST))
        {
            rootNode = rootNode->left;
        }
        else
        {
            rootNode = rootNode->right;
        }
    }

    return rootNode;
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


void Map::adjustKeys(Node* rootNode, std::atomic<int>& currentKey)
{
    Node* current = rootNode;
    Node* previous;

    while(current != nullptr)
    {
        if(current->left == nullptr)
        {
            current->key = fai(currentKey, 1, SEQCST);
            current = current->right;
        }
        else
        {
            previous = current->left;

            while((previous->right != nullptr) && (previous->right != current))
            {
                previous = previous->right;
            }

            if(previous->right == nullptr)
            {
                previous->right = current;
                current = current->left;
            }
            else
            {
                current->key = fai(currentKey, 1, SEQCST);
                previous->right = nullptr;
                current = current->right;
            }
        }
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
}


void Map::rightRotate(Node*& rootNode, Node*& pivotNode)
{
    Node* leftChild = pivotNode->left;
 
    pivotNode->left = leftChild->right;
 
    if(pivotNode->left != nullptr)
    {
        pivotNode->left->parent = pivotNode;
    }
 
    leftChild->parent = pivotNode->parent;
 
    if(pivotNode->parent == nullptr)
    {
        rootNode = leftChild;
    }
 
    else if(pivotNode == pivotNode->parent->left)
    {
        pivotNode->parent->left = leftChild;
    }
    else
    {
        pivotNode->parent->right = leftChild;
    }
 
    leftChild->right = pivotNode;
    pivotNode->parent = leftChild;
}


Node* Map::findNode(int keyToFind, Node* rootNode)
{
    // Check the root node
    if((rootNode == nullptr) || (keyToFind == fai(rootNode->key, 0, SEQCST)))
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


void Map::getRangeHelper(Node* rootNode, int start, int end, Range& result)
{
    if(rootNode == nullptr)
    {
        return;
    }

    // Lock the current rootNode of the subtree
    rootNode->nodeLock.lock();

    // Check if the current node's key is within the specified range
    if((fai(rootNode->value, 0, SEQCST) >= start) && (fai(rootNode->value, 0, SEQCST) <= end))
    {
        result.push_back(std::make_pair(static_cast<int>(fai(rootNode->key, 0, SEQCST)), static_cast<int>(fai(rootNode->value, 0, SEQCST))));
    }

    // Unlock the current rootNode of the subtree
    // We do this prior to in-order traversal to
    // achieve hand-over-hand locking
    rootNode->nodeLock.unlock();

    // In-order traversal
    getRangeHelper(rootNode->left, start, end, result);
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


void Map::destroyTree(Node*& node)
{
    if (node != nullptr)
    {
        if (node->left != nullptr)
        {
            destroyTree(node->left);
        }

        if (node->right != nullptr)
        {
            destroyTree(node->right);
        }

        delete node;
    }
}



int Map::fai(std::atomic<int>& x, int amount, std::memory_order MEM)
{
    return x.fetch_add(amount, MEM);
}

/************************************
 *         Public Functions         *
 ************************************/

void Map::clear(void)
{
    DEBUG_PRINT("Map::%s()\n", __FUNCTION__);

    while (this->root != nullptr)
    {
        Node* current = this->root;

        // Traverse to the leftmost leaf
        while (current->left != nullptr)
        {
            current = current->left;
        }

        // Update the tree structure
        if (current->right != nullptr)
        {
            current->right->parent = current->parent;
        }

        Node* parent = current->parent;
        if (parent != nullptr)
        {
            if (parent->left == current)
                parent->left = current->right;
            else
                parent->right = current->right;
        }
        else
        {
            this->root = current->right;
        }

        delete current;
    }

    this->counter = 0;

    DEBUG_PRINT("COUNTER AFTER CLEAR: %d\n", static_cast<int>(counter));
    DEBUG_PRINT("Map Cleared!\nMap.size(): %d\n", this->size());
}


void Map::put(int value)
{
    DEBUG_PRINT("Map::%s(%d)\n", __FUNCTION__, value);

    fai(this->counter, 1, SEQCST);
    Node* newNode = new Node(this->counter, value);
    this->root = insertNode(this->root, newNode);

    // std::atomic<int> currentKey = 0;
    
    this->rebalanceTree(root, newNode);
    DEBUG_PRINT("Inserted value(%d): Counter: %d\n", value, fai(this->counter, 0, SEQCST));
}



void Map::remove(int value)
{
    DEBUG_PRINT("Map::%s(%d)\n", __FUNCTION__, value);
    Node* nodeToDelete = findValue(value, this->root);

    if(nodeToDelete != nullptr)
    {
        deleteNode(nodeToDelete);
    }

    fai(counter, -1, SEQCST);

    this->root->nodeLock.lock();
    std::atomic<int> currentKey = 0;
    adjustKeys(this->root, currentKey);
    this->root->nodeLock.unlock();

    DEBUG_PRINT("Deleted value(%d): Counter: %d\n", value, fai(counter, 0, SEQCST));
}


int Map::at(const int index)
{
    Node* n = findNode(index, this->root);

    
        if (n == nullptr)
        {
            return -1;
        }

        return fai(n->value, 0, SEQCST);
}



/// @brief Retrieves all values within the given integer range (inclusive)
/// @param start    Starting value
/// @param end      Ending value
/// @return         Vector of [unsorted] key/value pairs where key is result.first
///                 and value is result.second
Range Map::getRange(int start, int end)
{
    Range result = {};

    // Return null it tree is empty
    if(this->root == nullptr)
    {
        return result;
    }

    this->getRangeHelper(this->root, start, end, result);

    std::sort(result.begin(), result.end());

    return result;
}


int Map::size(void)
{
    return fai(this->counter, 0, SEQCST);
}


void Map::printKVPairs(void)
{
    DEBUG_PRINT("\nPrinting all K/V pairs%s", "\n");
    Range fullRange = getRange(0, 1000);

    printf("[");
    for(size_t i = 0; i < fullRange.size(); i++)
    {
        printf(" {%d / %d} ", fullRange[i].first, fullRange[i].second);
    }
    printf("]\n");
}


int Map::operator[](const int index)
{
    return this->at(index);
}
