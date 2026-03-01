// Constraints:
//   - 1 <= number of insertions <= 10^4
//   - All keys are distinct integers in [-10^6, 10^6]

#include <bits/stdc++.h>
using namespace std;

struct Node {
    vector<int>   keys;
    vector<Node*> children;
    Node*         parent;
    bool          isLeaf;

    Node() : parent(nullptr), isLeaf(true) {}
    int size() const { return (int)keys.size(); }
};

struct TwoFourTree {
    Node* root;

    TwoFourTree() { root = new Node(); }

    // ----------------------------------------------------------------
    // splitAndFix
    // If node v has 4 keys (overflow), split it: promote the middle key
    // to v's parent and recurse upward. Creates a new root if needed.
    // ----------------------------------------------------------------
    void splitAndFix(Node* v) {
        if (v->size() <= 3) return;             // no overflow

        // v has keys: k0 k1 k2 k3  and children: c0 c1 c2 c3 c4
        int promoted = v->keys[2];

        Node* right = new Node();
        right->isLeaf = v->isLeaf;
        right->keys   = {v->keys[3]};
        if (!v->isLeaf) {
            right->children = {v->children[3], v->children[4]};
            for (auto c : right->children) c->parent = right;
            v->children.resize(3);
        }
        v->keys.resize(2);                      // v keeps k0, k1

        if (v == root) {
            // Grow tree height by 1
            Node* newRoot    = new Node();
            newRoot->isLeaf  = false;
            newRoot->keys    = {promoted};
            newRoot->children= {v, right};
            v->parent = right->parent = newRoot;
            root = newRoot;
        } else {
            // Insert promoted key into parent
            Node* par = v->parent;
            int i = 0;
            while (i < par->size() && promoted > par->keys[i]) i++;
            par->keys.insert(par->keys.begin() + i, promoted);
            par->children.insert(par->children.begin() + i + 1, right);
            right->parent = par;
            splitAndFix(par);
        }
    }

    // ----------------------------------------------------------------
    // TODO: Implement insert(key)
    //
    // Steps:
    //   1. Walk down to the correct leaf node (where key belongs).
    //   2. Insert key into that leaf.
    //   3. Call splitAndFix() on the leaf to resolve any overflow.
    // ----------------------------------------------------------------
    void insert(int key) {
        // YOUR CODE HERE
    }

    // ----------------------------------------------------------------
    // Inorder traversal (for correctness verification)
    // ----------------------------------------------------------------
    vector<int> inorder() {
        vector<int> result;
        inorderHelper(root, result);
        return result;
    }
    void inorderHelper(Node* v, vector<int>& res) {
        if (!v) return;
        int m = v->size();
        for (int i = 0; i < m; i++) {
            if (!v->isLeaf) inorderHelper(v->children[i], res);
            res.push_back(v->keys[i]);
        }
        if (!v->isLeaf) inorderHelper(v->children[m], res);
    }

    // Verify all leaves are at the same depth
    bool verifyDepthProperty() {
        int leafDepth = -1;
        return checkDepth(root, 0, leafDepth);
    }
    bool checkDepth(Node* v, int depth, int& leafDepth) {
        if (v->isLeaf) {
            if (leafDepth == -1) leafDepth = depth;
            return depth == leafDepth;
        }
        for (auto c : v->children)
            if (!checkDepth(c, depth + 1, leafDepth)) return false;
        return true;
    }

    // Verify every node has 1–3 keys
    bool verifySizeProperty() { return checkSize(root); }
    bool checkSize(Node* v) {
        if (v->size() < 1 || v->size() > 3) return false;
        if (!v->isLeaf)
            for (auto c : v->children)
                if (!checkSize(c)) return false;
        return true;
    }
};

int main() {
    // Test 1: insert the sequence from Lab Activity Sheet
    TwoFourTree t1;
    vector<int> keys1 = {5, 16, 22, 45, 2, 10, 18, 30, 50, 12};
    for (int k : keys1) t1.insert(k);

    vector<int> sorted1 = keys1;
    sort(sorted1.begin(), sorted1.end());

    bool inorderOK1 = (t1.inorder() == sorted1);
    bool depthOK1   = t1.verifyDepthProperty();
    bool sizeOK1    = t1.verifySizeProperty();
    cout << "Test 1 (lab sequence):\n";
    cout << "  Inorder correct : " << (inorderOK1 ? "PASS" : "FAIL") << "\n";
    cout << "  Depth property  : " << (depthOK1   ? "PASS" : "FAIL") << "\n";
    cout << "  Size property   : " << (sizeOK1    ? "PASS" : "FAIL") << "\n\n";

    // Test 2: sorted insert (triggers many splits)
    TwoFourTree t2;
    vector<int> keys2 = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    for (int k : keys2) t2.insert(k);

    bool inorderOK2 = (t2.inorder() == keys2);
    bool depthOK2   = t2.verifyDepthProperty();
    bool sizeOK2    = t2.verifySizeProperty();
    cout << "Test 2 (sorted insert, many splits):\n";
    cout << "  Inorder correct : " << (inorderOK2 ? "PASS" : "FAIL") << "\n";
    cout << "  Depth property  : " << (depthOK2   ? "PASS" : "FAIL") << "\n";
    cout << "  Size property   : " << (sizeOK2    ? "PASS" : "FAIL") << "\n\n";

    // Test 3: single element
    TwoFourTree t3;
    t3.insert(42);
    bool single = (t3.inorder() == vector<int>{42});
    cout << "Test 3 (single element): " << (single ? "PASS" : "FAIL") << "\n";

    return 0;
}
