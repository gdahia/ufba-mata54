#include "btree.hpp"

#include <bits/stdc++.h>
using namespace std;

int main() {
  int t;
  cin >> t;
  b::tree<int> tree(t);
  int x;
  vector<int> inserts;
  while (cin >> x) {
    tree.insert(x);
    inserts.push_back(x);
  }

  for (int x : inserts) {
    cout << "deleting " << x << endl;
    tree.erase(x);
    tree.print(cout);
    cout << endl;
  }
}
