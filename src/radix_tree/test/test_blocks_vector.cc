#include "catch.hpp"

#include "../blocks_vector.h"

TEST_CASE( "Blocks Vector", "[blocks_vector]" ) {
  const k_size_t K = 16;
  vector<blocks_vector_index_t> idx_elems_to_rm;

  auto bv = BlocksVector<uint32_t, 4>();

  REQUIRE( bv.nr_elems() == 0);


  SECTION( "Delete on an empty blocks vector" ) {
    // Add index of element to try to delete
    idx_elems_to_rm.push_back(1);
    bv.DeleteElems( idx_elems_to_rm);
    REQUIRE(bv.nr_elems() == 0);
  }

  SECTION( "Add elements") {
    // Add one element"
    bv.InsertElem(3); // Insert 0011
    REQUIRE( bv.nr_elems() == 1);

    SECTION( "Delete the only element in vector" ){
      idx_elems_to_rm.push_back(0);
      bv.DeleteElems( idx_elems_to_rm);
      REQUIRE(bv.nr_elems() == 0);
    }
    SECTION( "Test queries with one element.") {
      auto q_2 = Query<K>(2, 0);

      // Find subsets of 0010 in {0011}.
      REQUIRE_FALSE( (bv.FindElems<true, FindType::subset>(q_2, nullptr) ) );

      // Find supersets of 0010 in {0011}.
      REQUIRE( (bv.FindElems<true, FindType::superset>(q_2, nullptr) ) );

      // Find exact match of 0010 in {0011}.
      REQUIRE_FALSE( (bv.FindElems<true, FindType::equal>(q_2, nullptr) ) );
    

      auto q_7 = Query<K>(7,0);
      REQUIRE( (bv.FindElems<true, FindType::subset>(q_7, nullptr) ) );
    }
    SECTION( "Add another") {
      bv.InsertElem(20); // Insert 10100
      REQUIRE(bv.nr_elems() == 2);

      idx_elems_to_rm.push_back(0);
      bv.DeleteElems( idx_elems_to_rm);
      REQUIRE(bv.nr_elems() == 1);
    }
  }
}

