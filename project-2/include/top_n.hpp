#ifndef TOP_N_HPP
#define TOP_N_HPP

#include <queue>
#include <vector>

template <class T, class Compare = std::greater<int>>
class top_n {
 public:
  top_n(const int n) : n(n) {}
  ~top_n() = default;

  void push(const T& key, const int freq) {
    bool should_insert = true;
    int size = keys.size();
    for (int i = 0; i < size; i++)
      if (keys[i] == key) {
        freqs[i] = freq;
        should_insert = false;
      }

    if (should_insert) {
      keys.push_back(key);
      freqs.push_back(freq);
      size++;
    }

    sort();

    if (size > n) {
      keys.pop_back();
      freqs.pop_back();
    }
  }

  void sort() {
    const int size = keys.size();
    for (int i = 0; i < size; i++) {
      int l = i;
      for (int j = i + 1; j < size; j++)
        if (!Compare()(freqs[l], freqs[j])) l = j;
      std::swap(keys[i], keys[l]);
      std::swap(freqs[i], freqs[l]);
    }
  }

  const std::vector<int>& get_keys() const { return keys; }
  const std::vector<int>& get_frequencies() const { return freqs; }

 private:
  const int n;
  std::vector<T> keys;
  std::vector<int> freqs;
};

#endif
