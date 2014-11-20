#include "catch.hpp"
#include "../radix_tree.h"

TEST_CASE( "Radix Tree Node", "[radix_tree_node]" ) {
  const k_size_t K = 16;
  cout << "about to start radix tree node" << endl;  
  auto block_tree = BlockRadixTreeNode<uint16_t, 4, K>(); 
  cout << block_tree << endl;
}

TEST_CASE( "Radix Tree", "[radix_tree]") {
  const k_size_t K = 16;
  const k_size_t B = 4;

  cout << "about to start radix tree" << endl;
  auto tree = BlockRadixTree<uint16_t, B, K>();
  cout << "created radix tree" << endl;
  tree.InsertElement(bitset<K>(10));
  cout << "inserting 2nd" << endl;
  tree.InsertElement(bitset<K>(10));

  REQUIRE(tree.root.elems.nr_elems() == 1);

  cout << "RadixTree" << endl;
  cout << tree.root << endl;
  tree.InsertElement(bitset<K>(11)); // Insert 00001011
  tree.InsertElement(bitset<K>(110));// Insert 01101110
  tree.InsertElement(bitset<K>(210));// Insert 11010010
  tree.InsertElement(bitset<K>(15)); // Insert 00001111
  cout << tree.root << endl;

  SECTION( "Vector Extraction") {
    vector<bitset<K> > expected;
    expected.push_back(bitset<K>(10));
    expected.push_back(bitset<K>(11));
    expected.push_back(bitset<K>(110));
    expected.push_back(bitset<K>(210));
    expected.push_back(bitset<K>(15));

    // Vectors should be equal of to sorting
    sort(expected.begin(), expected.end(), compare_bitsets<K>);

    const auto & generated = tree.ExtractElements(true);

    REQUIRE(expected.size() == generated.size());

    // Vectors should be equal of to sorting
    sort(expected.begin(), expected.end(), compare_bitsets<K>);
    cout << "aaaaaaaaaaaaaaaaaaaaaaaa" << endl;
    for( auto & e : generated) cout << e << endl;
    for( unsigned int i = 0 ; i < expected.size(); ++i) {
      REQUIRE(expected[i] == generated[i]);
    }
  }

  SECTION( "Compaction I") {

    tree.Compact();

    vector<bitset<K> > expected;
    expected.push_back(bitset<K>(110));
    expected.push_back(bitset<K>(210));
    expected.push_back(bitset<K>(15));
    // Vectors should be equal of to sorting
    sort(expected.begin(), expected.end(), compare_bitsets<K>);

    const auto & generated = tree.ExtractElements(true);

    for( unsigned int i = 0 ; i < expected.size(); ++i) {
      REQUIRE(expected[i] == generated[i]);
    }
  }

  cout << tree.root << endl;
  tree.Compact();
  cout << tree.root << endl;
  REQUIRE(tree.root.elems.nr_elems() == 1);

  tree.InsertElement(bitset<K>("1010101010101010"));
  cout << tree.root << endl;
 
  tree.InsertElement(bitset<K>("1010101010101010"));
  cout << tree.root << endl;

  tree.InsertElement(bitset<K>("1100101010101010"));
  cout << tree.root << endl;

  tree.InsertElement(bitset<K>("1010101010101010"));
  cout << tree.root << endl;

  tree.Compact();
  cout << tree.root << endl;

  tree.InsertElement(bitset<K>("0101010101010101"));
  cout << tree.root << endl;
 
  tree.Compact();
  cout << tree.root << endl;

/*
  tree.InsertElement(bitset<K>("1010101010000011"));
  cout << tree.root << endl;

  tree.InsertElement(bitset<K>("1010101010101011"));

  cout << tree.root << endl;
  tree.Compact();
  cout << tree.root << endl;
  REQUIRE(tree.root.elems.nr_elems() == 4);*/

}
