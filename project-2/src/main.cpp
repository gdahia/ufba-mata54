#include <iostream>
#include <string>

#include "dictionary.hpp"

int main() {
  Dictionary dict;

  // handle input / output
  char opt;
  std::string word;
  while (std::cin >> opt, opt != 'e') {
    switch (opt) {
      case 'i':
        int n;
        std::cin >> n;
        for (int i = 0; i < n; i++) {
          std::cin >> word;
          dict.insert(word);
        }
        break;

      case 'd':
        std::cin >> word;
        if (!dict.type_word(word, std::cout)) {
          std::cin >> word;

          // treat retyped word as either insertion or correction
          int index = dict.query_correctness(word);
          if (index < 0) index = dict.insert(word);
          dict.update_word_sequencing(index);
        }
        break;

      case 'f':
        dict.print_frequencies(std::cout);
        break;
      case 'p':
        std::cin >> word;
        dict.print_followup_frequencies(word, std::cout);
        break;
    }
  }
}
