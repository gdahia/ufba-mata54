#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

struct Record {
    bool good;
    unsigned int chave, idade;
    unsigned int next, prev;
    char nome[21];
};

class File {
private:
    const unsigned int file_size;
    const std::string file_name;
    std::fstream handle;
    
    bool already_exists() const;
    void create();
    void open();
    void read_preamble();
    void write_preamble();

public:
    File(const unsigned int, const std::string & file_name = "records.log");
    ~File() = default;
    
};

#endif
