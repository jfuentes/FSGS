#include "catch.hpp"

#include "../radix_tree.h"

TEST_CASE( "Radix Tree Node", "[radix_tree_node]" ) {
  const k_size_t K = 16;
  
  auto block_tree = BlockRadixTreeNode<uint16_t, 4, K>(); 
  cout << block_tree << endl;
}

TEST_CASE( "Radix Tree", "[radix tree]") {
  const k_size_t K = 16;

  auto tree = BlockRadixTree<uint16_t, K>();
  tree.InsertElement(bitset<K>(10));
  tree.InsertElement(bitset<K>(10));

  cout << "RadixTree" << endl;
  cout << tree.root << endl;
}
