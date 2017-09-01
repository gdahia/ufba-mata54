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
    relative_frequencies.push_back(top_n<int>(3));
    for (int j = 0; j <= i; j++) {
      relative_frequencies[i].push(j, retrieve_relative_frequency(i, j));
      relative_frequencies[j].push(i, retrieve_relative_frequency(j, i));
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
  relative_frequencies.push_back(top_n<int>(3));

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
  std::vector<int> most_frequent_words_indices =
      relative_frequencies[index].get_keys();

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

int Dictionary::retrieve_relative_frequency(const int i, const int j) const {
  int relative_frequency;
  std::ifstream relative_frequency_file(std::to_string(i) + "-" +
                                        std::to_string(j) + "-freq.dat");
  if (relative_frequency_file)
    relative_frequency_file >> relative_frequency;
  else
    relative_frequency = 0;
  return relative_frequency;
}

void Dictionary::update_word_sequencing(const int index) {
  if (first_typed_word)
    first_typed_word = false;
  else {
    // update relative frequency in dataset
    const int relative_frequency =
        retrieve_relative_frequency(last_typed_word_index, index) + 1;
    relative_frequencies[last_typed_word_index].push(index, relative_frequency);

    // update/create relative frequncy file
    std::ofstream relative_frequency_file(
        std::to_string(last_typed_word_index) + "-" + std::to_string(index) +
        "-freq.dat");
    relative_frequency_file << relative_frequency;
  }
  last_typed_word_index = index;
  abs_frequencies[index]++;

  // update freqs in files
  std::ofstream freq_file(std::to_string(index) + "-freq.dat");
  freq_file << abs_frequencies[index];
}

std::vector<int> Dictionary::get_most_plausible_corrections(
    const std::string& word) const {
  // query substrings of words in substring of dictionary words
  // taking only those that appear most frequently
  const int len = word.size();
  std::string subword;
  top_n<int> filtered_indices(3);
  for (int i = 0; i < len; i++) {
    // query substring
    std::vector<int> subword_query =
        partial_words.query(subword + word.substr(i + 1));

    // update three corrections as most frequent possible
    for (const int index : subword_query)
      filtered_indices.push(index, abs_frequencies[index]);

    // update string
    subword += word[i];
  }

  // retrieve corrections indices
  std::vector<int> most_plausible_corrections_indices =
      filtered_indices.get_keys();

  // complete 3 corrections, if possible
  const int n_words = words.size();
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
    const std::vector<int>& keys = relative_frequencies[index].get_keys();
    const std::vector<int>& freqs =
        relative_frequencies[index].get_frequencies();
    const int len = keys.size();
    for (int i = 0; i < len; i++)
      if (freqs[i]) os << words[keys[i]] << " " << freqs[i] << std::endl;
  }
}
