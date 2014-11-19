#include "catch.hpp"

#include "../query.h"

TEST_CASE( "Query", "[query]" ) {
  const k_size_t K = 16;

  // 10 = 00001010b
  auto q = Query<K>(bitset<K>(10));

  REQUIRE( (q.AsStorageType<uint16_t, 16, false>(0)) == 10 );
  // TODO: More test cases with different offsets
  REQUIRE( (q.AsStorageType<uint16_t, 8,  false>(0)) == 10 );
  REQUIRE( (q.AsStorageType<uint16_t, 8,  false>(0)) == 10 );
  REQUIRE( (q.AsStorageType<uint16_t, 4, false>(0)) == 10 );
  REQUIRE( (q.AsStorageType<uint16_t, 2, false>(0)) == 2 );
  REQUIRE( (q.AsStorageType<uint16_t, 1, false>(0)) == 0 );
  REQUIRE( (q.AsStorageType<uint16_t, 1, false>(1)) == 1 );

  REQUIRE( (q.AsStorageType<uint16_t, 4, false>(3)) == 1 );

  REQUIRE( (q.AsStorageType<uint16_t, 2, false>(3)) == 1 );

  REQUIRE( (q.AsStorageType<uint16_t, 4, false>(4)) == 0 );
 
  REQUIRE( (q.AsStorageType<uint16_t, 4, true>(0)) == 0xAAAA);

  REQUIRE( (q.AsStorageType<uint16_t, 4, true>(1)) == 0x5555);
}
