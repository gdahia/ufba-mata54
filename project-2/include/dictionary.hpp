#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <ostream>
#include <string>
#include <vector>

#include "trie.hpp"

class Dictionary {
 public:
  Dictionary() = default;
  ~Dictionary() = default;

  void insert(const std::string&);
  void print_frequencies(std::ostream&);

 private:
  Trie whole_words, partial_words;

  std::vector<std::string> words;
  std::vector<int> frequencies;

  int retrieve_index(const std::string&);
};

#endif
