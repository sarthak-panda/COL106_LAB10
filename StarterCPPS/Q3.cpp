// Constraints:
//   - 1 <= d <= 5
//   - Number of keys in the tree: [0, 10^4]
//   - All key values are distinct integers in [-10^9, 10^9]

#include <bits/stdc++.h>
using namespace std;

struct Node {
    vector<int> keys;       // keys stored in this node (sorted ascending)
    vector<Node*> children; // children[i] is the subtree between keys[i-1] and keys[i]
    bool isLeaf;

    Node() : isLeaf(true) {}
};

// TODO: Implement this function.
// Returns true if key x is found in the subtree rooted at v, false otherwise.
// Hint: check v->isLeaf before accessing v->children.
bool search(Node* v, int x) {
    // YOUR CODE HERE
}

// ----------------------------------------------------------------
// Helper: build a small (2,4) tree for testing.
//
//          [10, 20]
//         /    |    \
//      [5]   [15]   [25, 30]
// ----------------------------------------------------------------
Node* buildTestTree() {
    Node* root = new Node();
    root->keys = {10, 20};
    root->isLeaf = false;

    Node* l = new Node(); l->keys = {5};
    Node* m = new Node(); m->keys = {15};
    Node* r = new Node(); r->keys = {25, 30};

    root->children = {l, m, r};
    return root;
}

int main() {
    Node* root = buildTestTree();

    // {key, expected_result}
    vector<pair<int, bool>> tests = {
        {10, true},
        {20, true},
        {5,  true},
        {15, true},
        {25, true},
        {30, true},
        {1,  false},
        {12, false},
        {100,false}
    };

    int passed = 0;
    for (auto& [key, expected] : tests) {
        bool result = search(root, key);
        bool ok = (result == expected);
        cout << "search(" << setw(3) << key << ") = "
             << (result   ? "true " : "false")
             << "  |  expected: "
             << (expected ? "true " : "false")
             << "  |  " << (ok ? "PASS" : "FAIL") << "\n";
        if (ok) passed++;
    }
    cout << "\nPassed " << passed << " / " << tests.size() << " tests.\n";
    return 0;
}
