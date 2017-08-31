#include "trie.hpp"

#include <algorithm>

using namespace trie;

node::~node() {
  for (std::pair<char, node*> x : nodes) delete x.second;
}

Trie::Trie() { root = new node(); }
Trie::~Trie() { delete root; }

void Trie::insert(const std::string& word, const int index) {
  node* x = root;
  for (char c : word) {
    bool found = false;
    for (const std::pair<char, node*>& y : x->nodes)
      if (y.first == c) {
        found = true;
        x = y.second;
        break;
      }

    if (!found) {
      node* y = new node();
      x->nodes.emplace_front(c, y);
      x = y;
    }
  }

  x->indices.push_back(index);
}

std::vector<int> Trie::query(const std::string& word) const {
  node const* x = root;
  for (char c : word) {
    bool found = false;
    for (const std::pair<char, node*>& y : x->nodes)
      if (y.first == c) {
        found = true;
        x = y.second;
        break;
      }

    if (!found) return std::vector<int>();
  }

  return x->indices;
}

std::vector<int> Trie::walk() {
  node* x = root;
  return recursive_walk(x);
}

std::vector<int> Trie::recursive_walk(node* x) {
  // sort nodes by char
  x->nodes.sort(
      [](const std::pair<char, node*>& a, const std::pair<char, node*>& b) {
        return a.first < b.first;
      });

  // recursively obtain indices for child nodes
  std::vector<int> ret = x->indices;
  for (const std::pair<char, node*> y : x->nodes) {
    std::vector<int> sub_ret = recursive_walk(y.second);
    std::copy(sub_ret.begin(), sub_ret.end(), std::back_inserter(ret));
  }

  return ret;
}
