#include "radix_tree.h"
#include <iostream>

const int K = 16;
typedef bitset<K> k_t;

int main(void) {
  auto trie = FastTrieNode<uint64_t, 4, K>();

  cout << k_t (trie.getQueryStorageType(k_t(14), 0, true)) << endl;
  cout << k_t (trie.getQueryStorageType(k_t(14), 1, true)) << endl;

  cout << k_t (trie.getQueryStorageType(k_t(10), 3, true)) << endl;
//  cout << k_t (trie.getQueryStorageType(k_t(14), 14)) << endl;
  cout << __builtin_popcount(14) << endl;

  cout << trie.containsSubset(k_t(1), 0) << endl; 

  cout << "Inserting elems" << endl;

  cout << trie.nr_elems() << endl;

  trie.insertSubElem(k_t(13), 0);
  cout << trie.nr_elems() << endl;
 
 trie.insertSubElem(k_t(1), 0);
  cout << trie.nr_elems() << endl;
  trie.insertSubElem(k_t(2), 0);
  cout << trie.nr_elems() << endl;

  trie.insertSubElem(k_t(1), 0);
  cout << trie.nr_elems() << endl;
  trie.insertSubElem(k_t(2), 0);
  cout << trie.nr_elems() << endl;

  cout << trie << endl;

  cout << trie.containsSubset(k_t(1), 0) << endl;
  cout << trie.containsSubset(k_t(2), 0) << endl;
  cout << trie.containsSubset(k_t(5), 0) << endl;
  cout << trie.containsSubset(k_t(16), 0) << endl;
  cout << trie.containsSubset(k_t(7), 0) << endl;
}
