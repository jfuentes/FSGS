#include "catch.hpp"
#include <iostream>

#include "../index_attribute.h"

TEST_CASE( "IndexAttribute", "[indexattribute]" ) {

  SECTION("Create index on an attribute"){
     auto index = IndexAttribute<int>(7, 6);

     SECTION("Insert values"){
        index.insertValue(1, 0, 1);
        index.insertValue(1, 0, 2);
        index.insertValue(1, 0, 3);

        REQUIRE(index.getNumberValuesAttr()==1);

        index.insertValue(2, 0, 2);
        index.insertValue(2, 0, 4);

        REQUIRE(index.getNumberValuesAttr()==2);

     }
  }
}


TEST_CASE( "Refutations", "[refutations]" ) {

   SECTION("Find refutations"){
      auto index = IndexAttribute<int>(6, 5);

      SECTION("Insert values"){
         index.insertValue(1, 0, 1);
         index.insertValue(1, 0, 3);
         index.insertValue(1, 0, 4);
         index.insertValue(1, 0, 5);
         index.insertValue(1, 1, 3);
         index.insertValue(1, 2, 2);
         index.insertValue(1, 2, 5);
         index.insertValue(1, 3, 1);
         index.insertValue(1, 4, 4);
         index.insertValue(1, 2, 5);
         index.insertValue(1, 0, 2);
         index.insertValue(1, 0, 4);

         index.insertValue(2, 0, 1);
         index.insertValue(2, 1, 2);
         index.insertValue(2, 2, 2);
         index.insertValue(2, 3, 3);
         index.insertValue(2, 4, 1);

         index.insertValue(4, 0, 9);
         index.insertValue(4, 1, 9);

         std::vector<word_t> v;

         index.getRefutations(v);

         REQUIRE(v.size()==1);

      }
   }
}
