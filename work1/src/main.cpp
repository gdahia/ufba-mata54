#include <iostream>

#include "file.hpp"

Record read_record() {
    Record r;
    
    std::cin >> r.chave;
    std::cin.ignore(1);
    std::cin.getline(r.nome, 21);
    std::cin >> r.idade;
    
    return r;
}

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
    
    // handle input / output
    while (std::cin >> opt, opt != 'e') {
        switch (opt) {
            case 'i': 
                read_record();
                break;
            case 'c':
                record_lookup();
                break;
            case 'r':
                record_removal();
                break;
            case 'p':
                f.print();
                break;
            case 'm':
                compute_stats();
                break;
        }
    }
}
