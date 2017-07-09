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
  handle.write(reinterpret_cast<const char *>(&empty_list_head),
               sizeof empty_list_head);
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
  /* creates new file with path 'file_name', initializing the empty positions
   * with a
   * linked list of their positions. */

  handle.open(file_name, std::ios::in | std::ios::out | std::ios::binary |
                             std::ios::trunc);
  if (!handle.is_open())
    throw std::runtime_error("Unable to create file " + file_name);

  // initialize next empty position pointer
  empty_list_head = file_size - 1;

  // write header
  handle.write(reinterpret_cast<const char *>(&file_size), sizeof file_size);
  handle.write(reinterpret_cast<const char *>(&empty_list_head),
               sizeof empty_list_head);

  // initialize empty positions with linked list
  // write first empty position
  Record empty;
  empty.good = false;
  empty.prev = (file_size > 1 ? 1 : -1);
  empty.next = -1;
  handle.write(reinterpret_cast<const char *>(&empty), sizeof empty);

  // write internal empty positions
  for (unsigned int i = 1; i < file_size - 1; i++) {
    empty.prev = i + 1;
    empty.next = i - 1;
    handle.write(reinterpret_cast<const char *>(&empty), sizeof empty);
  }

  // write last empty position
  empty.prev = -1;
  empty.next = (file_size > 1 ? file_size - 2 : -1);
  handle.write(reinterpret_cast<const char *>(&empty), sizeof empty);
}

void File::read_header() {
  /* reads the header of a previously opened file. */

  unsigned int saved_file_size;
  handle.read(reinterpret_cast<char *>(&saved_file_size),
              sizeof saved_file_size);
  handle.read(reinterpret_cast<char *>(&empty_list_head),
              sizeof empty_list_head);

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
  handle.seekg((sizeof file_size) + (sizeof empty_list_head) +
               pos * sizeof(Record));

  handle.write(reinterpret_cast<const char *>(&r), sizeof r);
}

Record File::read(const unsigned int pos) {
  /* read record in 'pos' file position.
  - 'pos': position in file to be read
  - returns: record read */

  // adjust file pointer to 'pos' position
  handle.seekg((sizeof file_size) + (sizeof empty_list_head) +
               pos * sizeof(Record));

  Record r;
  handle.read(reinterpret_cast<char *>(&r), sizeof r);

  return r;
}

void File::empty_list_delete(const Record &to_delete) {
  /* erase record 'to_delete' from linked list.
  - 'to_delete': constant reference to file to be erased from linked list */

  // to_delete.prev.next points to to_erase.next
  if (to_delete.prev < 0)
    empty_list_head = to_delete.next;
  else {
    Record prev = read(to_delete.prev);
    prev.next = to_delete.next;
    write(prev, to_delete.prev);
  }

  // to_delete.next.prev points to to_erase.prev
  if (to_delete.next >= 0) {
    Record next = read(to_delete.next);
    next.prev = to_delete.prev;
    write(next, to_delete.next);
  }
}

int File::search(const unsigned int key) {
  /* searches file for record with key 'key'.
  - 'key': key of record being searched
  - returns: index of found record, or -1 on unsuccessful search */

  int found_index = hash(key);
  Record current = read(found_index);

  if (current.good) {
    // search key through list
    while (current.key != key && current.next >= 0) {
      found_index = current.next;
      current = read(found_index);
    }

    if (current.key == key) return found_index;
  }

  return -1;
}

void File::insert(Record &to_insert, std::ostream &stream) {
  /* inserts record 'to_insert' in file if it has no record with this same key,
  indicating otherwise.
  - 'to_insert': reference to record to be inserted
  - 'stream': ostream reference to output operations log */

  const unsigned int key_hash = hash(to_insert.key);
  Record in_place = read(key_hash);
  if (!in_place.good) {
    // empty position found

    // erases empty position
    empty_list_delete(in_place);

    // write linked list head
    to_insert.prev = to_insert.next = -1;
    write(to_insert, key_hash);

  } else if (key_hash != hash(in_place.key)) {
    // in_place is illegitimate, ie, to_insert is not in the file

    // in_place.prev.next points to in_place's new position
    Record prev = read(in_place.prev);
    prev.next = empty_list_head;
    write(prev, in_place.prev);

    // adjust in_place.next pointer to in_place's new position
    if (in_place.next >= 0) {
      Record next = read(in_place.next);
      next.prev = empty_list_head;
      write(next, in_place.next);
    }

    // save relocation position prior to empty list update
    const int relocation_pos = empty_list_head;

    // doubly linked list removal of empty position
    Record empty = read(empty_list_head);
    empty_list_delete(empty);

    // replace empty position with in_place
    write(in_place, relocation_pos);

    // write linked list head
    to_insert.prev = to_insert.next = -1;
    write(to_insert, key_hash);

  } else if (search(to_insert.key) < 0) {
    // in_place is legitimate and to_insert is not in the file

    // adjust to_insert pointers
    to_insert.next = empty_list_head;
    to_insert.prev = -1;

    // in_place.next.prev points to in_place's new position
    if (in_place.next >= 0) {
      Record next = read(in_place.next);
      next.prev = empty_list_head;
      write(next, in_place.next);
    }

    // save chaining position prior to empty list update
    const int chain_pos = empty_list_head;

    // doubly linked list removal of empty position
    Record empty = read(empty_list_head);
    empty_list_delete(empty);

    // in_place.prev points to new list head
    in_place.prev = key_hash;

    // write changes to file
    write(in_place, chain_pos);
    write(to_insert, key_hash);

  } else {
    stream << "chave ja existente: " << to_insert.key << std::endl;
  }
}

void File::lookup(const unsigned int key, std::ostream &stream) {
  /* looks up record with key 'key'.
  - 'key': key to be looked up
  - 'stream': ostream reference to output operations log */

  const int index = search(key);

  if (index >= 0) {
    Record in_place = read(index);
    stream << "chave: " << key << std::endl
           << in_place.name << std::endl
           << in_place.age << std::endl;
  } else
    stream << "chave nao encontrada: " << key << std::endl;
}

void File::remove(const unsigned int key, std::ostream &stream) {
  /* removes record with key 'key' if it is present in file, indicating
  otherwise.
  - 'key': key of record to be removed
  - 'stream': ostream reference to output operations log */

  const int index = search(key);

  // checks if search was successful
  if (index < 0) {
    stream << "chave nao encontrada: " << key << std::endl;
  } else {
    Record to_erase = read(index);

    // empty record
    Record empty;
    empty.good = false;
    empty.next = empty_list_head;
    empty.prev = -1;

    // replace removed record with either next, if not last in list, or empty
    // record, otherwise
    Record replacement;
    if (to_erase.next < 0) {
      empty_list_head = index;
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
      empty_list_head = to_erase.next;
      write(empty, to_erase.next);

      // point replacement.next.prev to replacements new position
      if (replacement.next >= 0) {
        Record next = read(replacement.next);
        next.prev = index;
        write(next, replacement.next);
      }
    }

    // adjust empty positions list
    if (empty.next >= 0) {
      Record second = read(empty.next);
      second.prev = empty_list_head;
      write(second, empty.next);
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

    // if position is not filled
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

      // follow list backwards to compute access time of ith record
      while (current.prev >= 0) {
        current = read(current.prev);
        access_time++;
      }
    }
  }

  if (!number_of_records)
    stream << "0.0" << std::endl;
  else {
    const double average_access_time = (double)access_time / number_of_records;

    stream << std::fixed << std::setprecision(1) << average_access_time
           << std::endl;
  }
}
