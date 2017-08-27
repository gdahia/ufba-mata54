#ifndef TRIE_HPP
#define TRIE_HPP

#include <list>
#include <string>
#include <vector>

namespace trie {
struct node {
  ~node();
  std::list<std::pair<char, node*>> nodes;
  std::vector<int> indices;
};
}

class Trie {
 public:
  Trie();
  ~Trie();

  void insert(const std::string&, const int);
  std::vector<int> query(const std::string&);

 private:
  trie::node* root;
};

#endif
