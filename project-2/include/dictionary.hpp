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

  int insert(const std::string&);
  bool type_word(const std::string&, std::ostream&);
  void print_frequencies(std::ostream&);
  int query_correctness(const std::string&) const;
  void update_word_sequencing(const int);
  void print_followup_frequencies(const std::string&, std::ostream&) const;

 private:
  Trie whole_words, partial_words;
  std::vector<std::string> words;
  std::vector<int> abs_frequencies;
  std::vector<std::map<int, int>> relative_frequencies;
  bool first_typed_word;
  int last_typed_word_index;

  std::vector<int> get_most_frequent_followups(const int) const;
  std::vector<int> get_most_plausible_corrections(const std::string&) const;
};

#endif
