#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

template <class T>
class File {
private:
    std::fstream handle;

public:
    File() {
    
    }
    File(const std::string & path) {
        open(path);
    }
    
    void open(const std::string & path) {
        handle.open(path.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        if (!handle.is_open())
            throw std::runtime_error("Unable to open file " + path);
        handle.fill(0);
    }
    
    T read(const unsigned int pos) {
        if (!handle.is_open())
            throw std::runtime_error("Attempt to read without opening file");
        
        T record;
        handle.seekg(pos * sizeof(T));
        handle.read(reinterpret_cast<char *>(&record), sizeof record);
        
        if (!handle) {
            record.good = false;
            handle.clear();
        }

        return record;
    }
    
    void write(const T & record, const unsigned int pos) {
        if (!handle.is_open())
            throw std::runtime_error("Attempt to write without opening file");
    
        handle.seekp(pos * sizeof(T));
        handle.write(reinterpret_cast<const char *>(&record), sizeof record);
    }
};

#endif
