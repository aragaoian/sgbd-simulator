#include <iostream>
using namespace std;

class BST {

    struct Node {
        int data;
        Node *left;
        Node *right;
    };

    Node *root;

    Node *makeEmpty(Node *t) {
        if (t == NULL) return NULL;
        {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
        }
        return NULL;
    }

    pair<Node *, bool> insert(int x, Node *t) {
        if (t == NULL) {
            t = new Node;
            t->data = x;
            t->left = t->right = NULL;
            return {t, true}; // Inserted new node
        } else if (x < t->data) {
            auto res = insert(x, t->left);
            t->left = res.first;
            return {t, res.second};
        } else if (x > t->data) {
            auto res = insert(x, t->right);
            t->right = res.first;
            return {t, res.second};
        } else {
            return {t, false}; // Node already exists
        }
    }

    Node *findMin(Node *t) {
        if (t == NULL)
            return NULL;
        else if (t->left == NULL)
            return t;
        else
            return findMin(t->left);
    }

    Node *remove(int x, Node *t) {
        Node *temp;
        if (t == NULL)
            return NULL;
        else if (x < t->data)
            t->left = remove(x, t->left);
        else if (x > t->data)
            t->right = remove(x, t->right);
        else if (t->left && t->right) {
            temp = findMin(t->right);
            t->data = temp->data;
            t->right = remove(t->data, t->right);
        } else {
            temp = t;
            if (t->left == NULL)
                t = t->right;
            else if (t->right == NULL)
                t = t->left;
            delete temp;
        }

        return t;
    }

    Node *find(Node *t, int x) {
        if (t == NULL)
            return NULL;
        else if (x < t->data)
            return find(t->left, x);
        else if (x > t->data)
            return find(t->right, x);
        else
            return t;
    }

  public:
    BST() { root = NULL; }

    ~BST() { root = makeEmpty(root); }

    bool insert(int x) {
        auto res = insert(x, root);
        root = res.first;
        return res.second;
    }

    void remove(int x) { root = remove(x, root); }

    void search(int x) { root = find(root, x); }

    void clear() { root = makeEmpty(root); }
};