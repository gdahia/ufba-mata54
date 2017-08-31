#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "trie.hpp"

class Dictionary {
 public:
  Dictionary();
  ~Dictionary() = default;

  void insert(const std::string&);
  void print_frequencies(std::ostream&);
  int query_correctness(const std::string&) const;
  std::vector<std::string> get_most_frequent_following_words(const int) const;
  void update_word_sequencing(const int);
  std::vector<std::string> get_most_plausible_corrections(
      const std::string&) const;

 private:
  Trie whole_words, partial_words;
  std::vector<std::string> words;
  std::vector<int> abs_frequencies;
  std::vector<std::map<int, int>> relative_frequencies;
  bool first_typed_word;
  int last_typed_word_index;
};

#endif
