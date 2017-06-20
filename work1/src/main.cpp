#include <iostream>

#include "file.hpp"

void record_lookup() {

}

void record_removal() {

}

void print_file() {

}

void compute_stats() {

}

const unsigned int TAMANHO_ARQUIVO = 11;

int main() {
    char opt;
    
    File f(TAMANHO_ARQUIVO);
    Record r;
    
    // handle input / output
    while (std::cin >> opt, opt != 'e') {
        switch (opt) {
            case 'i': 
                std::cin >> r;
                std::cout << r << std::endl;
                break;
            case 'c':
                record_lookup();
                break;
            case 'r':
                record_removal();
                break;
            case 'p':
                f.print(std::cout);
                break;
            case 'm':
                compute_stats();
                break;
        }
    }
}
