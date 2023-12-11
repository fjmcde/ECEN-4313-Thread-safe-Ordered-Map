#include "../headers/map.h"

Map::Map()
{
    root = nullptr;
    counter = 0;
}


Map::~Map()
{
    std::stack<Node*> nodeStack;
    Node* currentNode = root;
    Node* lastNode = nullptr;

    while((currentNode != nullptr) || !nodeStack.empty())
    {
        while(currentNode != nullptr)
        {
            nodeStack.push(currentNode);
            currentNode = currentNode->left;
        }

        Node* peek = nodeStack.top();

        if((peek->right != nullptr) && (peek->right != lastNode))
        {
            currentNode = peek->right;
        }
        else
        {
            delete peek;
        }
    }

    root = nullptr;
    counter = 0;
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
        newNode->key = ++counter;
        root = newNode;

        return;
    }
    
    Node* current = root;
    while(current != nullptr)
    {
        // Insert into the left path
        // NOTE: Should this compare keys or values?
        if(newNode->key < current->key)
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
    newNode->key = ++counter;
    rebalanceTree(newNode);

    int currentKey = 0;
    adjustKeys(root, currentKey);
}


void Map::deleteNode(Node* nodeToDelete)
{
    if(nodeToDelete == nullptr)
    {
        return;
    }

    Node* successor;
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
        delete nodeToDelete;
        counter--;
    }

    if(replacementColor == black)
    {
        deleteFix(actualReplacementNode);
    }

    int currentKey = 0;
    adjustKeys(root, currentKey);
}


void Map::deleteFix(Node* node)
{
    while((node != root) && (node->color == black))
    {
        Node* siblingNode;

        // Node is a left child
        if(node == node->parent->left)
        {
            siblingNode = node->parent->right;

            // Case 1: sibling is red
            if(siblingNode->color == red)
            {
                siblingNode->color = black;
                node->parent->color = red;
                leftRotate(node->parent);
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
                    rightRotate(siblingNode);
                    siblingNode = node->parent->right;
                }

                // Case 4: Right child is red
                siblingNode->color = node->parent->color;
                node->parent->color = black;
                siblingNode->right->color = black;
                leftRotate(node->parent);
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
                rightRotate(node->parent);
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
                    leftRotate(siblingNode);
                    siblingNode = node->parent->left;
                }

                // Case 4: left child is red
                siblingNode->color = node->parent->color;
                node->parent->color = black;
                siblingNode->left->color = black;
                rightRotate(node->parent);
                node = root;
            }
        }
    }

    if(node != nullptr)
    {
        node->parent = nullptr;
        node->color = black;
    }
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


bool Map::isLeaf(const Node* node)
{
    // A node is a leaf is if has no child nodes
    if(node->left == nullptr && node->right == nullptr)
    {
        return true;
    }

    return false;
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
    int currentKey = 0;
    adjustKeys(root, currentKey);
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

    // Adjust keys to maintain consistent indexes
    pivotNode->key = leftChild->key;
    leftChild->key = pivotNode->key + 1;
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

    // Adjust keys to maintain consistent indexes
    pivotNode->key = rightChild->key;
    rightChild->key = pivotNode->key + 1;
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


int& Map::operator[](const int index)
{
    return at(index);
}


/************************************
 *         Public Functions         *
 ************************************/

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


void Map::put(int value)
{
    Node* newNode = new Node(++counter, value);
    DEBUG_PRINT("newNode value:", newNode->value);
    insertNode(newNode);
}


void Map::remove(int index)
{
    Node* nodeToDelete = findNode(index, root);

    if(nodeToDelete != nullptr)
    {
        deleteNode(nodeToDelete);
        counter--;
    }
}


// Range Map::getRange(int start, int end)
// {

// }


int Map::size(void)
{
    // return getNumNodes(root);
    return counter;
}


void Map::clear(void)
{
    std::stack<Node*> nodeStack;
    Node* currentNode = root;

    while((currentNode != nullptr) || (!nodeStack.empty()))
    {
        // Traverse the left tree
        while(currentNode != nullptr)
        {
            nodeStack.push(currentNode);
            currentNode = currentNode->left;
        }

        // Pop the top of the stack
        currentNode = nodeStack.top();
        nodeStack.pop();

        // Store the right subtree so traversal can continue
        Node* rightSubTree = currentNode->right;

        // Delete current Node
        delete currentNode;

        // Prepare to traverse the right subtree
        currentNode = rightSubTree;
    }

    // Reset the root and counter so that the Map can be reused
    root = nullptr;
    counter = 0;
}