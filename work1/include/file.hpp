#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

struct Record {
    bool good;
    unsigned int chave, idade;
    unsigned int next, prev;
    char nome[21];
    
    friend std::ostream & operator <<(std::ostream &, const Record &);
    friend std::istream & operator >>(std::istream &, Record &);
};

class File {
private:
    const unsigned int file_size;
    const std::string file_name;
    
    std::fstream handle;
    unsigned int next_free;
    
    bool already_exists() const;
    void create();
    void open();
    void read_preamble();

public:
    File(const unsigned int, const std::string & file_name = "records.log");
    ~File() = default;
    
    Record read(const unsigned int);
    void print(std::ostream &);
};

#endif
