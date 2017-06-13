#include "file.hpp"

File::File(const unsigned int file_size, const std::string & file_name) : file_size(file_size), file_name(file_name) {
    if (already_exists())
        open();
    else
        create();
}

bool File::already_exists() const {
    std::ifstream f(file_name);
    return f.good();
}

void File::open() {
    handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
    if (!handle.is_open())
        throw std::runtime_error("Unable to open existing file " + file_name);

    read_preamble();
}

void File::create() {
    handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    if (!handle.is_open())
        throw std::runtime_error("Unable to create file " + file_name);
    
    write_preamble();
}

void File::read_preamble() {
    
}

void File::write_preamble() {

}
