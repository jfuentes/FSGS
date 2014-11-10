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


template <class storageType, k_size_t B, k_size_t K> struct BlocksVector {
  static_assert(K >= B, "K, the number of bits in tree, must be larger than B, the number of bits in node.");
  static_assert(K % B == 0, "K must be a multiple of B.");
  static_assert(B <= sizeof(storageType)*8, "Number of bits in storage type must have at least B.");

  const uint_fast8_t nr_elems_per_block = K / B; // Guaranteed to be an integer

  vector<storageType> elem_blocks;
  uint_fast8_t elems_size_residual = 0;

  size_t nr_elems() const {
    return (elems_size_residual == 0)? elem_blocks.size() * nr_elems_per_block: (elem_blocks.size() - 1) * nr_elems_per_block + elems_size_residual;
  }

 void InsertElem(storageType q_st) {
    // TODO: Insert sorted to improve performance (at least ordered blocks)

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

  void DeleteElems(vector<uint_fast8_t> & elems) {
  //  sort(elems.begin(), elems.end());
  }
};

template <class storageType, k_size_t B, k_size_t K> struct FastTrieNode {

  BlocksVector<storageType, B, K> elems;

  void InsertElem(const bitset<K> & q, const k_size_t k_offset) {
    // TODO: Insert sorted to improve performance (at least ordered blocks)

    auto q_st = getQueryStorageType<false>(q, k_offset);
    elems.InsertElem(q_st);
  }

  void deleteElems(vector<uint_fast8_t> & elems) {
  //  sort(elems.begin(), elems.end());
  }

  enum FindType{ subset, superset, equal};

  template <bool stop_first, FindType find_type> bool const FindElems(
      const storageType q_st, BlocksVector<storageType, B, K> * matches) const {

    // calling stop_first and do not provide matches makes no sense
    assert(stop_first ||  matches != nullptr);

    for (const auto & block : elems.elem_blocks) {
      const auto q_and_b = q_st & block;

#ifdef DEBUG
      cout << "Block: " << bitset<K>(block) << " \t q_and_b: " <<bitset<K>(q_and_b) << endl;
#endif

      // Mask with B Least Significant Bits set
      storageType mask_q_one = (1 << (B)) -1;

      for (uint_fast8_t i = 0; i < elems.nr_elems_per_block; ++i) {
        bool found = false;

        switch(find_type) {
          case FindType::superset: found = (q_and_b & mask_q_one) == (q_st & mask_q_one);  break;
          case FindType::subset  : found = (q_and_b & mask_q_one) == (block & mask_q_one); break;
          case FindType::equal   : found = (q_st & mask_q_one)    == (block & mask_q_one); break;
        }
        if (found) {
#ifdef DEBUG
      cout << "Found. find_type: " << find_type << " query: " << bitset<K>(q_st) << " mask:" << bitset<K>(mask_q_one)  << " offset: " << (int)i << endl;
#endif 
         if( matches != nullptr)
            matches->InsertElem(q_st);
          if (stop_first)
            return true;            
        }
        mask_q_one <<= B;
      }
    }

    return false;
  };

  bool const containsSubset(const bitset<K> & q, const k_size_t k_offset) const {
    auto q_st = getQueryStorageType<true>(q, k_offset);
    return FindElems<true, FindType::subset>(q_st, NULL);
  }

  bool const containsSuperset(const bitset<K> & q, const k_size_t k_offset) const {
    auto q_st = getQueryStorageType<true>(q, k_offset);
    return FindElems<true, FindType::superset>(q_st, NULL);
  }

  template <bool fill_storage_type> storageType getQueryStorageType (const bitset<K> & q, const k_size_t k_offset) const {
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

  friend ostream& operator<< (ostream & out, const FastTrieNode & node) {

    size_t iter_elems = 0;
    auto tot_elems = node.elems.nr_elems();

    for( auto b : node.elems.elem_blocks) {
      for(uint_fast8_t i = 0; i < node.elems.nr_elems_per_block && iter_elems < tot_elems; ++i, ++iter_elems) {
        out << iter_elems << ": \t" << bitset<B>(b) << endl;
        b >>= B;
      }
    }

    return out;
  }
};
