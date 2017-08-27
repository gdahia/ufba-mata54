#include "trie.hpp"

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

std::vector<int> Trie::query(const std::string& word) {
  node* x = root;
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
