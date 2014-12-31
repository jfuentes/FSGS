#include "catch.hpp"
#include <iostream>

#include "../hash_buckets.h"

TEST_CASE( "HashBuckets", "[hashindex]" ) {

  SECTION("Create index of integers"){
     auto index = HashBuckets<int>(7, 6);
     auto index2 = HashBuckets<int>(7, 6);

     SECTION("Insert elements"){
        index.insertElement(0,0);
        index.insertElement(0,1);
        index.insertElement(0,2);

        REQUIRE(index.numberElementsBucket(0)==3);

        index2.insertElement(0,0);
        index2.insertElement(0,1);
        index2.insertElement(0,3);
        index2.insertElement(0,4);

        REQUIRE(index2.numberElementsBucket(0)==4);

        SECTION("Intersection between indexed attributes"){
          std::vector<int> v =index.intersection(0, index2.getBucket(0));

          REQUIRE(v.size()==2);
       }

     }
  }
}


TEST_CASE( "HashBuckets2", "[hashindex]" ) {

   SECTION("Create index of strings"){
      auto index = HashBuckets<std::string>(7, 6);
      auto index2 = HashBuckets<std::string>(7, 6);

      SECTION("Insert elements"){
         index.insertElement(5,"aa");
         index.insertElement(5,"bb");
         index.insertElement(5,"cc");

         REQUIRE(index.numberElementsBucket(5)==3);

         index2.insertElement(5,"aa");
         index2.insertElement(5,"ff");
         index2.insertElement(5,"bb");
         index2.insertElement(5,"ii");

         REQUIRE(index2.numberElementsBucket(5)==4);



         SECTION("Intersection between indexed attributes"){
            std::vector<std::string> v =index.intersection(5, index2.getBucket(5));

            REQUIRE(v.size()==2);
         }

      }
   }
}
