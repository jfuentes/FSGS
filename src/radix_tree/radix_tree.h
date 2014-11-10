#include <cstdint>
#include <vector>
#include <bitset>
#include <cassert>
#include <sstream>
#include <algorithm>
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
  static_assert(B <= sizeof(storageType) * 8, "Number of bits in storage type must have at least B.");
  static_assert(sizeof(storageType) * 8 % B == 0, "Number of bits in storage type must be multiple of B.");
//  static_assert( (B & (B-1)) == 0, "B must be power of 2.");

  static const uint_fast8_t block_bitsize = sizeof(storageType) * 8;
  static const uint_fast8_t nr_elems_per_block = block_bitsize / B;

  static_assert( (nr_elems_per_block & (nr_elems_per_block-1)) == 0, "nr_elems_per_block must be power of 2.");

  static const storageType elem_mask = (1 << B) - 1; // Mask with B LSB set

  static const storageType offset_mask = (nr_elems_per_block - 1);

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
    cout << "elem_mask: " << bitset<block_bitsize>(elem_mask) << endl;
    elems_size_residual = (elems_size_residual + 1) & offset_mask;
  }

  void CopyElemsBetweenBlocks(const uint_fast8_t offset_orig, const uint_fast8_t offset_dest,
                              const storageType block_orig, storageType & block_dest) {

    cout << "block_orig: " << bitset<block_bitsize>(block_orig);
    cout << " \tblock_dest: " << bitset<block_bitsize>(block_dest) << " \toff_o: " << (int)offset_orig << " off_d: " << (int)offset_dest << endl;

    // Masks to obtain elems to copy
    const storageType orig_mask = elem_mask << (offset_orig * B);
    const storageType dest_mask = elem_mask << (offset_dest * B);

    // Get orig element
    storageType aux = block_orig & orig_mask;
    aux >>= (offset_orig * B); // Place it leftmost
    aux <<= (offset_dest * B); // Place it on destination

    // Write destination
    block_dest &= ~ dest_mask; // Clean destination
    block_dest |= aux;         // Copy

  }

  void DeleteElems(vector<size_t> & idxs_elems_to_rm) {
    if (idxs_elems_to_rm.size() == 0)
      return

    // Sort indexes elements to remove in ascending order
    sort(idxs_elems_to_rm.begin(), idxs_elems_to_rm.end());

    // Copy current elements to auxiliar array
    vector<storageType> aux_elem_blocks(elem_blocks);

    size_t i_dest = idxs_elems_to_rm.front();
    size_t i_orig = i_dest;

    size_t i_rm = 0;

    const size_t nr_elems_orig = nr_elems();
    const size_t nr_elems_rm = idxs_elems_to_rm.size();

    cout << bitset<K>(elem_mask) << endl;
    while(i_orig < nr_elems_orig ) {

      cout << "loop " << i_orig << endl;
      // Move i_orig until idx next elem to add
      while(i_rm <  nr_elems_rm && i_orig == idxs_elems_to_rm[i_rm]) {
        ++i_rm;
        ++i_orig;
      }
      if(i_orig < nr_elems_orig) {
        cout << "loop_after. orig: " << i_orig << " dest: " << i_dest << " rm: " << i_rm << endl; 
        CopyElemsBetweenBlocks(i_orig & offset_mask, i_dest & offset_mask,
                               aux_elem_blocks[i_orig / B], elem_blocks[i_dest / B]);
        ++i_dest;
        ++i_orig;
      }
    }

    cout << "elems_size_residual: " << (int)elems_size_residual << " \t i_orig: " << i_orig << " \t i_dest: " << i_dest << endl;

    
    elems_size_residual = (i_dest) & offset_mask;

    // Delete unused blocks

    size_t nr_blocks_rm = (i_orig-1)/B - (i_dest-1) / B;
    elem_blocks.erase(elem_blocks.end() - nr_blocks_rm, elem_blocks.end());
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
      cout << "Block: " << bitset<elems.block_bitsize>(block) << " \t q_and_b: " <<bitset<elems.block_bitsize>(q_and_b) << endl;
#endif

      // Mask with B Least Significant Bits set
      storageType mask_q_one = (1 << B) -1;

      for (uint_fast8_t i = 0; i < elems.nr_elems_per_block; ++i) {
        bool found = false;

        switch(find_type) {
          case FindType::superset: found = (q_and_b & mask_q_one) == (q_st & mask_q_one);  break;
          case FindType::subset  : found = (q_and_b & mask_q_one) == (block & mask_q_one); break;
          case FindType::equal   : found = (q_st & mask_q_one)    == (block & mask_q_one); break;
        }
        if (found) {
#ifdef DEBUG
      cout << "Found. find_type: " << find_type << " query: " << bitset<elems.block_bitsize>(q_st) << " mask:" << bitset<elems.block_bitsize>(mask_q_one)  << " offset: " << (int)i << endl;
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
      cout << bitset<sizeof(storageType)*8>(b) << endl;
      for(uint_fast8_t i = 0; i < node.elems.nr_elems_per_block && iter_elems < tot_elems; ++i, ++iter_elems) {
        out << iter_elems << ": \t" << bitset<B>(b) << endl;
        b >>= B;
      }
    }

    return out;
  }
};
