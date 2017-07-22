#ifndef BTREE_HPP
#define BTREE_HPP

#include <algorithm>
#include <vector>

namespace b {

template <typename T>
struct Node {
  Node(const int t) : t(t) {
    keys.reserve(t);
    children.reserve(t);
  }

  ~Node() {
    const int len = children.size();
    for (int i = 0; i < len; i++) delete children[i];
  }

  void split_child(const int i) {
    // create right child
    Node<T>* right = new Node(t);
    children.insert(children.begin() + i + 1, right);

    // copy keys and children to right child
    std::copy(children[i]->keys.begin() + t + 1, children[i]->keys.end(),
              std::back_inserter(right->keys));
    std::copy(children[i]->children.begin() + t + 1,
              children[i]->children.end(), std::back_inserter(right->children));

    // erase right child's keys and children from left child
    children[i]->keys.erase(children[i]->keys.begin() + t + 1,
                            children[i]->keys.end());
    children[i]->children.erase(children[i]->children.begin() + t + 1,
                                children[i]->children.end());

    // move median up
    keys.insert(keys.begin() + i, children[i]->keys[t - 1]);
    children[i]->keys.pop_back();
  }

  const int t;
  std::vector<T> keys;
  std::vector<Node<T>*> children;
};

template <typename T>
class Tree {
 public:
  Tree(const int t) : t(t) { root = new Node<T>(t); }

  ~Tree() { delete root; }

  void insert(const T& key) {
    // if root is full, create new root and split old root
    if (root->keys.size() == 2 * t - 1) {
      Node<T>* r = root;
      root = new Node<T>(t);
      root->children.push_back(r);
      root->split_child(0);
    }

    insert(root, key);
  }

 private:
  void insert(Node<T>* node, const T& key) {
    // if node is leaf, insert in it
    // otherwise, continue insertion downwards
    if (node->children.empty()) {
      // find key position in node
      typename std::vector<T>::iterator it =
          std::upper_bound(node->keys.begin(), node->keys.end(), key);

      node->keys.insert(it, key);

    } else {
      // find proper child
      typename std::vector<T>::iterator it =
          std::upper_bound(node->keys.begin(), node->keys.end(), key);
      int i = it - node->keys.begin();
      Node<T>* child = node->children[i];

      // if proper child is full, split it
      if (child->keys.size() == 2 * t - 1) {
        node->split_child(i);

        // find out which of the new children is proper
        if (key > node->keys[i]) i++;
      }

      insert(node->children[i], key);
    }
  }

  const int t;
  Node<T>* root;
};
}

#endif
