#include <iostream>

int main() {
  char opt;
  std::string word;

  // handle input / output
  while (std::cin >> opt, opt != 'e') {
    switch (opt) {
      case 'i':
        int n;
        std::cin >> n;

        for (int i = 0; i < n; i++) {
          std::cin >> word;
          // insert
        }

        break;
      case 'd':
        std::cin >> word;
        // query correctnes
        break;
      case 'f':
        // frequency
        break;
      case 'p':
        std::cin >> word;
        // query next word
        break;
    }
  }
}
