#include "dictionary.hpp"

void Dictionary::insert(const std::string& word) {
  const int index = words.size();
  words.push_back(word);
  frequencies.push_back(0);

  whole_words.insert(word, index);

  std::string partial_word;
  const int len = word.size();
  if (len > 1)
    for (int i = 0; i < len; i++) {
      partial_words.insert(partial_word + word.substr(i + 1), index);
      partial_word += word[i];
    }
}

void Dictionary::print_frequencies(std::ostream& os) {
  std::vector<int> alpha_sorted = whole_words.walk();
  for (int i : alpha_sorted)
    os << words[i] << " " << frequencies[i] << std::endl;
}

int Dictionary::retrieve_index(const std::string& word) {
  std::vector<int> indices = whole_words.query(word);
  if (indices.empty())
    return -1;
  else
    return indices[0];
}
