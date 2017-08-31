#include "dictionary.hpp"

#include <algorithm>
#include <queue>

Dictionary::Dictionary() { first_typed_word = true; }

void Dictionary::insert(const std::string& word) {
  const int index = words.size();
  words.push_back(word);
  abs_frequencies.push_back(0);
  relative_frequencies.push_back(std::map<int, int>());

  whole_words.insert(word, index);

  std::string partial_word;
  const int len = word.size();
  for (int i = 0; i < len; i++) {
    partial_words.insert(partial_word + word.substr(i + 1), index);
    partial_word += word[i];
  }
}

void Dictionary::print_frequencies(std::ostream& os) {
  std::vector<int> alpha_sorted = whole_words.walk();
  for (int i : alpha_sorted)
    os << words[i] << " " << abs_frequencies[i] << std::endl;
}

int Dictionary::query_correctness(const std::string& word) const {
  std::vector<int> whole_query = whole_words.query(word);

  if (whole_query.empty())
    return -1;
  else
    return whole_query[0];
}

std::vector<std::string> Dictionary::get_most_frequent_following_words(
    const int index) const {
  // find said words
  std::vector<int> most_frequent_words_indices;
  for (auto it = relative_frequencies[index].crbegin();
       it != relative_frequencies[index].crend(); it++) {
    most_frequent_words_indices.push_back(it->first);
    if (most_frequent_words_indices.size() == 3) break;
  }

  // complete 3 suggestions, if possible
  if (most_frequent_words_indices.size() < 3) {
    const int n_words = words.size();
    for (int i = 0; i < n_words; i++) {
      bool already_suggested = false;
      for (int j : most_frequent_words_indices)
        if (j == i) {
          already_suggested = false;
          break;
        }
      if (!already_suggested) {
        most_frequent_words_indices.push_back(i);
        if (most_frequent_words_indices.size() == 3) break;
      }
    }
  }

  // retrieve strings for word indices
  std::vector<std::string> most_frequent_words;
  for (int i : most_frequent_words_indices)
    most_frequent_words.push_back(words[i]);

  return most_frequent_words;
}

void Dictionary::update_word_sequencing(const int index) {
  if (first_typed_word)
    first_typed_word = false;
  else
    relative_frequencies[last_typed_word_index][index]++;
  last_typed_word_index = index;
}

std::vector<std::string> Dictionary::get_most_plausible_corrections(
    const std::string& word) const {
  // maintain number of times each substring appears
  const int n_words = word.size();
  std::vector<int> subwords_frequencies(n_words, 0);

  // query substrings of words in substring of dictionary words
  const int len = word.size();
  std::string subword;
  for (int i = 0; i < len; i++) {
    std::vector<int> subword_query =
        partial_words.query(subword + word.substr(i + 1));
    for (int index : subword_query) subwords_frequencies[index]++;
    subword += word[i];
  }

  // filter the 3 words that differ at most one char and appeared most
  // frequently
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      filtered_indices;
  for (int i = 0; i < n_words; i++)
    if (subwords_frequencies[i] == len - 1 &&
        (filtered_indices.size() < 3 ||
         filtered_indices.top().first < abs_frequencies[i])) {
      filtered_indices.emplace(abs_frequencies[i], i);
      if (filtered_indices.size() > 3) filtered_indices.pop();
    }

  // retrieve corrections indices
  std::vector<int> most_plausible_corrections_indices;
  while (!filtered_indices.empty()) {
    int index, frequency;
    std::tie(index, frequency) = filtered_indices.top();
    filtered_indices.pop();
    most_plausible_corrections_indices.push_back(index);
  }

  // sort corrections by frequency
  if (most_plausible_corrections_indices.size() > 1)
    std::swap(most_plausible_corrections_indices[0],
              most_plausible_corrections_indices.back());

  // complete 3 corrections, if possible
  if (most_plausible_corrections_indices.size() < 3) {
    for (int i = 0; i < n_words; i++) {
      bool already_suggested = false;
      for (int j : most_plausible_corrections_indices)
        if (j == i) {
          already_suggested = false;
          break;
        }
      if (!already_suggested) {
        most_plausible_corrections_indices.push_back(i);
        if (most_plausible_corrections_indices.size() == 3) break;
      }
    }
  }

  // retrieve words
  std::vector<std::string> most_plausible_corrections;
  for (int i : most_plausible_corrections_indices)
    most_plausible_corrections.push_back(words[i]);

  return most_plausible_corrections;
}
