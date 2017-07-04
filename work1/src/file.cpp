#include "file.hpp"

#include <iomanip>

std::istream &operator>>(std::istream &stream, Record &r) {
  stream >> r.key;
  stream.ignore(1);
  stream.getline(r.name, 21);
  stream >> r.age;
  r.good = true;
  return stream;
}

File::File(const unsigned int file_size, const std::string &file_name)
    : file_size(file_size), file_name(file_name) {
  if (already_exists())
    open();
  else
    create();
}

File::~File() {
  // updates header to file
  handle.seekg(sizeof file_size);
  handle.write(reinterpret_cast<const char *>(&next_empty), sizeof next_empty);
}

bool File::already_exists() const {
  /* checks existence of file in path 'file_name'.
  - returns: 'true' if file already exists and is accessible, and 'false'
  otherwise */

  std::ifstream f(file_name);
  return f.good();
}

void File::open() {
  /* opens file with path 'file_name' (without discarding its content) for
   * reading and writing in binary mode. */

  handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary);

  if (!handle.is_open())
    throw std::runtime_error("Unable to open existing file " + file_name);

  read_header();
}

void File::create() {
  /* creates new file with path 'file_name', initializing the empty slots with a
   * linked list of their positions. */

  handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary |
                             std::ios::trunc);
  if (!handle.is_open())
    throw std::runtime_error("Unable to create file " + file_name);

  // initialize next empty slot pointer
  next_empty = file_size - 1;

  // write header
  handle.write(reinterpret_cast<const char *>(&file_size), sizeof file_size);
  handle.write(reinterpret_cast<const char *>(&next_empty), sizeof next_empty);

  // initialize empty slots with linked list
  // write first empty slot
  Record empty;
  empty.good = false;
  empty.prev = 1;
  empty.next = -1;
  handle.write(reinterpret_cast<const char *>(&empty), sizeof empty);

  // write internal empty slots
  for (unsigned int i = 1; i < file_size - 1; i++) {
    empty.prev = i + 1;
    empty.next = i - 1;
    handle.write(reinterpret_cast<const char *>(&empty), sizeof empty);
  }

  // write last empty slot
  empty.prev = -1;
  empty.next = file_size - 2;
  handle.write(reinterpret_cast<const char *>(&empty), sizeof empty);
}

void File::read_header() {
  /* reads the header of a previously opened file. */

  unsigned int saved_file_size;
  handle.read(reinterpret_cast<char *>(&saved_file_size),
              sizeof saved_file_size);
  handle.read(reinterpret_cast<char *>(&next_empty), sizeof next_empty);

  // checks if 'saved_file_size' equals current 'file_size'
  if (saved_file_size != file_size)
    throw std::runtime_error("Unexpected file size. Expected size " +
                             std::to_string(file_size) + " and got " +
                             std::to_string(saved_file_size));
}

unsigned int File::hash(const unsigned int key) {
  /* hashes 'key' with chosen hash function.
  - 'key': key to be hashed
  - returns: 'key' hash value */

  return (key % file_size);
}

void File::write(const Record &r, const unsigned int pos) {
  /* writes a record 'r' into 'pos' file position.
  - 'r': record to be written to file
  - 'pos': position in file to write record to */

  // adjust file pointer, considering header space
  handle.seekg((sizeof file_size) + (sizeof next_empty) + pos * sizeof(Record));

  handle.write(reinterpret_cast<const char *>(&r), sizeof r);
}

Record File::read(const unsigned int pos) {
  /* read record in 'pos' file position.
  - 'pos': position in file to be read
  - returns: record read */

  // adjust file pointer to 'pos' position
  handle.seekg((sizeof file_size) + (sizeof next_empty) + pos * sizeof(Record));

  Record r;
  handle.read(reinterpret_cast<char *>(&r), sizeof r);

  return r;
}

void File::erase(const Record &to_erase) {
  /* erase record 'to_erase' from linked list.
  - 'to_erase': constant reference to file to be erased from linked list */

  // to_erase.prev.next points to to_erase.next
  if (to_erase.prev >= 0) {
    Record prev = read(to_erase.prev);
    prev.next = to_erase.next;
    write(prev, to_erase.prev);
  }

  // to_erase.next.prev points to to_erase.prev
  if (to_erase.next >= 0) {
    Record next = read(to_erase.next);
    next.prev = to_erase.prev;
    write(next, to_erase.next);
  }
}

void File::relocate(Record &to_relocate) {
  /* relocates record 'to_relocate' to next empty slot; 'to_relocate' is
  guaranteed to have a valid 'prev' pointer.
  - 'to_relocate': reference of record to be relocated */

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
  erase(empty);

  // replace empty slot with to_relocate
  write(to_relocate, next_empty);

  // update next empty slot pointer
  next_empty = empty.next;
}

int File::search(const unsigned int key) {
  /* searches file for record with key 'key'.
  - 'key': key of record being searched
  - returns: index of found record, or -1 on unsuccessful search */

  int found_index = hash(key);
  Record current = read(found_index);

  // search key through chain
  while (current.key != key && current.next >= 0) {
    found_index = current.next;
    current = read(current.next);
  }

  if (!current.good || current.key != key)
    return -1;
  else
    return found_index;
}

void File::chain(Record &to_chain) {
  /* makes list head 'to_chain' the second element of the list.
  - 'to_chain': reference to current list head */

  // to_chain.next.prev points to to_chain's new position
  if (to_chain.next >= 0) {
    Record next = read(to_chain.next);
    next.prev = next_empty;
    write(next, to_chain.next);
  }

  // doubly linked list removal of empty slot
  Record empty = read(next_empty);
  erase(empty);

  // to_chain.prev points to new list head
  to_chain.prev = hash(to_chain.key);

  write(to_chain, next_empty);

  // update next empty slot pointer
  next_empty = empty.next;
}

void File::insert(Record &to_insert, std::ostream &stream) {
  /* inserts record 'to_insert' in file if it has no record with this same key,
  indicating otherwise.
  - 'to_insert': reference to record to be inserted
  - 'stream': ostream reference to output operations log */

  const unsigned int key_hash = hash(to_insert.key);
  Record in_place = read(key_hash);
  if (!in_place.good) {
    // empty slot found

    // erases empty slot and adjust next empty pointer if necessary
    erase(in_place);
    if (key_hash == next_empty) next_empty = in_place.next;

    // write first linked list element
    to_insert.prev = to_insert.next = -1;
    write(to_insert, key_hash);

  } else if (key_hash != hash(in_place.key)) {
    // current occupant of slot hashes to other position, ie this is first
    // proper record to hash to this position

    relocate(in_place);

    // write first linked list element
    to_insert.prev = to_insert.next = -1;
    write(to_insert, key_hash);

  } else if (search(to_insert.key) < 0) {
    // current slot occupant hashes to same key, but inserted key is not present

    // to_insert.next points to position to be occupied be current list head
    to_insert.next = next_empty;

    // linked list insertion
    chain(in_place);
    to_insert.prev = -1;
    write(to_insert, key_hash);

  } else {
    stream << "chave ja existente " << to_insert.key << std::endl;
  }
}

void File::lookup(const unsigned int key, std::ostream &stream) {
  /* looks up record with key 'key'.
  - 'key': key to be looked up
  - 'stream': ostream reference to output operations log */

  const unsigned int hash_key = hash(key);
  Record in_place = read(hash_key);

  // checks existence of records with keys hashing to 'hash_key'
  if (!in_place.good || hash_key != hash(in_place.key)) {
    // no record has key hashing to 'hash_key'; hence, 'key' is key to no record
    stream << "chave nao encontrada " << key << std::endl;

  } else {
    // some record has key hashing to 'hash_key'

    // search for record with key 'key' through linked list
    while (in_place.key != key && in_place.next >= 0)
      in_place = read(in_place.next);

    if (in_place.key == key)
      stream << "chave: " << key << std::endl
             << in_place.name << std::endl
             << in_place.age << std::endl;
    else
      stream << "chave nao encontrada " << key << std::endl;
  }
}

void File::remove(const unsigned int key, std::ostream &stream) {
  /* removes record with key 'key' if it is present in file, indicating
  otherwise.
  - 'key': key of record to be removed
  - 'stream': ostream reference to output operations log */

  const int index = search(key);

  // checks if search was successful
  if (index < 0) {
    stream << "chave nao encontrada " << key << std::endl;
  } else {
    Record to_erase = read(index);

    // empty record
    Record empty;
    empty.good = false;
    empty.next = next_empty;
    empty.prev = -1;
    const int next_next_empty = next_empty;

    // replace removed record with either next or empty record
    Record replacement;
    if (to_erase.next < 0) {
      next_empty = index;
      replacement = empty;

      // point to_erase.prev.next to null
      if (to_erase.prev >= 0) {
        Record prev = read(to_erase.prev);
        prev.next = -1;
        write(prev, to_erase.prev);
      }
    } else {
      replacement = read(to_erase.next);
      replacement.prev = to_erase.prev;

      // to_erase.next is made empty
      next_empty = to_erase.next;
      write(empty, to_erase.next);

      // point replacement.next.prev to replacements new position
      if (replacement.next >= 0) {
        Record next = read(replacement.next);
        next.prev = index;
        write(next, replacement.next);
      }
    }

    // adjust empty file list
    if (next_next_empty >= 0) {
      Record second = read(next_next_empty);
      second.prev = next_empty;
      write(second, next_next_empty);
    }

    write(replacement, index);
  }
}

void File::print(std::ostream &stream) {
  /* output formatted file contents.
  - 'stream': ostream reference to output operations log */

  for (unsigned int i = 0; i < file_size; i++) {
    Record current = read(i);
    stream << i << ": ";

    // if slot is not filled
    if (!current.good)
      stream << "vazio nulo";
    else {
      stream << current.key << " " << current.name << " " << current.age << " ";

      if (current.next < 0)
        stream << "nulo";
      else
        stream << current.next;
    }
    stream << std::endl;
  }
}

void File::stats(std::ostream &stream) {
  /* iterate over records computing average access time E(A).
  - 'stream': ostream reference to output operations log */

  unsigned int access_time = 0;
  unsigned int number_of_records = 0;

  for (unsigned int i = 0; i < file_size; i++) {
    Record current = read(i);

    if (current.good) {
      number_of_records++;
      access_time++;

      // follow chain backwards to compute access time of ith record
      while (current.prev >= 0) {
        current = read(current.prev);
        access_time++;
      }
    }
  }

  const double average_access_time = (double)access_time / number_of_records;

  stream << std::fixed << std::setprecision(1) << average_access_time
         << std::endl;
}
