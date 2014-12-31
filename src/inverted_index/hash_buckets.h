#include <vector>
#include <unordered_set>
#include <algorithm>

template <typename KeyType> struct HashBuckets {

   std::vector<std::unordered_set<KeyType>>   buckets;
   unsigned int numAttributes;

   HashBuckets(unsigned int numAttributes, unsigned int currentAttrPosition):numAttributes(numAttributes){
      //obs: the currentAttPosition may be used for optimization
      for(unsigned i=0; i<numAttributes; i++){
         buckets.push_back(std::unordered_set<KeyType>());
      }
   }


   void insertElement(unsigned int attribute, KeyType elem){
      buckets[attribute].insert(elem);
   }

   //method to get the intersection between two indexed set of values in a specific attribute
   std::vector<KeyType> intersection(unsigned int attribute, const std::unordered_set<KeyType> & otherBucket){
      std::vector<KeyType> v;

      if(buckets[attribute].size() < otherBucket.size()){
         for (auto it=buckets[attribute].begin(); it!=buckets[attribute].end(); ++it)
            if(otherBucket.find(*it)!=otherBucket.end()){
               v.push_back(*it);
            }
      }else{
         for (auto it=otherBucket.begin(); it!=otherBucket.end(); ++it)
            if(buckets[attribute].find(*it) != buckets[attribute].end()){
               v.push_back(*it);
            }
      }
      return v;
   }

   //intersection method based in boolean answer
   bool intersectionBool(unsigned int attribute, const std::unordered_set<KeyType> & otherBucket){
      if(buckets[attribute].size() < otherBucket.size()){
         for (auto it=buckets[attribute].begin(); it!=buckets[attribute].end(); ++it)
            if(otherBucket.find(*it)!=otherBucket.end()){
               return true;
            }
         }else{
            for (auto it=otherBucket.begin(); it!=otherBucket.end(); ++it)
               if(buckets[attribute].find(*it) != buckets[attribute].end()){
                  return true;
               }
            }
            return false;
         }


   unsigned int numberElementsBucket(unsigned int attribute){
      return buckets[attribute].size();
   }

   std::unordered_set<KeyType> getBucket(unsigned int attribute){
      return buckets[attribute];
   }

};
