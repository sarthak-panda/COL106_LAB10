// Constraints:
//   - Up to 10^4 insertions followed by up to 10^4 deletions.
//   - Deleting a key not in the tree: no-op.
//   - After every delete the tree must satisfy size and depth properties.

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
    // PROVIDED: insert (same as Problem 7 solution)
    // ----------------------------------------------------------------
    void splitAndFix(Node* v) {
        if (v->size() <= 3) return;
        int promoted  = v->keys[2];
        Node* right   = new Node();
        right->isLeaf = v->isLeaf;
        right->keys   = {v->keys[3]};
        if (!v->isLeaf) {
            right->children = {v->children[3], v->children[4]};
            for (auto c : right->children) c->parent = right;
            v->children.resize(3);
        }
        v->keys.resize(2);
        if (v == root) {
            Node* nr     = new Node();
            nr->isLeaf   = false;
            nr->keys     = {promoted};
            nr->children = {v, right};
            v->parent = right->parent = nr;
            root = nr;
        } else {
            Node* par = v->parent;
            int i = 0;
            while (i < par->size() && promoted > par->keys[i]) i++;
            par->keys.insert(par->keys.begin() + i, promoted);
            par->children.insert(par->children.begin() + i + 1, right);
            right->parent = par;
            splitAndFix(par);
        }
    }

    void insert(int key) {
        Node* v = root;
        while (!v->isLeaf) {
            int m = v->size(), i = 0;
            while (i < m && key > v->keys[i]) i++;
            v = v->children[i];
        }
        int i = 0;
        while (i < v->size() && key > v->keys[i]) i++;
        v->keys.insert(v->keys.begin() + i, key);
        splitAndFix(v);
    }

    // ----------------------------------------------------------------
    // TODO: Implement findNode(v, key)
    // Returns the node containing key in the subtree rooted at v,
    // or nullptr if key is not present.
    // ----------------------------------------------------------------
    Node* findNode(Node* v, int key) {
        // YOUR CODE HERE
    }

    // ----------------------------------------------------------------
    // TODO: Implement inorderSucc(v, keyIdx)
    // Returns {leaf, indexInLeaf} for the in-order successor of
    // v->keys[keyIdx]. The successor is the leftmost key of the
    // subtree rooted at v->children[keyIdx + 1].
    // ----------------------------------------------------------------
    pair<Node*, int> inorderSucc(Node* v, int keyIdx) {
        // YOUR CODE HERE
    }

    // ----------------------------------------------------------------
    // TODO: Implement fixUnderflow(v)
    // v currently has 0 keys. Restore the size property via:
    //   1. Transfer from right sibling (if it has >= 2 keys).
    //   2. Transfer from left sibling  (if it has >= 2 keys).
    //   3. Fusion with right sibling   (pull separator from parent).
    //   4. Fusion with left sibling    (pull separator from parent).
    // After a fusion, if the parent has 0 keys recurse upward.
    // Special case: if v is root with 0 keys and one child, promote child.
    // ----------------------------------------------------------------
    void fixUnderflow(Node* v) {
        // YOUR CODE HERE
    }

    // ----------------------------------------------------------------
    // TODO: Implement remove(key)
    // ----------------------------------------------------------------
    void remove(int key) {
        // YOUR CODE HERE
    }

    // ----------------------------------------------------------------
    // PROVIDED: verification helpers and inorder
    // ----------------------------------------------------------------
    vector<int> inorder() {
        vector<int> res;
        inHelper(root, res);
        return res;
    }
    void inHelper(Node* v, vector<int>& res) {
        if (!v) return;
        for (int i = 0; i < v->size(); i++) {
            if (!v->isLeaf) inHelper(v->children[i], res);
            res.push_back(v->keys[i]);
        }
        if (!v->isLeaf) inHelper(v->children[v->size()], res);
    }

    bool verifyDepth() { int d = -1; return chkD(root, 0, d); }
    bool chkD(Node* v, int dep, int& d) {
        if (v->isLeaf) { if (d == -1) d = dep; return dep == d; }
        for (auto c : v->children) if (!chkD(c, dep + 1, d)) return false;
        return true;
    }

    bool verifySize() { return chkSz(root); }
    bool chkSz(Node* v) {
        if (v->size() < 1 || v->size() > 3) return false;
        if (!v->isLeaf)
            for (auto c : v->children) if (!chkSz(c)) return false;
        return true;
    }
};

int main() {
    TwoFourTree t;
    vector<int> keys = {10,20,5,15,25,3,8,12,18,22,30,1,4,6,9};
    for (int k : keys) t.insert(k);

    vector<int> toDelete = {5,15,1,20,9,12,25,6};
    vector<int> remaining = keys;

    cout << "Deletion trace:\n";
    for (int k : toDelete) {
        t.remove(k);
        remaining.erase(find(remaining.begin(), remaining.end(), k));

        vector<int> exp = remaining;
        sort(exp.begin(), exp.end());

        bool depOK = t.verifyDepth();
        bool szOK  = t.verifySize();
        bool ioOK  = (t.inorder() == exp);

        cout << "  remove(" << setw(2) << k << "): "
             << "depth=" << (depOK ? "OK" : "FAIL")
             << "  size=" << (szOK  ? "OK" : "FAIL")
             << "  inorder=" << (ioOK ? "OK" : "FAIL") << "\n";
    }

    // Final check
    vector<int> finalExp = remaining;
    sort(finalExp.begin(), finalExp.end());
    bool finalOK = (t.inorder() == finalExp);
    cout << "\nFinal inorder: ";
    for (int k : t.inorder()) cout << k << " ";
    cout << "\nExpected:      ";
    for (int k : finalExp)    cout << k << " ";
    cout << "\n" << (finalOK ? "PASS" : "FAIL") << "\n";

    return 0;
}
