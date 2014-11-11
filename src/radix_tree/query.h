#include "common.h"
#include <bitset>


template <k_size_t K> struct Query {

  const bitset<K> q;
  k_size_t k_offset;

  Query(const bitset<K> & _q, const k_size_t _k_offset) :q(_q), k_offset(_k_offset) {
  }

  template <typename StorageType, k_size_t B, bool fill_storage_type> StorageType AsStorageType () const {
    static_assert( (B & (B-1)) == 0, "B must be power of 2.");
    //K must be larger or equal that k_offset.
     assert(k_offset + B <= K);

    // TODO: This can be done more efficiently
    StorageType q_st = 0;

    StorageType aux_flags = 1;

    // B LSB set, to wrap around size without requiring division or modulus
    const size_t sign_bits_B = B - 1;
    const auto nr_iters = (fill_storage_type) ? sizeof(StorageType) * 8 : B;

    for(uint_fast8_t i = 0; i < nr_iters; ++i, aux_flags <<= 1) {
      if (q[k_offset + (i & sign_bits_B)])
        q_st |= aux_flags;
    }

    return q_st;
  };

  friend ostream& operator<< (ostream & out, const  Query & q_obj) {
    out << q_obj.q;
    return out;
  }
};
