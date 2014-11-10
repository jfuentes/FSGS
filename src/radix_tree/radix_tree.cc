#include "radix_tree.h"
#include <iostream>

const int K = 8;
typedef bitset<K> k_t;

int main(void) {
  auto trie = FastTrieNode<uint16_t, 4, K>();

  cout << k_t (trie.getQueryStorageType<true>(k_t(14), 0)) << endl;
  cout << k_t (trie.getQueryStorageType<true>(k_t(14), 1)) << endl;

  cout << k_t (trie.getQueryStorageType<true>(k_t(10), 3)) << endl;
//  cout << k_t (trie.getQueryStorageType(k_t(14), 14)) << endl;

  BlocksVector<uint64_t, 4, K> matches;

  cout << trie.containsSubset(k_t(1), 0) << endl; 

  cout << "Inserting elems" << endl;

  cout << trie.elems.nr_elems() << endl;

  trie.InsertElem(k_t(13), 0);
  cout << trie.elems.nr_elems() << endl;
 
  trie.InsertElem(k_t(1), 0);
  cout << trie.elems.nr_elems() << endl;
  trie.InsertElem(k_t(2), 0);
  cout << trie.elems.nr_elems() << endl;

  trie.InsertElem(k_t(1), 0);
  cout << trie.elems.nr_elems() << endl;
  trie.InsertElem(k_t(2), 0);
  cout << trie.elems.nr_elems() << endl;

  trie.InsertElem(k_t(20), 0);
  cout << trie.elems.nr_elems() << endl;


  cout << trie << endl;

  cout << trie.containsSuperset(k_t(1), 0) << endl;
  cout << trie.containsSubset(k_t(1), 0) << endl;

  cout << trie.containsSuperset(k_t(2), 0) << endl;
  cout << trie.containsSubset(k_t(2), 0) << endl;

  cout << trie.containsSuperset(k_t(5), 0) << endl;
  cout << trie.containsSubset(k_t(5), 0) << endl;

  cout << "Test 16" << endl; 
  cout << trie.containsSuperset(k_t(16), 0) << endl;
  cout << trie.containsSubset(k_t(16), 0) << endl;

  cout << "Test 7" << endl; 
  cout << trie.containsSuperset(k_t(7), 0) << endl;
  cout << trie.containsSubset(k_t(7), 0) << endl;

  cout << endl << endl << "Deletion" << endl;

  vector<size_t> idxs_elems_to_rm;
  idxs_elems_to_rm.push_back(0);
  idxs_elems_to_rm.push_back(3);

  cout << trie << endl;
  trie.elems.DeleteElems(idxs_elems_to_rm);
  cout << trie.elems.nr_elems() << endl;
  cout << trie << endl;
  trie.elems.DeleteElems(idxs_elems_to_rm);
  cout << trie.elems.nr_elems() << endl;
  cout << trie << endl;

  cout << "end" << endl;
}
