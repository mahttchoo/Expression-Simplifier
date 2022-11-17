//
// Implements the ExpressionTree Class
// Author: Max Benson
// Date: 10/27/2021
//

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

#include "Stack.h"
#include "ExpressionTree.h"

// Token testing routines
bool IsNumber(string token);
bool IsVariable(string token);
bool IsOperator(string token);

/**
 * Default constructor
 * Creates an "null tree"
 */
ExpressionTree::ExpressionTree() {
    _root = nullptr;
}

/**
 * Destructor
 * Frees the dynamic memory allocated for the tree
 */
ExpressionTree::~ExpressionTree() {
    delete _root; // This should call the destructor in TreeNode
}

/**
 * Build an expression tree from its postfix representation
 * In case of error the stack is cleaned up.  Because it contains
 * pointers to TreeNodes, if any are left on the stack they must be
 * explicitly deleted
 * @param postfix string representation of tree
 * @return true if postfix valid and tree was built, false otherwise
 */
bool ExpressionTree::BuildExpressionTree(const string& postfix) {
    stringstream ss(postfix);
    string token;
    Stack<TreeNode*> nodeStack;

    while(ss >> token) {
        if (IsOperator(token) == false) {
            NodeType nodeType;

            if (IsNumber(token) == true) {
                nodeType = NumberOperand;
            } else if (IsVariable(token) == true) {
                nodeType = VariableOperand;
            } else {
                cout << "EXPRESSION DOESN'T MAKE SENSE" << endl;
                cout << "       Invalid Token" << endl;
                for (size_t i = 0; i < nodeStack.Size(); i++) {
                    delete nodeStack.Pop(); // Is this actually calling the TreeNode destructor? Or not since this itself is a pointer?
                }
                return false;
            }

            TreeNode* node = new TreeNode(nodeType, token);
            nodeStack.Push(node);
        } else {
            if (nodeStack.Size() < 2) {
                cout << "EXPRESSION DOESN'T MAKE SENSE" << endl;
                cout << "       Too many Operators or out of order" << endl;
                for (size_t i = 0; i < nodeStack.Size(); i++) {
                    delete nodeStack.Pop(); // Is this actually calling the TreeNode destructor? Or not since this itself is a pointer?
                }
                return false;
            }
            TreeNode* node = new TreeNode(Operator, token);
            node->SetRight(nodeStack.Pop());
            node->SetLeft(nodeStack.Pop());
            nodeStack.Push(node);
        }
    }

    if (nodeStack.Size() == 1) {
        _root = nodeStack.Pop();
        return true;
    } else {
        cout << "EXPRESSION DOESN'T MAKE SENSE" << endl;
        cout << "       Too many Operands" << endl;
        for (size_t i = 0; i < nodeStack.Size(); i++) {
            delete nodeStack.Pop(); // Is this actually calling the TreeNode destructor? Or not since this itself is a pointer?
        }
        return false;
    }
}

/**
 * Recursively simplify an expression stored in an expression tree.  The following simplications are performed
 * - Addition, multiplication, and subtraction of constants is performed reducing the subtree to a leaf containing a number
 * - 0 + exp, exp + 0, exp - 0  will be reduced to exp, in general exp will a tree
 * - 1 * exp, exp * 1  will be reduced to exp, in general exp will a tree
 * - 0 * exp, exp * 0  will be reduce to a leaf containing 0
 * - exp - exp will be reduce to a leaf containing 0
 * - exp * number will be changed to number * exp
 * - (c1 * exp) + (c2 * exp) where c1, c2 are numbers  will be changed to (c1+c2) * exp
 * - (c1 * exp) - (c2 * exp) where c1, c2 are numbers will be changed to (c1-c2) * exp
 * pointers to TreeNodes, if any are left on the stack they must be
 * explicitly deleted
 * @param postfix string representation of tree
 * @return true if postfix valid and tree was built, false otherwise
 */
TreeNode* ExpressionTree::SimplifyTree(TreeNode* tree) {
    if (tree != nullptr) { // prevents segmentation faults. very good if statement. much appreciative.
        TreeNode* leftSubTree = SimplifyTree(tree->Left());
        TreeNode* rightSubTree = SimplifyTree(tree->Right());
        if (tree->Type() == Operator) {
            if (tree->Data() == "+") {
                if (leftSubTree->Type() == NumberOperand && rightSubTree->Type() == NumberOperand) { // Adding two numbers
                    int sum = stoi(leftSubTree->Data()) + stoi(rightSubTree->Data());
                    string newData = to_string(sum);
                    TreeNode* newNode = new TreeNode(NumberOperand, newData);
                    //delete tree; // This line is creating double frees. I don't really know how, I feel like not having this makes memory leaks.
                    return newNode;
                }
                if (leftSubTree->Data() == "0") { // Adding a variable/expression to 0
                    delete leftSubTree;
                    return rightSubTree;
                }
                if (rightSubTree->Data() == "0") { // Adding 0 to a variable/expression
                    delete rightSubTree;
                    return leftSubTree;
                }
                if (leftSubTree->Data() == "*" && rightSubTree->Data() == "*") { // Distributative Property. Also I think this specifically is egregiously bad when it comes to memory leaks.
                    if (leftSubTree->Left()->Type() == NumberOperand && rightSubTree->Left()->Type() == NumberOperand) {
                        if (IsSameTree(leftSubTree->Right(), rightSubTree->Right())) {
                            int sum = stoi(leftSubTree->Left()->Data()) + stoi(rightSubTree->Left()->Data());
                            string newData = "*";
                            TreeNode* newNode = new TreeNode(Operator, newData);
                            TreeNode* newLeft = new TreeNode(NumberOperand, to_string(sum));
                            TreeNode* newRight = leftSubTree->Right();
                            newNode->SetLeft(newLeft);
                            newNode->SetRight(newRight);
                            //delete leftSubTree; // Having these statements break my code :). They turn my expression into a happy little box with an x. I do not know why :).
                            //delete rightSubTree;
                            return newNode;
                        }
                    }
                }
            }
            if (tree->Data() == "-") {
                if (leftSubTree->Type() == NumberOperand && rightSubTree->Type() == NumberOperand) { // Subtracting two numbers
                    int diff = stoi(leftSubTree->Data()) - stoi(rightSubTree->Data());
                    string newData = to_string(diff);
                    TreeNode* newNode = new TreeNode(NumberOperand, newData);
                    //delete tree; This line creates a double free.
                    return newNode;
                }
                if (rightSubTree->Data() == "0") { // subtracting 0 from a variable/expression
                    delete rightSubTree;
                    return leftSubTree;
                }
                if (IsSameTree(rightSubTree, leftSubTree)) { // subtracting two equivalent variables/expressions
                    string newData = "0";
                    TreeNode* newNode = new TreeNode(NumberOperand, newData);
                    delete tree;
                    return newNode;
                }
                if (leftSubTree->Data() == "*" && rightSubTree->Data() == "*") { // Distributative Property. Also I think this specifically is egregiously bad when it comes to memory leaks.
                    if (leftSubTree->Left()->Type() == NumberOperand && rightSubTree->Left()->Type() == NumberOperand) {
                        if (IsSameTree(leftSubTree->Right(), rightSubTree->Right())) {
                            int diff = stoi(leftSubTree->Left()->Data()) - stoi(rightSubTree->Left()->Data());
                            string newData = "*";
                            TreeNode* newNode = new TreeNode(Operator, newData);
                            TreeNode* newLeft = new TreeNode(NumberOperand, to_string(diff));
                            TreeNode* newRight = leftSubTree->Right();
                            newNode->SetLeft(newLeft);
                            newNode->SetRight(newRight);
                            return newNode;
                        }
                    }
                }
            }
            if (tree->Data() == "*") {
                if (leftSubTree->Type() == NumberOperand && rightSubTree->Type() == NumberOperand) { // multiplying two numbers
                    int prod = stoi(leftSubTree->Data()) * stoi(rightSubTree->Data());
                    string newData = to_string(prod);
                    TreeNode* newNode = new TreeNode(NumberOperand, newData);
                    //delete tree; // This line creates a double free.
                    return newNode;
                }
                if (leftSubTree->Data() == "0" || rightSubTree->Data() == "0") { // multiplying by 0
                    string newData = "0";
                    TreeNode* newNode = new TreeNode(NumberOperand, newData);
                    delete tree;
                    return newNode;
                }
                if (leftSubTree->Data() == "1") { // multiplying 1 by variable/expression
                    delete leftSubTree;
                    return rightSubTree;
                }
                if (rightSubTree->Data() == "1") { // multiplying variable/expression by 1
                    delete rightSubTree;
                    return leftSubTree;
                }
                if (rightSubTree->Type() == NumberOperand) { // puts the number on the left if variable/expression is being multiplied by non-zero non-one int
                    tree->SetLeft(rightSubTree);
                    tree->SetRight(leftSubTree);
                    return tree;
                }
            }
        }
        tree->SetLeft(leftSubTree);
        tree->SetRight(rightSubTree);
    }
    return tree;
}

/**
 * Determine whether two tree structures represent the same expression
 * @param tree1 first tree structure
 * @param tree2 second tree structure
 * @return true if same, false otherwise
 */
bool ExpressionTree::IsSameTree(TreeNode* tree1, TreeNode* tree2) const {
    // These operations ARE commutative.
    if (tree1->Data() == "*" && tree2->Data() == "*") {
        if (IsSameTree(tree1->Left(), tree2->Left()) && IsSameTree(tree1->Right(), tree2->Right())) {
            return true;
        }
        if (IsSameTree(tree1->Left(), tree2->Right()) && IsSameTree(tree1->Right(), tree2->Left())) {
            return true;
        }
        return false;
    }
    if (tree1->Data() == "+" && tree2->Data() == "+") {
        if (IsSameTree(tree1->Left(), tree2->Left()) && IsSameTree(tree1->Right(), tree2->Right())) {
            return true;
        }
        if (IsSameTree(tree1->Left(), tree2->Right()) && IsSameTree(tree1->Right(), tree2->Left())) {
            return true;
        }
        return false;
    }
    // This operation is NOT commutative.
    if (tree1->Data() == "-" && tree2->Data() == "-") {
        if (IsSameTree(tree1->Left(), tree2->Left()) && IsSameTree(tree1->Right(), tree2->Right())) {
            return true;
        }
        return false;
    }
    if (tree1->Data() == tree2->Data()) { // This should only run on leaf nodes or maybe at most leaf nodes pointers (which SHOULD be nullptrs).
        return true;
    }
    return false;
}

/**
 * Produce an infix representation of the tree structure
 * @param tree
 * @param fNeedOuterParen - caller will generatlly pass false to eliminate outer set of paraentheses, recursive calls pass true
 * @return string representation
 */
string ExpressionTree::ToString(TreeNode* tree, bool fNeedOuterParen) const {
    string s;

    if (Operator == tree->Type()) {
        if (fNeedOuterParen) {
            s += "(";
        }
        s += ToString(tree->Left(), true);
        s += tree->Data();
        s += ToString(tree->Right(), true);
        if (fNeedOuterParen) {
            s += ")";
        }
    } else {
        s += tree->Data();
    }
    return s;
}

/**
 * Basically just used so I could test IsSameTree(). Does what you think it does.
 * @return returns _root
 */
TreeNode* ExpressionTree::GetRoot() const {
    return _root;
}

/**
 * Scans token to see if all characters are digits
 * @param token a string
 * @return true if nonempty and contains all digits, otherwise false
 */
bool IsNumber(string token) {
    if (token.length() == 0) {
        return false;
    }
    for (size_t i = 0; i < token.length(); i ++) {
        if (!isdigit(token[i]) ) {
            return false;
        }
    }
    return true;
}

/**
 * Scans token to see if it starts with letter and rest are letter or digits
 * @param token a string
 * @return true if nonempty,starts with letter, and rest are letters or digits
 */
bool IsVariable(string token)  {
    if (token.length() == 0 || !isalpha(token[0])) {
        return false;
    }
    for (size_t i = 0; i < token.length(); i ++) {
        if (!isalnum(token[i]) ) {
            return false;
        }
    }
    return true;
}

/**
 * Checks if the stirng is a +, -, or *
 * @param token a string
 * @return true if its an operator
 */
bool IsOperator(string token)  {
    return (token.length() == 1 && (token[0] == '+' || token[0] == '-' || token[0] == '*'));
}

