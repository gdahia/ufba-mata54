#include "file.hpp"

std::ostream & operator <<(std::ostream & stream, const Record & r) {
    if (r.good)
        stream << r.chave << " " << r.nome << " " << r.idade << " " << r.next;
    else
        stream << "vazio nulo";
    return stream;
}

std::istream & operator >>(std::istream & stream, Record & r) {
    stream >> r.chave;
    stream.ignore(1);
    stream.getline(r.nome, 21);
    stream >> r.idade;
    return stream;
}

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
    
    // initialize next free
    next_free = 0;
    
    // write preamble
	handle.write(reinterpret_cast<const char *>(&file_size), sizeof file_size);
	handle.write(reinterpret_cast<const char *>(&next_free), sizeof next_free);
	
	// initialize empty records as free space pointers
	// write first record
	Record r;
	r.good = false;
	r.prev = file_size - 1;
	r.next = 1;
	handle.write(reinterpret_cast<const char *>(&r), sizeof r);
	
	// write 1st to (n - 1)th records
	for (unsigned int i = 1; i < file_size - 1; i++) {
		r.prev = i - 1;
		r.next = i + 1;
		handle.write(reinterpret_cast<const char *>(&r), sizeof r);
	}
	
	// write nth record
	r.prev = file_size - 2;
	r.next = 0;
	handle.write(reinterpret_cast<const char *>(&r), sizeof r);
}

void File::read_preamble() {
	unsigned int saved_file_size;
    handle.read(reinterpret_cast<char *>(&saved_file_size), sizeof saved_file_size);
    handle.read(reinterpret_cast<char *>(&next_free), sizeof next_free);
    
    if (saved_file_size != file_size)
    	throw std::runtime_error("Unexpected file size. Expected size " + std::to_string(file_size) + " and got " + std::to_string(saved_file_size));
}

Record File::read(const unsigned int pos) {
	// adjust file pointer
	handle.seekg(sizeof file_size + sizeof next_free + pos * sizeof(Record));
	
	// read record
	Record r;
	handle.read(reinterpret_cast<char *>(&r), sizeof r);
	
	return r;
}

void File::print(std::ostream & stream) {
	for (unsigned int i = 0; i < file_size; i++)
		stream << i << ": " << read(i) << std::endl;
}
