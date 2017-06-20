#include "file.hpp"

std::istream & operator >>(std::istream & stream, Record & r) {
    stream >> r.chave;
    stream.ignore(1);
    stream.getline(r.nome, 21);
    stream >> r.idade;
    r.good = true;
    return stream;
}

File::File(const unsigned int file_size, const std::string & file_name) : file_size(file_size), file_name(file_name) {
    if (already_exists())
        open();
    else
        create();
}

File::~File() {
    handle.seekg(sizeof file_size);
    handle.write(reinterpret_cast<const char *>(&next_empty), sizeof next_empty);
}

bool File::already_exists() const {
    std::ifstream f(file_name);
    return f.good();
}

void File::open() {
    handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!handle.is_open())
        throw std::runtime_error("Unable to open existing file " + file_name);

    read_preamble();
}

void File::create() {
    handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    if (!handle.is_open())
        throw std::runtime_error("Unable to create file " + file_name);
    
    // initialize next empty
    next_empty = file_size - 1;
    
    // write preamble
	handle.write(reinterpret_cast<const char *>(&file_size), sizeof file_size);
	handle.write(reinterpret_cast<const char *>(&next_empty), sizeof next_empty);
	
	// initialize empty records as empty space pointers
	// write first record
	Record r;
	r.good = false;
	r.prev = 1;
	r.next = file_size - 1;
	handle.write(reinterpret_cast<const char *>(&r), sizeof r);
	
	// write 1st to (n - 1)th records
	for (unsigned int i = 1; i < file_size - 1; i++) {
		r.prev = i + 1;
		r.next = i - 1;
		handle.write(reinterpret_cast<const char *>(&r), sizeof r);
	}
	
	// write nth record
	r.prev = 0;
	r.next = file_size - 2;
	handle.write(reinterpret_cast<const char *>(&r), sizeof r);
}

void File::read_preamble() {
	unsigned int saved_file_size;
    handle.read(reinterpret_cast<char *>(&saved_file_size), sizeof saved_file_size);
    handle.read(reinterpret_cast<char *>(&next_empty), sizeof next_empty);
    
    if (saved_file_size != file_size)
    	throw std::runtime_error("Unexpected file size. Expected size " + std::to_string(file_size) + " and got " + std::to_string(saved_file_size));
}

unsigned int File::hash(const unsigned int key) {
    return (key % file_size);
}

void File::write(const Record & r, const unsigned int pos) {
    // adjust file pointer
	handle.seekg(sizeof file_size + sizeof next_empty + pos * sizeof(Record));
	
	// write record
	handle.write(reinterpret_cast<const char *>(&r), sizeof r);
}

Record File::read(const unsigned int pos) {
	// adjust file pointer
	handle.seekg(sizeof file_size + sizeof next_empty + pos * sizeof(Record));
	
	// read record
	Record r;
	handle.read(reinterpret_cast<char *>(&r), sizeof r);
	
	return r;
}

void File::remove(const Record & to_remove) {
    // to_remove.prev.next points to to_remove.next
    if (to_remove.prev >= 0) {
        Record prev = read(to_remove.prev);
        prev.next = to_remove.next;
        write(prev, to_remove.prev);
    }
    
    // to_remove.next.prev points to to_remove.prev
    if (to_remove.next >= 0) {
        Record next = read(to_remove.next);
        next.prev = to_remove.prev;
        write(next, to_remove.next);
    }
}

void File::relocate(Record & to_relocate) {
    // to_relocate.prev.next points to to_relocate's new position
    Record prev = read(to_relocate.prev);
    prev.next = next_empty;
    write(prev, to_relocate.prev);
    
    // adjust to_relocate.next pointer to to_relocate's new position
    if (to_relocate.next >= 0) {
        Record next = read(to_relocate.next);
        next.prev = next_empty;
        write(next, to_relocate.next);
    }
    
    // doubly linked list removal of empty slot
    Record empty = read(next_empty);
    remove(empty);
    
    // replace empty slot with to_relocate
    write(to_relocate, next_empty);
    
    // update next empty slot pointer
    next_empty = empty.next;
}

bool File::search(const unsigned int key) {
    Record current = read(hash(key));

    // search key through chain
    while (current.chave != key && current.next >= 0)
        current = read(current.next);

    return (current.good && current.chave == key);
}

unsigned int File::chain(Record & to_chain) {
    // to_chain.next.prev points to to_chain's new position
    if (to_chain.next >= 0) {
        Record next = read(to_chain.next);
        next.prev = next_empty;
        write(next, to_chain.next);
    }
    
    // doubly linked list removal of empty slot
    Record empty = read(next_empty);
    remove(empty);
    
    // to_chain.prev points to new list head
    to_chain.prev = hash(to_chain.chave);
    write(to_chain, next_empty);
    
    // update next empty slot pointer
    const unsigned int old_list_head = next_empty;
    next_empty = empty.next;
    
    // return pointer to old list head
    return old_list_head;
}

void File::insert(Record & to_insert, std::ostream & stream) {
    // hash key
    const unsigned int key_hash = hash(to_insert.chave);
    
    // check if slot is filled
    Record in_place = read(key_hash);
    
    if (!in_place.good) {
        remove(in_place);
        
        // next and prev point to null
        to_insert.prev = to_insert.next = -1;
        
        // update next_empty pointer
        if (key_hash == next_empty)
            next_empty = in_place.next;
    
        // write to slot
        write(to_insert, key_hash);
    }
    else if (key_hash != hash(in_place.chave)) {
        relocate(in_place);
        
        // next and prev point to null
        to_insert.prev = to_insert.next = -1;
        
        // write to slot
        write(to_insert, key_hash);
    }
    else if (!search(to_insert.chave)) {
        to_insert.next = chain(in_place);
        
        // prev points to null
        to_insert.prev = -1;
        
        // write to slot
        write(to_insert, key_hash);
    }
    else
        stream << "chave ja existente " << to_insert.chave << std::endl;
}

void File::lookup(const unsigned int key, std::ostream & stream) {
    const unsigned int hash_key = hash(key);
    Record in_place = read(hash_key);

    if (!in_place.good || hash_key != hash(in_place.chave))
        stream << "chave nao encontrada " << key << std::endl;
    else {
        while (in_place.chave != key && in_place.next > 0)
            in_place = read(in_place.next);
        if (in_place.chave == key)
            stream << "chave: " << key << std::endl << in_place.nome << std::endl << in_place.idade << std::endl;
        else
            stream << "chave nao encontrada " << key << std::endl;
    }
}

void File::remove(const unsigned int key, std::ostream & stream) {
    
}

void File::print(std::ostream & stream) {
	for (unsigned int i = 0; i < file_size; i++) {
	    Record r = read(i);
		stream << i << ": ";
	    if (!r.good)
            stream << "vazio " << r.next << " " << r.prev;
        else {
            stream << r.chave << " " << r.nome << " " << r.idade << " ";
            if (r.next < 0)
                stream << "nulo";
            else
                stream << r.next;
        }
        stream << std::endl;
    }
}
