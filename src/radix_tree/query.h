#include "common.h"
#include <bitset>


template <k_size_t K> struct Query {

  const bitset<K> q;

  Query(const bitset<K> & _q) :q(_q) {
  }

  template <typename StorageType, k_size_t B, bool fill_storage_type> StorageType AsStorageType (const k_size_t k_offset) const {
    // Recall that in bitset, [0] is the least significant bit, hence, q=0 will get the B least significant bits
    static_assert( sizeof(StorageType) * 8 >= B, "StorageType must have at least B bits.");
    static_assert( (B & (B - 1)) == 0, "B must be power of 2.");
    static_assert( B <= K, "B cannot be larger that K.");
    static_assert( K > 0,  "K must be positive.");
    static_assert( B > 0, "B must be positive.");
    static_assert( K % B == 0, "K must be multiple of B.");

    // K must be larger or equal that k_offset + B to guarantee that all required bits are within the K bits in this bitset.
     assert(k_offset + B <= K);

    // TODO: Should this flag be an auxiliar member?
    bitset<K> mask_b_bits((1 << B ) - 1);

    // Obtain the B bits that start at position k_offset. This is the first B bits more significant that the bit at position k_offset
    const bitset<K> & subq = (q >> k_offset) & mask_b_bits;

    // The resulting subq (subquery) will have up to B bits set. StorageType is guaranted to contain at least B bits
    StorageType q_st = (StorageType)(sizeof(StorageType) > sizeof(unsigned long)) ? subq.to_ullong(): subq.to_ulong();

    if (fill_storage_type) {
      // TODO: This can be optimized further by removing the counter i and having a while loop that breaks when aux reaches 
      // a value that is close to some function of K and B
      const auto nr_iters = K/B;
      StorageType aux = q_st;
      for(uint_fast8_t i = 0; i < nr_iters; ++i, aux <<= B) {
        q_st |= aux;
      }
    }

    return q_st;
  };

  friend ostream& operator<< (ostream & out, const  Query & q_obj) {
    out << "bitset: " << q_obj.q.to_string() << endl;
    return out;
  }
};
