#include "catch.hpp"

#include "../blocks_vector.h"

TEST_CASE( "Blocks Vector", "[blocks_vector]" ) {
  const k_size_t K = 16;
  
  auto bv = BlocksVector<uint16_t, 4, K>();

  REQUIRE( bv.nr_elems() == 0);

  bv.InsertElem(3); // Insert 0011

  REQUIRE( bv.nr_elems() == 1);

  auto q_2 = Query<K>(2, 0);

  // Find subsets of 0010 in {0011}.
  REQUIRE_FALSE( (bv.FindElems<true, decltype(bv)::FindType::subset>(q_2, nullptr) ) );

  // Find supersets of 0010 in {0011}.
  REQUIRE( (bv.FindElems<true, decltype(bv)::FindType::superset>(q_2, nullptr) ) );

  // Find exact match of 0010 in {0011}.
  REQUIRE_FALSE( (bv.FindElems<true, decltype(bv)::FindType::equal>(q_2, nullptr) ) );


  auto q_7 = Query<K>(7,0);
  REQUIRE( (bv.FindElems<true, decltype(bv)::FindType::subset>(q_7, nullptr) ) );

  bv.InsertElem(20); // Insert 10100

  cout << bv << endl;
}
