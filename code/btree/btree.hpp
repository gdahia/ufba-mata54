#ifndef BTREE_HPP
#define BTREE_HPP

#include <algorithm>
#include <fstream>
#include <vector>

namespace b {

template <typename T>
struct node {
  node(const int t) : t(t) {
    keys.reserve(t);
    children.reserve(t);
  }

  ~node() {
    const int len = children.size();
    for (int i = 0; i < len; i++) delete children[i];
  }

  void split_child(const int i) {
    // create right child
    node<T>* right = new node<T>(t);
    children.insert(children.begin() + i + 1, right);

    // copy keys and children to right child
    std::copy(children[i]->keys.begin() + t, children[i]->keys.end(),
              std::back_inserter(right->keys));
    std::copy(children[i]->children.begin() + t, children[i]->children.end(),
              std::back_inserter(right->children));

    // erase right child's keys and children from left child
    children[i]->keys.erase(children[i]->keys.begin() + t,
                            children[i]->keys.end());
    if (!children[i]->children.empty())
      children[i]->children.erase(children[i]->children.begin() + t,
                                  children[i]->children.end());

    // move median up
    keys.insert(keys.begin() + i, children[i]->keys[t - 1]);
    children[i]->keys.pop_back();
  }

  void print(std::ostream& stream, const int level) const {
    for (int i = 0; i < level; i++) stream << " ";
    stream << "[";
    int n = keys.size();
    for (int i = 0; i < n; i++) {
      stream << keys[i];
      if (i < n - 1) stream << " ";
    }
    n = children.size();
    stream << "]" << std::endl;
    for (int i = 0; i < n; i++) children[i]->print(stream, level + 1);
  }

  const int t;
  std::vector<T> keys;
  std::vector<node<T>*> children;
};

template <typename T>
class tree {
 public:
  tree(const int t) : t(t) { root = new node<T>(t); }

  ~tree() { delete root; }

  bool search(const T& key) const {
    typename std::vector<T>::iterator it =
        std::lower_bound(root->keys.begin(), root->keys.end(), key);

    if (it != root->keys.end() && *it == key)
      return true;
    else if (root->children.empty())
      return false;
    else {
      const int i = it - root->keys.begin();
      return search(root->children[i], key);
    }
  }

  void insert(const T& key) {
    // if root is full, create new root and split old root
    if (root->keys.size() == 2 * t - 1) {
      node<T>* r = root;
      root = new node<T>(t);
      root->children.push_back(r);
      root->split_child(0);
    }

    insert(root, key);
  }

  void print(std::ostream& stream) const { root->print(stream, 0); }

 private:
  void insert(node<T>* x, const T& key) {
    // if x is leaf, insert in it
    // otherwise, continue insertion downwards
    if (x->children.empty()) {
      // find key position in x
      typename std::vector<T>::iterator it =
          std::upper_bound(x->keys.begin(), x->keys.end(), key);

      x->keys.insert(it, key);

    } else {
      // find proper child
      typename std::vector<T>::iterator it =
          std::upper_bound(x->keys.begin(), x->keys.end(), key);
      int i = it - x->keys.begin();
      node<T>* child = x->children[i];

      // if proper child is full, split it
      if (child->keys.size() == 2 * t - 1) {
        x->split_child(i);

        // find out which of the new children is proper
        if (key > x->keys[i]) i++;
      }

      insert(x->children[i], key);
    }
  }

  bool search(node<T>* x, const T& key) const {
    typename std::vector<T>::iterator it =
        std::lower_bound(x->keys.begin(), x->keys.end(), key);
    if (x->children.empty())
      return (it != x->keys.end() && *it == key);
    else {
      if (it == x->keys.end())
        return search(x->children.back(), key);
      else if (*it == key)
        return true;
      else {
        const int i = it - x->keys.begin();
        return search(x->children[i], key);
      }
    }
  }

  const int t;
  node<T>* root;
};
}

#endif
