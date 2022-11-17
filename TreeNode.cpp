//
// Implements the TreeNode Class
// Author: Max Benson
// Date: 10/27/2021
//

#include <assert.h>
#include "TreeNode.h"

using std::cout;
using std::endl;

//void RecursiveDeleteTree(TreeNode* node);

/**
 * Constructor
 * @param nodeType one of Operator, NumberOperand, or VariableOperand
 * @param data an operator (+, -, *), a number, or a variable name
 */
TreeNode::TreeNode(NodeType nodeType, string data) {
    _nodeType = nodeType;
    _data = data;
}

/**
 * Destructor
 * Frees allocated memory
 */
TreeNode::~TreeNode() {

    delete _left;
    delete _right;
    // delete this; // Uhhhhh i don't think i want this... but maybe I do? it is here just in case I realize it is important :)

    /* Old code. I'm 99% sure it's just objectively worse than what I have now. But it worked at least, I think.
    cout << "MAIN TreeNode() DESTRUCTOR CALLED" << endl;
    if (this->Left() == nullptr && this->Right() == nullptr) {
        delete this;
    } else {
        RecursiveDeleteTree(this);
    }
     */
}

/*
void RecursiveDeleteTree(TreeNode* node) { // Very extra. Did not need to write. Not being used, but might come back to it if I need to.
    cout << "Recursive delete called" << endl;
    if (node != nullptr) {
        cout << "Recursive delete is actually deleting something" << endl;
        RecursiveDeleteTree(node->Left());
        RecursiveDeleteTree(node->Right());
        delete node;
    }
}
 */


/**
 * If it's a multiplcation node, and left is a number, return number on left, and expression tree on right
 * @param c receives number
 * @param ptree receives pointer to expression tree
 * @return true if node is a multiplication of number * exp, false otherwise
 */
bool TreeNode::SplitNumTimesVariable(int& c, TreeNode** ptree) const {
    assert(false);
    return false;
}
