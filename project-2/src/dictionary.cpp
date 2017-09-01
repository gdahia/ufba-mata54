#include "dictionary.hpp"

#include <algorithm>
#include <fstream>
#include <queue>

Dictionary::Dictionary() {
  first_typed_word = true;

  // restore dictionary state
  for (int i = 0;; i++) {
    // read ith word from its file
    std::ifstream input(std::to_string(i) + "-word.dat");
    if (!input) break;
    std::string word;
    char c;
    while (input >> c) word += c;
    words.push_back(word);

    // read ith word frequency from its file
    std::ifstream freq_input(std::to_string(i) + "-freq.dat");
    int abs_frequency;
    freq_input >> abs_frequency;
    abs_frequencies.push_back(abs_frequency);

    // read ith word relative frequencies to all predecessors
    relative_frequencies.push_back(std::map<int, int>());
    for (int j = 0; j <= i; j++) {
      // read i-j relative frequency
      std::ifstream input(std::to_string(i) + "-" + std::to_string(j) +
                          "-freq.dat");
      if (input) {
        int rel_frequency;
        input >> rel_frequency;
        if (rel_frequency) relative_frequencies[i][j] = rel_frequency;
      }

      // read j-i relative frequency
      std::ifstream rev_input(std::to_string(j) + "-" + std::to_string(i) +
                              "-freq.dat");
      if (rev_input) {
        int rel_frequency;
        rev_input >> rel_frequency;
        if (rel_frequency) relative_frequencies[j][i] = rel_frequency;
      }
    }

    // rebuild tries
    whole_words.insert(word, i);

    std::string partial_word;
    const int len = word.size();
    for (int j = 0; j < len; j++) {
      partial_words.insert(partial_word + word.substr(j + 1), i);
      partial_word += word[j];
    }
  }
}

int Dictionary::insert(const std::string& word) {
  // add word to database
  const int index = words.size();
  words.push_back(word);
  abs_frequencies.push_back(0);
  relative_frequencies.push_back(std::map<int, int>());

  // add word to tries
  whole_words.insert(word, index);

  std::string partial_word;
  const int len = word.size();
  for (int i = 0; i < len; i++) {
    partial_words.insert(partial_word + word.substr(i + 1), index);
    partial_word += word[i];
  }

  // create word file
  std::ofstream word_file(std::to_string(index) + "-word.dat");
  word_file << word;

  // create frequency file
  std::ofstream freq_file(std::to_string(index) + "-freq.dat");
  freq_file << 0;

  return index;
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

std::vector<int> Dictionary::get_most_frequent_followups(
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
          already_suggested = true;
          break;
        }
      if (!already_suggested) {
        most_frequent_words_indices.push_back(i);
        if (most_frequent_words_indices.size() == 3) break;
      }
    }
  }

  return most_frequent_words_indices;
}

void Dictionary::update_word_sequencing(const int index) {
  if (first_typed_word)
    first_typed_word = false;
  else {
    // update relative frequency in dataset
    const int relative_frequency =
        ++relative_frequencies[last_typed_word_index][index];

    // update/create relative frequncy file
    std::ofstream relfreq_file(std::to_string(last_typed_word_index) + "-" +
                               std::to_string(index) + "-freq.dat");
    relfreq_file << relative_frequency;
  }
  last_typed_word_index = index;
  abs_frequencies[index]++;

  // update freqs in files
  std::ofstream freq_file(std::to_string(index) + "-freq.dat");
  freq_file << abs_frequencies[index];
}

std::vector<int> Dictionary::get_most_plausible_corrections(
    const std::string& word) const {
  // maintain number of times each substring appears
  const int n_words = words.size();
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
          already_suggested = true;
          break;
        }
      if (!already_suggested) {
        most_plausible_corrections_indices.push_back(i);
        if (most_plausible_corrections_indices.size() == 3) break;
      }
    }
  }

  return most_plausible_corrections_indices;
}

bool Dictionary::type_word(const std::string& word, std::ostream& os) {
  const int index = query_correctness(word);
  if (index >= 0) {
    // print followup suggestions
    os << "proximas palavras:";
    for (const int i : get_most_frequent_followups(index))
      os << " " << words[i];
    os << std::endl;

    // update followup frequencies
    update_word_sequencing(index);

    return true;
  } else {
    // print correction suggestions
    os << "palavra desconhecida. possiveis correcoes:";
    for (const int i : get_most_plausible_corrections(word))
      os << " " << words[i];
    os << std::endl;

    return false;
  }
}

void Dictionary::print_followup_frequencies(const std::string& word,
                                            std::ostream& os) const {
  const int index = query_correctness(word);
  if (index >= 0) {
    const std::vector<int> followup_indices =
        get_most_frequent_followups(index);
    const std::map<int, int>& relative_frequency = relative_frequencies[index];
    for (const int i : followup_indices) {
      auto it = relative_frequency.find(i);
      if (it != relative_frequency.cend())
        os << words[i] << " " << it->second << std::endl;
    }
  }
}
