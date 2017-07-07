#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>

struct Record {
  bool good;
  unsigned int key, age;
  int next, prev;
  char name[21];

  friend std::istream &operator>>(std::istream &, Record &);
};

class File {
 private:
  const unsigned int file_size;
  const std::string file_name;

  std::fstream handle;
  int empty_list_head;

  bool already_exists() const;
  void create();
  void open();
  void read_header();
  unsigned int hash(const unsigned int);
  void write(const Record &, const unsigned int);
  void empty_list_delete(const Record &);
  void relocate(Record &);
  int search(const unsigned int);
  void chain(Record &);

 public:
  File(const unsigned int, const std::string &file_name = "records.log");
  ~File();
  File(const File &) = delete;
  File(File &&) = delete;
  File &operator=(const File &) = delete;

  Record read(const unsigned int);
  void insert(Record &, std::ostream &);
  void lookup(const unsigned int, std::ostream &);
  void remove(const unsigned int, std::ostream &);
  void print(std::ostream &);
  void stats(std::ostream &);
};

#endif
