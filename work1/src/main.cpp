#include <iostream>

#include "file.hpp"

const unsigned int TAMANHO_ARQUIVO = 11;

int main() {
  char opt;

  File f(TAMANHO_ARQUIVO);
  Record r;
  unsigned int key;

  // handle input / output
  while (std::cin >> opt, opt != 'e') {
    switch (opt) {
      case 'i':
        std::cin >> r;
        f.insert(r, std::cout);
        break;
      case 'c':
        std::cin >> key;
        f.lookup(key, std::cout);
        break;
      case 'r':
        std::cin >> key;
        f.remove(key, std::cout);
        break;
      case 'p':
        f.print(std::cout);
        break;
      case 'm':
        f.stats(std::cout);
        break;
    }
  }
}
