#include "btree.hpp"

#include <bits/stdc++.h>
using namespace std;

int main() {
  int t;
  cin >> t;
  b::tree<int> tree(t);
  int x;
  while (cin >> x) {
    tree.insert(x);
    tree.print(cout);
  }
}
