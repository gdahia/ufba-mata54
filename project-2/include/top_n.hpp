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
    // prevent repeated key insertion
    for (int i = 0; i < size; i++)
      if (keys[i] == key) {
        // update frequency
        freqs[i] = freq;

        // insert and keep sorted
        for (int j = i - 1; j >= 0 && freqs[j] < freqs[j + 1]; j--) {
          std::swap(freqs[j], freqs[j + 1]);
          std::swap(keys[j], keys[j + 1]);
        }

        should_insert = false;
      }

    if (should_insert) {
      keys.push_back(key);
      freqs.push_back(freq);

      // restore ordering
      for (int i = size - 1; i >= 0 && freqs[i] < freqs[i + 1]; i--) {
        std::swap(freqs[i], freqs[i + 1]);
        std::swap(keys[i], keys[i + 1]);
      }

      size++;
    }

    // discard extraneous
    if (size > n) {
      keys.pop_back();
      freqs.pop_back();
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
