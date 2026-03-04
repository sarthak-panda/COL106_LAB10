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
    // Idea (findNode): standard multiway search.
    // At each node, if a key matches return the node; if key < keys[i]
    // go to children[i]; otherwise continue or go to the last child.
    // ----------------------------------------------------------------
    Node* findNode(Node* v, int key) {
        if (!v) return nullptr;
        for (int i = 0; i < v->size(); i++) {
            if (v->keys[i] == key) return v;
            if (key < v->keys[i])
                return findNode(v->children[i], key);
        }
        return findNode(v->children[v->size()], key);
    }

    // Idea (inorderSucc): go to child at index keyIdx+1, then follow
    // the leftmost path down to a leaf.
    pair<Node*, int> inorderSucc(Node* v, int keyIdx) {
        Node* cur = v->children[keyIdx + 1];
        while (!cur->isLeaf) cur = cur->children[0];
        return {cur, 0};
    }

    // Idea (fixUnderflow):
    //   Find v's index among parent's children.
    //   Try right sibling transfer, then left sibling transfer.
    //   If neither works, fuse with a sibling (right preferred) by pulling
    //   the separating key down from the parent. Recurse if parent underflows.
    //   Special case: an underflowing root with one child — promote the child.
    //
    // Time:  O(log n) — propagates at most O(h) levels, O(1) per level.
    void fixUnderflow(Node* v) {
        // Special case: underflowing root
        if (v == root) {
            if (v->keys.empty() && !v->isLeaf) {
                root = v->children[0];
                root->parent = nullptr;
                delete v;
            }
            return;
        }

        Node* par = v->parent;
        // Find v's index among parent's children
        int idx = 0;
        while (par->children[idx] != v) idx++;

        // 1. Transfer from right sibling
        if (idx < par->size() && par->children[idx + 1]->size() >= 2) {
            Node* rs = par->children[idx + 1];
            // Pull separator from parent down to v
            v->keys.push_back(par->keys[idx]);
            // Push rs's leftmost key up to parent
            par->keys[idx] = rs->keys[0];
            rs->keys.erase(rs->keys.begin());
            // Adopt rs's leftmost child if internal
            if (!rs->isLeaf) {
                rs->children[0]->parent = v;
                v->children.push_back(rs->children[0]);
                rs->children.erase(rs->children.begin());
            }
            return;
        }

        // 2. Transfer from left sibling
        if (idx > 0 && par->children[idx - 1]->size() >= 2) {
            Node* ls = par->children[idx - 1];
            // Pull separator from parent down to v (prepend)
            v->keys.insert(v->keys.begin(), par->keys[idx - 1]);
            // Push ls's rightmost key up to parent
            par->keys[idx - 1] = ls->keys.back();
            ls->keys.pop_back();
            // Adopt ls's rightmost child if internal
            if (!ls->isLeaf) {
                ls->children.back()->parent = v;
                v->children.insert(v->children.begin(), ls->children.back());
                ls->children.pop_back();
            }
            return;
        }

        // 3. Fusion — fuse v with a sibling (right preferred)
        if (idx < par->size()) {
            // Fuse v (left) with right sibling rs
            Node* rs = par->children[idx + 1];
            // rs absorbs: v's keys, separator, then rs's own keys
            rs->keys.insert(rs->keys.begin(), par->keys[idx]);
            rs->keys.insert(rs->keys.begin(), v->keys.begin(), v->keys.end());
            if (!v->isLeaf) {
                rs->children.insert(rs->children.begin(),
                                    v->children.begin(), v->children.end());
                for (auto c : v->children) c->parent = rs;
            }
            par->keys.erase(par->keys.begin() + idx);
            par->children.erase(par->children.begin() + idx);  // remove v
            delete v;
        } else {
            // Fuse left sibling ls with v (v is rightmost child)
            Node* ls = par->children[idx - 1];
            // ls absorbs: its own keys, separator, then v's keys
            ls->keys.push_back(par->keys[idx - 1]);
            ls->keys.insert(ls->keys.end(), v->keys.begin(), v->keys.end());
            if (!v->isLeaf) {
                ls->children.insert(ls->children.end(),
                                    v->children.begin(), v->children.end());
                for (auto c : v->children) c->parent = ls;
            }
            par->keys.erase(par->keys.begin() + idx - 1);
            par->children.erase(par->children.begin() + idx);   // remove v
            delete v;
        }

        // Parent may now underflow; recurse upward
        if (par->keys.empty()) fixUnderflow(par);
    }

    // Idea (remove):
    //   1. Locate the node. If not found, return.
    //   2. If internal, swap key with its in-order successor (a leaf key).
    //   3. Erase from the leaf.
    //   4. Fix underflow bottom-up.
    //
    // Time:  O(log n)
    // Space: O(log n) (fixUnderflow call stack)
    void remove(int key) {
        Node* v = findNode(root, key);
        if (!v) return;                 // key not present

        // Find position of key within v
        int ki = 0;
        while (v->keys[ki] != key) ki++;

        // If v is internal, replace with in-order successor then delete from leaf
        if (!v->isLeaf) {
            auto [leaf, si] = inorderSucc(v, ki);
            v->keys[ki] = leaf->keys[si];
            v  = leaf;
            ki = si;
        }

        // Remove key from leaf
        v->keys.erase(v->keys.begin() + ki);

        // Fix underflow if needed
        if (v->keys.empty()) fixUnderflow(v);
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

