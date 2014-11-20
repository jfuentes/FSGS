#include "catch.hpp"
#include "../radix_tree.h"

TEST_CASE( "Radix Tree Node", "[radix_tree_node]" ) {
  const k_size_t K = 16;
  cout << "about to start radix tree node" << endl;  
  auto block_tree = BlockRadixTreeNode<uint16_t, 4, K>(); 
  cout << block_tree << endl;
}

template <k_size_t K>
void CompareBitsetVectors(vector<bitset<K> > expected, vector<bitset<K> > generated){
 sort(expected.begin(), expected.end(), compare_bitsets<K>);
 sort(generated.begin(), generated.end(), compare_bitsets<K>);

 for( unsigned int i = 0 ; i < expected.size(); ++i) {
   REQUIRE(expected[i] == generated[i]);
 }
}

TEST_CASE( "Radix Tree", "[radix_tree]") {
  const k_size_t K = 16;
  const k_size_t B = 16;

  auto tree = BlockRadixTree<uint64_t, B, K>();
  vector<bitset<K> > expected;

  // Add one element
  tree.InsertElement(bitset<K>(10)); // 00001010

  expected.push_back(bitset<K>(10)); // 00001010
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  // Try to add another, but it is already in
  tree.InsertElement(bitset<K>(10)); // 00001010
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  // Try to add another, but it is contained by one inside
  tree.InsertElement(bitset<K>(8)); // 00001000
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  // Try to add another, but it is also contained by one inside
  tree.InsertElement(bitset<K>(2)); // 00000010
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  cout << "RadixTree" << endl;
  cout << tree.root << endl;

  // Add one that is not contained, we have two in total
  tree.InsertElement(bitset<K>(11)); // Insert 00001011
  expected.push_back(bitset<K>(11)); // Insert 00001011
  REQUIRE(tree.root.CountElems() == 2);
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));
  
  tree.InsertElement(bitset<K>(110)); // Insert 01101110
  expected.push_back(bitset<K>(110)); // Insert 01101110
  cout << tree.root << endl;
  REQUIRE(tree.root.CountElems() == 3);
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  tree.InsertElement(bitset<K>(210)); // Insert 11010010
  expected.push_back(bitset<K>(210)); // Insert 11010010
  REQUIRE(tree.root.CountElems() == 4);
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  tree.InsertElement(bitset<K>(15)); // Insert 00001111
  expected.push_back(bitset<K>(15));
  REQUIRE(tree.root.CountElems() == 5);
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  // Add another one that is already contained
  tree.InsertElement(bitset<K>(13)); // Insert 00001101
  REQUIRE(tree.root.CountElems() == 5);
  CompareBitsetVectors<K>(expected, tree.ExtractElements(false));

  cout << tree.root << endl;

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
  REQUIRE(tree.root.CountElems() == 3);

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

  tree.InsertElement(bitset<K>("1111111111111111"));
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
  REQUIRE(tree.root.CountElems() == 8);*/

}
