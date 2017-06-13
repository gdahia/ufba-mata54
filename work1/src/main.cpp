#include <iostream>

#include "file.hpp"

void record_entry() {
    unsigned int chave, idade;
    char nome[21];

    std::cin >> chave;
    std::cin.ignore(1);
    std::cin.getline(nome, 21);
    std::cin >> idade;
    
    std::cout << "nome = " << nome << std::endl;
    std::cout << "chave = " << chave << std::endl;
    std::cout << "idade = " << idade << std::endl;
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
    while (std::cin >> opt, opt != 'e') {
        switch (opt) {
            case 'i': 
                record_entry();
                break;
            case 'c':
                record_lookup();
                break;
            case 'r':
                record_removal();
                break;
            case 'p':
                print_file();
                break;
            case 'm':
                compute_stats();
                break;
        }
    }
}
