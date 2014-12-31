#include <vector>
#include <unordered_map>
#include <algorithm>
#include "hash_buckets.h"

typedef uint32_t word_t;

template <typename KeyType> struct IndexAttribute {

   std::unordered_map<KeyType, HashBuckets<KeyType>>  values_map;
   unsigned int numAttributes;
   unsigned int currentAttrPosition;

   IndexAttribute(unsigned int numAttributes, unsigned int currentAttrPosition):numAttributes(numAttributes), currentAttrPosition(currentAttrPosition){

   }


   void insertValue(KeyType attributeValue, unsigned int valueAttrPosition, KeyType value){
      /*
      * if the value for attribute doesn't exist, create it
      * otherwise, insert the value
      *
      */

      auto it=values_map.find(attributeValue);
      if(it!=values_map.end()){
         (it->second).insertElement(valueAttrPosition, value);


      }else{ //the value for attribute doesn't exist

         auto index = HashBuckets<KeyType>(numAttributes, currentAttrPosition);
         index.insertElement(valueAttrPosition, value);
         values_map.insert(std::pair<KeyType, HashBuckets<KeyType>>(attributeValue, index));

      }

   }

   unsigned int getNumberValuesAttr(){
      return values_map.size();
   }

   void getRefutations(std::vector<word_t> & refs){
      refs.clear();

      //get the iterator from map and competare every attribute with the rest

      for (auto it=values_map.begin(); it!=values_map.end(); ++it){
         for(auto it2=std::next(it); it2!=values_map.end(); ++it2){
            word_t refutation = 0;
            word_t mask = 1;
            for (int X = numAttributes - 1; X >= 0; X--) {
               if(it->second.intersectionBool(X, it2->second.getBucket(X))) {
                  mask = 1;
                  mask<<=X;
                  refutation|=mask;
               }
            }
            // if a valid refutation was found, add it
            if(refutation!=0)
               refs.push_back(refutation);
         }
      }

   }



};
