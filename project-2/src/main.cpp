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
        // query correctnes
        break;
      case 'f':
        dict.print_frequencies(std::cout);
        break;
      case 'p':
        std::cin >> word;
        // query next word
        break;
    }
  }
}
