#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

struct Record {
    bool good;
    unsigned int chave, idade;
    int next, prev;
    char nome[21];
    
    friend std::istream & operator >>(std::istream &, Record &);
};

class File {
private:
    const unsigned int file_size;
    const std::string file_name;
    
    std::fstream handle;
    unsigned int next_empty;
    
    bool already_exists() const;
    void create();
    void open();
    void read_preamble();
    unsigned int hash(const unsigned int);
    void write(const Record &, const unsigned int);
    void remove(const Record &);
    void relocate(Record &);
    int search(const unsigned int);
    unsigned int chain(Record &);

public:
    File(const unsigned int, const std::string & file_name = "records.log");
    ~File();
    
    Record read(const unsigned int);
    void insert(Record &, std::ostream &);
    void lookup(const unsigned int, std::ostream &);
    void remove(const unsigned int, std::ostream &);
    void print(std::ostream &);
};

#endif
