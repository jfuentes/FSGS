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
    //K must be larger or equal that k_offset.
     assert(k_offset + B <= K);

    // TODO: Should this flag be an auxiliar member?
    bitset<K> mask_b_bits((1 << B ) - 1);

    // Obtain the B bits that start at position k_offset
    const bitset<K> & subq = (q >> (K - B - k_offset) ) & mask_b_bits;

    // The resulting subq (subquery) will have up to B bits set. StorageType is guaranted to contain at least B bits
    return (StorageType)(sizeof(StorageType) > sizeof(unsigned long)) ? subq.to_ullong(): subq.to_ulong();

/*    StorageType q_st = 0;

    StorageType aux_flags = 1;

    // B LSB set, to wrap around size without requiring division or modulus
    const size_t sign_bits_B = B - 1;
    const auto nr_iters = (fill_storage_type) ? sizeof(StorageType) * 8 : B;
*/
/*    for(uint_fast8_t i = 0; i < nr_iters; ++i, aux_flags <<= 1) {
      if (q[K - 1 - k_offset - (i & sign_bits_B)])
        q_st |= aux_flags;
    }*/

  //  return q_st;
  };

  friend ostream& operator<< (ostream & out, const  Query & q_obj) {
    out << "bitset: " << q_obj.q.to_string() << endl;
    return out;
  }
};
