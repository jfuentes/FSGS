#include "catch.hpp"

#include "../query.h"

TEST_CASE( "Query", "[query]" ) {
  const k_size_t K = 16;

  // 10 = 1010b
  auto q = Query<K>(bitset<K>(10), 0);

  REQUIRE( (q.AsStorageType<uint16_t, 16, false>()) == 10 );
  REQUIRE( (q.AsStorageType<uint16_t, 8,  false>()) == 10 );
  REQUIRE( (q.AsStorageType<uint16_t, 4, false>()) == 10 );
  REQUIRE( (q.AsStorageType<uint16_t, 2, false>()) == 2 );
  REQUIRE( (q.AsStorageType<uint16_t, 1, false>()) == 0 );
  REQUIRE( (q.AsStorageType<uint16_t, 0, false>()) == 0 );

  q.k_offset = 3;
  REQUIRE( (q.AsStorageType<uint16_t, 4, false>()) == 1 );

  q.k_offset = 3;
  REQUIRE( (q.AsStorageType<uint16_t, 2, false>()) == 1 );

  q.k_offset = 4;
  REQUIRE( (q.AsStorageType<uint16_t, 4, false>()) == 0 );
 
  q.k_offset = 0;
  REQUIRE( (q.AsStorageType<uint16_t, 4, true>()) == 0xAAAA);

  q.k_offset = 1;
  REQUIRE( (q.AsStorageType<uint16_t, 4, true>()) == 0x5555);
}
