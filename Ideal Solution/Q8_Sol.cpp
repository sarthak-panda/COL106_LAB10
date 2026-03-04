// Constraints:
//   - Number of nodes: [1, 10^4]
//   - Node values: distinct integers in [-10^9, 10^9]

#include <bits/stdc++.h>
using namespace std;

// Color constants
const int BLACK = 0;
const int RED   = 1;

struct Node {
    int   val, color;
    Node *left, *right, *parent;
    Node(int v, int c)
        : val(v), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
};

// Global NIL sentinel (black, used as all leaf pointers)
Node* NIL = new Node(0, BLACK);

// Returns black-height of subtree rooted at v, or -1 if any property violated.
int checkRBT(Node* v) {
    if (v == NIL) return 0;                           // NIL counts as black; bh = 0

    // Property 4: a red node must have two black children
    if (v->color == RED) {
        if (v->left->color  == RED) return -1;
        if (v->right->color == RED) return -1;
    }

    int leftBH  = checkRBT(v->left);
    int rightBH = checkRBT(v->right);

    if (leftBH == -1 || rightBH == -1) return -1;    // propagate failure

    // Property 5: black-heights of left and right subtrees must be equal
    if (leftBH != rightBH) return -1;

    // Current node contributes +1 to black-height if it is black
    return leftBH + (v->color == BLACK ? 1 : 0);
}

bool isValidRBT(Node* root) {
    if (root == NIL) return true;
    if (root->color != BLACK) return false;           // Property 2: root is black
    return checkRBT(root) != -1;
}

int blackHeight(Node* root) {
    if (root == NIL) return 0;
    if (root->color != BLACK) return -1;
    return checkRBT(root);
}

// ----------------------------------------------------------------
// Helper: link parent -> child (sets both directions)
// ----------------------------------------------------------------
void link(Node* par, Node* child, bool goLeft) {
    if (goLeft) par->left  = child;
    else        par->right = child;
    if (child != NIL) child->parent = par;
}

int main() {
    // --------------------------------------------------
    // Test 1: Valid RBT
    //         7(B)
    //        /    \
    //     3(R)    18(R)
    //    /  \     /  \
    //  2(B) 4(B) 12(B) 19(B)
    // --------------------------------------------------
    Node* n7  = new Node(7,  BLACK);
    Node* n3  = new Node(3,  RED);
    Node* n18 = new Node(18, RED);
    Node* n2  = new Node(2,  BLACK);
    Node* n4  = new Node(4,  BLACK);
    Node* n12 = new Node(12, BLACK);
    Node* n19 = new Node(19, BLACK);
    n7->parent = NIL;
    link(n7, n3,  true);  link(n7, n18, false);
    link(n3, n2,  true);  link(n3, n4,  false);
    link(n18,n12, true);  link(n18,n19, false);
    n2->left  = n2->right  = NIL;
    n4->left  = n4->right  = NIL;
    n12->left = n12->right = NIL;
    n19->left = n19->right = NIL;

    bool v1 = isValidRBT(n7);
    int  b1 = blackHeight(n7);
    cout << "Test 1 (valid RBT):\n";
    cout << "  isValidRBT  = " << (v1 ? "true" : "false")
         << "  (expected: true)\n";
    cout << "  blackHeight = " << b1
         << "  (expected: 2)\n";
    cout << "  " << ((v1 && b1 == 2) ? "PASS" : "FAIL") << "\n\n";

    // --------------------------------------------------
    // Test 2: Invalid — root is RED
    // --------------------------------------------------
    Node* r2 = new Node(5, RED);
    r2->left = r2->right = NIL; r2->parent = NIL;

    bool v2 = isValidRBT(r2);
    cout << "Test 2 (red root):\n";
    cout << "  isValidRBT = " << (v2 ? "true" : "false")
         << "  (expected: false)\n";
    cout << "  " << (!v2 ? "PASS" : "FAIL") << "\n\n";

    // --------------------------------------------------
    // Test 3: Invalid — two consecutive red nodes (property 4)
    //   10(B) -> 5(R) -> 3(R)
    // --------------------------------------------------
    Node* a3 = new Node(10, BLACK);
    Node* b3 = new Node(5,  RED);
    Node* c3 = new Node(3,  RED);
    a3->parent = NIL;
    link(a3, b3, true);  a3->right = NIL;
    link(b3, c3, true);  b3->right = NIL;
    c3->left = c3->right = NIL;

    bool v3 = isValidRBT(a3);
    cout << "Test 3 (double red):\n";
    cout << "  isValidRBT = " << (v3 ? "true" : "false")
         << "  (expected: false)\n";
    cout << "  " << (!v3 ? "PASS" : "FAIL") << "\n\n";

    // --------------------------------------------------
    // Test 4: Invalid — unequal black heights (property 5)
    //   10(B): left=NIL (bh=0), right=5(B)->NIL (bh=1)
    // --------------------------------------------------
    Node* a4 = new Node(10, BLACK);
    Node* b4 = new Node(5,  BLACK);
    a4->parent = NIL;
    a4->left   = NIL;
    link(a4, b4, false);
    b4->left = b4->right = NIL;

    bool v4 = isValidRBT(a4);
    cout << "Test 4 (unequal black heights):\n";
    cout << "  isValidRBT = " << (v4 ? "true" : "false")
         << "  (expected: false)\n";
    cout << "  " << (!v4 ? "PASS" : "FAIL") << "\n";

    return 0;
}
