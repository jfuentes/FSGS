#include <cstdint>
#include <vector>
#include <bitset>
#include <cassert>
#include <sstream>

#define DEBUG

#ifdef DEBUG
  #include <iostream>
#endif

// K is the number of bits in each vector
// The type utilized to store length of K, maximum K is 2 ^ sizeof(K_SIZE_TYPE)-1
typedef uint8_t k_size_t;

using namespace std;

template <class storageType, k_size_t B, k_size_t K> class FastTrieNode {

  static_assert(K >= B, "K, the number of bits in tree, must be larger than B, the number of bits in node.");
  static_assert(K % B == 0, "K must be a multiple of B.");
  static_assert(B <= sizeof(storageType)*8, "Number of bits in storage type must have at least B.");

  const uint_fast8_t nr_elems_per_block = K / B; // Guaranteed to be an integer

  vector<storageType> elem_blocks;
  uint_fast8_t elems_size_residual = 0;
  
  public:

  size_t nr_elems() const {
    return (size_t) max( (int)elem_blocks.size()-1, 0) * nr_elems_per_block + elems_size_residual;
  } 
  void insertSubElem(const bitset<K> q, const k_size_t k_offset) {
    // TODO: Insert sorted to improve performance (at least ordered blocks)

    auto q_st = getQueryStorageType(q, k_offset, false);
 
    if (elems_size_residual > 0) {

      assert(elem_blocks.size() > 0);

      auto & last_block = elem_blocks.back();
      q_st <<= (B * elems_size_residual);
      last_block |= q_st;
    } else {
     elem_blocks.push_back(q_st);
    }
    elems_size_residual = (elems_size_residual + 1) & (B-1);
  }

  bool const containsSubset(const bitset<K> q, const k_size_t k_offset) const {

    auto q_st = getQueryStorageType(q, k_offset, true);

    for (const auto & block : elem_blocks) {
      const auto q_and_b = q_st & block;
      cout << bitset<K>(q_and_b) << endl;

      storageType mask_q_one = B - 1;

      for (uint_fast8_t i = 0; i < nr_elems_per_block; ++i) {

        if ((q_and_b & mask_q_one) == (q_st & mask_q_one)) {
          return true;
        }
        mask_q_one <<= B;
      }
    }

    return false;
  };

  storageType getQueryStorageType(const bitset<K> q, const k_size_t k_offset, const bool fill_storage_type) const {
    //K must be larger or equal that k_offset.
     assert(k_offset + B <= K);


    // TODO: This can be done more efficiently
    storageType q_st = 0;

    storageType aux_flags = 1;

    // B LSB set, to wrap around size without requiring division or modulus
    const size_t sign_bits_B = B - 1;
    const auto nr_iters = (fill_storage_type) ? sizeof(storageType) * 8 : B;

    for(uint_fast8_t i = 0; i < nr_iters; ++i, aux_flags <<= 1) {
      if (q[k_offset + (i & sign_bits_B)])
        q_st |= aux_flags;
    }

    return q_st;
  };

  friend ostream& operator<< (ostream & out, const FastTrieNode node) {

    size_t iter_elems = 0;
    auto tot_elems = node.nr_elems();

    for( auto b : node.elem_blocks) {
      for(uint_fast8_t i = 0; i < node.nr_elems_per_block && iter_elems < tot_elems; ++i, ++iter_elems) {
        out << iter_elems << ": \t" << bitset<B>(b) << endl;
        b >>= B;
      }
    }

    return out;
  }
};
