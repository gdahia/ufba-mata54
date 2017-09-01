#ifndef BTREE_HPP
#define BTREE_HPP

#include <algorithm>
#include <fstream>
#include <vector>

namespace b {

template <typename T, class Compare>
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
    node<T, Compare>* right = new node<T, Compare>(t);
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

  T* insert(const T& key) {
    // if leaf, insert in it
    // otherwise, continue insertion downwards
    if (children.empty()) {
      // find key position
      typename std::vector<T>::iterator it =
          std::upper_bound(keys.begin(), keys.end(), key, Compare());

      keys.insert(it, key);
      return &(*(it + 1));

    } else {
      // find proper child
      typename std::vector<T>::iterator it =
          std::upper_bound(keys.begin(), keys.end(), key, Compare());
      int i = it - keys.begin();
      node<T, Compare>* child = children[i];

      // if proper child is full, split it
      if (child->keys.size() == 2 * t - 1) {
        split_child(i);

        // find out which of the new children is proper
        if (key > keys[i]) i++;
      }

      return children[i]->insert(key);
    }
  }

  T* search(const T& key) {
    typename std::vector<T>::iterator it =
        std::lower_bound(keys.begin(), keys.end(), key, Compare());
    if (children.empty())
      if (it != keys.end() && !Compare(key, *it) && !Compare(*it, key))
        return &(*it);
      else
        return NULL;
    else if (it == keys.end())
      return children.back()->search(key);
    else if (!Compare(key, *it) && !Compare(*it, key))
      return &(*it);
    else {
      const int i = it - keys.begin();
      return children[i]->search(key);
    }
  }

  void merge_right_left(const int i) {
    // descend ith-key to be new median
    children[i]->keys.push_back(keys[i]);
    keys.erase(keys.begin() + i);

    // append the keys and children of the right child to left's
    std::copy(children[i + 1]->keys.begin(), children[i + 1]->keys.end(),
              std::back_inserter(children[i]->keys));
    if (!children[i]->children.empty())
      std::copy(children[i + 1]->children.begin(),
                children[i + 1]->children.end(),
                std::back_inserter(children[i]->children));

    // delete pointer to right child
    // prevent recursive deletion of its children
    children[i + 1]->children.clear();
    delete children[i + 1];
    children.erase(children.begin() + i + 1);
  }

  void erase(const T& key) {
    typename std::vector<T>::iterator it =
        std::lower_bound(keys.begin(), keys.end(), key, Compare());
    const int i = it - keys.begin();

    if (it != keys.end() && *it == key) {
      if (children.empty()) {
        // if leaf, just erase it
        keys.erase(it);
      } else if (children[i]->keys.size() >= t) {
        // get key from child left of key
        const T replacement = children[i]->keys.back();
        children[i]->erase(replacement);
        *it = replacement;

      } else if (children[i + 1]->keys.size() >= t) {
        // get key from child right of key
        const T replacement = children[i + 1]->keys[0];
        children[i + 1]->erase(replacement);
        *it = replacement;

      } else {
        merge_right_left(i);

        // recursively delete key from newly merged child
        children[i]->erase(key);
      }
    } else {
      if (!children.empty()) {
        // maintain every node with at lest t keys
        // while recursing down the tree
        if (children[i]->keys.size() >= t)
          children[i]->erase(key);
        else {
          if (it != keys.end() && children[i + 1]->keys.size() >= t) {
            // descend a key from current node to recursion node
            children[i]->keys.push_back(*it);

            // replace descended key with leftmost key of
            // recursion node's right sibiling
            std::swap(children[i + 1]->keys[0], *it);
            children[i + 1]->keys.erase(children[i + 1]->keys.begin());

            if (!children[i]->children.empty()) {
              // move right sibiling's leftmost child to recursion node
              children[i]->children.push_back(children[i + 1]->children[0]);
              children[i + 1]->children.erase(
                  children[i + 1]->children.begin());
            }

            // continue recursive deletion
            children[i]->erase(key);

          } else if (i > 0 && children[i - 1]->keys.size() >= t) {
            // descend a key from current node to recursion node
            children[i]->keys.insert(children[i]->keys.begin(), *(it - 1));

            // replace descended key with rightmost key of
            // recursion node's left sibiling
            std::swap(children[i - 1]->keys.back(), *(it - 1));
            children[i - 1]->keys.pop_back();

            if (!children[i]->children.empty()) {
              // move left sibiling's rightmost child to recursion node
              children[i]->children.insert(children[i]->children.begin(),
                                           children[i - 1]->children.back());
              children[i - 1]->children.pop_back();
            }

            // continue recursive deletion
            children[i]->erase(key);

          } else if (it != keys.end()) {
            // merge recursion node with its right sibiling,
            // descending a key from the current node
            merge_right_left(i);

            // continue recursive deletion
            children[i]->erase(key);

          } else if (i > 0) {
            // merge recursion node with its left sibiling
            // descending a key from the current node
            merge_right_left(i - 1);

            // continue recursive deletion
            children[i - 1]->erase(key);
          }
        }
      }
    }
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
  std::vector<node<T, Compare>*> children;
};

template <typename T, class Compare>
class tree {
 public:
  tree(const int t) : t(t) { root = new node<T, Compare>(t); }

  ~tree() { delete root; }

  T* search(const T& key) const {
    typename std::vector<T>::iterator it =
        std::lower_bound(root->keys.begin(), root->keys.end(), key, Compare());

    if (it != root->keys.end() && !Compare(*it, key) && !Compare(key, *it))
      return &(*it);
    else if (root->children.empty())
      return NULL;
    else {
      const int i = it - root->keys.begin();
      return root->children[i]->search(key);
    }
  }

  T* insert(const T& key) {
    // if root is full, create new root and split old root
    if (root->keys.size() == 2 * t - 1) {
      node<T, Compare>* r = root;
      root = new node<T, Compare>(t);
      root->children.push_back(r);
      root->split_child(0);
    }

    return root->insert(key);
  }

  void erase(const T& key) {
    root->erase(key);
    // if root is emptied by deletion, make left child new root
    if (root->keys.empty() && !root->children.empty()) {
      node<T, Compare>* new_root = root->children[0];
      root->children.clear();
      delete root;
      root = new_root;
    }
  }

  void print(std::ostream& stream) const { root->print(stream, 0); }

 private:
  const int t;
  node<T, Compare>* root;
  node<T, Compare>* rightmost;
};
}

#endif
