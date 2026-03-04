// Constraints:
//   - Up to 10^5 insertions, all keys distinct integers.
//   - insert() must run in O(log n) worst-case.

#include <bits/stdc++.h>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    int   key;
    Color color;
    Node *left, *right, *parent;
    explicit Node(int k)
        : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RedBlackTree {
public:
    Node* root;
    Node* NIL;   // sentinel: black, represents all null leaves

    RedBlackTree() {
        NIL        = new Node(0);
        NIL->color = BLACK;
        NIL->left  = NIL->right = NIL;
        root       = NIL;
    }

    // ----------------------------------------------------------------
    // PROVIDED: rotations
    // ----------------------------------------------------------------
    void leftRotate(Node* x) {
        Node* y    = x->right;
        x->right   = y->left;
        if (y->left != NIL) y->left->parent = x;
        y->parent  = x->parent;
        if (x->parent == NIL)          root          = y;
        else if (x == x->parent->left) x->parent->left  = y;
        else                           x->parent->right = y;
        y->left    = x;
        x->parent  = y;
    }

    void rightRotate(Node* y) {
        Node* x    = y->left;
        y->left    = x->right;
        if (x->right != NIL) x->right->parent = y;
        x->parent  = y->parent;
        if (y->parent == NIL)          root          = x;
        else if (y == y->parent->left) y->parent->left  = x;
        else                           y->parent->right = x;
        x->right   = y;
        y->parent  = x;
    }

    // Idea: loop while z's parent is red (property 4 violated).
    //   Determine which side the parent is on relative to the grandparent,
    //   then handle three cases:
    //
    //   Case 1 — uncle is RED:
    //     Recolor parent and uncle BLACK, grandparent RED.
    //     Move z up to grandparent and continue.
    //     (Corresponds to a (2,4)-tree split of a 4-node.)
    //
    //   Case 2 — uncle is BLACK, z is a "triangle" (zig-zag):
    //     Rotate at parent to make it a straight line.
    //     Fall through to Case 3.
    //
    //   Case 3 — uncle is BLACK, z is a "line":
    //     Rotate at grandparent and swap parent/grandparent colors.
    //     Violation resolved; loop terminates.
    //
    // At most O(log n) recolorings (Case 1) and at most 2 rotations total.
    void insertFixup(Node* z) {
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                // Parent is left child of grandparent
                Node* y = z->parent->parent->right;    // uncle

                if (y->color == RED) {                 // Case 1
                    z->parent->color         = BLACK;
                    y->color                 = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;             // move z up
                } else {
                    if (z == z->parent->right) {       // Case 2: triangle
                        z = z->parent;
                        leftRotate(z);                 // converts to Case 3
                    }
                    // Case 3: line
                    z->parent->color         = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                // Mirror: parent is right child of grandparent
                Node* y = z->parent->parent->left;     // uncle

                if (y->color == RED) {                 // Case 1 (mirror)
                    z->parent->color         = BLACK;
                    y->color                 = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {        // Case 2 (mirror)
                        z = z->parent;
                        rightRotate(z);
                    }
                    // Case 3 (mirror)
                    z->parent->color         = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;                           // enforce property 2
    }

    // Standard BST insert, then fix via insertFixup.
    //
    // Time:  O(log n) — BST descent O(h) + fixup O(h) with h = O(log n).
    // Space: O(log n) — iterative; only stack usage is the while loop.
    void insert(int key) {
        Node* z    = new Node(key);
        z->left    = z->right = NIL;
        z->color   = RED;

        Node* y = NIL;
        Node* x = root;
        while (x != NIL) {
            y = x;
            x = (z->key < x->key) ? x->left : x->right;
        }
        z->parent = y;
        if (y == NIL)               root        = z;
        else if (z->key < y->key)   y->left     = z;
        else                        y->right    = z;

        insertFixup(z);
    }

    // ----------------------------------------------------------------
    // PROVIDED: search, inorder, isValid
    // ----------------------------------------------------------------
    bool search(int key) {
        Node* cur = root;
        while (cur != NIL) {
            if (key == cur->key) return true;
            cur = (key < cur->key) ? cur->left : cur->right;
        }
        return false;
    }

    vector<int> inorder() {
        vector<int> res;
        inorderHelper(root, res);
        return res;
    }
    void inorderHelper(Node* v, vector<int>& res) {
        if (v == NIL) return;
        inorderHelper(v->left, res);
        res.push_back(v->key);
        inorderHelper(v->right, res);
    }

    // Full RBT property checker (for testing only)
    bool isValid() {
        if (root == NIL) return true;
        if (root->color != BLACK) return false;
        int bh = -1;
        return checkRBT(root, 0, bh);
    }
    bool checkRBT(Node* v, int curBlack, int& bh) {
        if (v == NIL) {
            if (bh == -1) bh = curBlack;
            return curBlack == bh;
        }
        if (v->color == RED &&
            (v->left->color == RED || v->right->color == RED)) return false;
        int add = (v->color == BLACK) ? 1 : 0;
        return checkRBT(v->left,  curBlack + add, bh) &&
               checkRBT(v->right, curBlack + add, bh);
    }
};

int main() {
    // Test 1: sorted insert; maximally stresses rotations
    RedBlackTree rbt1;
    vector<int> keys1 = {1,2,3,4,5,6,7,8,9,10};
    for (int k : keys1) rbt1.insert(k);

    bool io1   = (rbt1.inorder() == keys1);
    bool val1  = rbt1.isValid();
    cout << "Test 1 (sorted insert 1..10):\n";
    cout << "  Inorder correct : " << (io1  ? "PASS" : "FAIL") << "\n";
    cout << "  RBT valid       : " << (val1 ? "PASS" : "FAIL") << "\n\n";

    // Test 2: random order
    RedBlackTree rbt2;
    vector<int> keys2 = {15,6,3,17,22,1,9,4,13,20};
    for (int k : keys2) rbt2.insert(k);

    vector<int> sorted2 = keys2;
    sort(sorted2.begin(), sorted2.end());

    bool io2  = (rbt2.inorder() == sorted2);
    bool val2 = rbt2.isValid();
    cout << "Test 2 (random order):\n";
    cout << "  Inorder correct : " << (io2  ? "PASS" : "FAIL") << "\n";
    cout << "  RBT valid       : " << (val2 ? "PASS" : "FAIL") << "\n\n";

    // Test 3: search
    bool s1 = rbt2.search(15);
    bool s2 = rbt2.search(100);
    cout << "Test 3 (search):\n";
    cout << "  search(15)  = true  : " << (s1  ? "PASS" : "FAIL") << "\n";
    cout << "  search(100) = false : " << (!s2 ? "PASS" : "FAIL") << "\n\n";

    // Test 4: large sorted insert
    RedBlackTree rbt4;
    vector<int> keys4(200);
    iota(keys4.begin(), keys4.end(), 1);
    for (int k : keys4) rbt4.insert(k);

    bool io4  = (rbt4.inorder() == keys4);
    bool val4 = rbt4.isValid();
    cout << "Test 4 (sorted insert 1..200):\n";
    cout << "  Inorder correct : " << (io4  ? "PASS" : "FAIL") << "\n";
    cout << "  RBT valid       : " << (val4 ? "PASS" : "FAIL") << "\n";

    return 0;
}
