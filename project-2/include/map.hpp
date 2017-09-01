#ifndef MAP_HPP
#define MAP_HPP

#include <utility>
#include <vector>

#include "btree.hpp"

template <class Key, class T>
class Map {
 public:
  Map(const int min_bucket_size) : btree(min_bucket_size + 1) {}
  ~Map() = default;

  T& operator[](const Key& key) {
    std::pair<Key, T>* node = btree.search(std::make_pair(key, T()));
    if (node == NULL) return btree.insert(std::make_pair(key, T()))->second;
    return node->second;
  }

  const std::vector<std::pair<Key, T>>& get_last_bucket() const {}

 private:
  struct pair_first_comparator {
    bool operator()(const std::pair<Key, T>& a, const std::pair<Key, T>& b) {
      return std::greater<Key>(b.first, a.first);
    }
  };

  b::tree<std::pair<Key, T>, pair_first_comparator> btree;
};

#endif
