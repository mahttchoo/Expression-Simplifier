#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::getline;

#include "ExpressionTree.h"

int main() {
    string postfix;

    // Professor Benson you can mess around with my IsSameTree() method if you want, so I left this code here. Just mess with these two strings.
    string tree1string = "3 y x * 6 + -";
    string tree2string = "3 x y * 6 + -";

    ExpressionTree tree1;
    ExpressionTree tree2;
    tree1.BuildExpressionTree(tree1string);
    tree2.BuildExpressionTree(tree2string);

    cout << "Are tree1 and tree2 the same: " << tree1.IsSameTree(tree1.GetRoot(), tree2.GetRoot()) << endl;

    cout << "> ";
    while ( getline(cin, postfix) ) {
        if (postfix.length() == 0 || postfix[0] == '#') {
            cout << postfix << endl;
        }
        else {
            ExpressionTree expTree;

            cout << "Postfix: " << postfix << endl;
            if (expTree.BuildExpressionTree(postfix)) {
                cout << "Infix:  " << expTree << endl;
                expTree.Simplify();
                cout << "Simplified: " << expTree << endl;
            }
            cout << "> ";
        }
    }
    return 0;
}

