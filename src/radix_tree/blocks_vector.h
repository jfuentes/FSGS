#include "common.h"
#include "query.h"
#include <vector>
#include <algorithm>

typedef uint_fast16_t blocks_vector_index_t;
typedef vector<blocks_vector_index_t> IdxsContainer;

enum FindType{ subset, superset, equal};

template <typename StorageType, k_size_t B> struct BlocksVector {
  static_assert(B <= sizeof(StorageType) * 8, "Number of bits in storage type must have at least B.");
  static_assert(sizeof(StorageType) * 8 % B == 0, "Number of bits in storage type must be multiple of B.");
  static_assert( (B & (B-1)) == 0, "B must be power of 2.");

  static const uint_fast8_t block_bitsize = sizeof(StorageType) * 8;
  static const uint_fast8_t nr_elems_per_block = block_bitsize / B;

  static_assert( (nr_elems_per_block & (nr_elems_per_block-1)) == 0, "nr_elems_per_block must be power of 2.");

  static const StorageType elem_mask = (1 << B) - 1; // Mask with B LSB set

  static const StorageType offset_mask = (nr_elems_per_block - 1);

  vector<StorageType> elem_blocks;
  uint_fast8_t elems_size_residual = 0;

  size_t is_empty() const {
    return elem_blocks.size() == 0;
  }
  size_t nr_elems() const {
    return (elems_size_residual == 0)? elem_blocks.size() * nr_elems_per_block: (elem_blocks.size() - 1) * nr_elems_per_block + elems_size_residual;
  }

 void InsertElem(StorageType q_st) {
    // TODO: Insert sorted to improve performance (at least ordered blocks)

    if (elems_size_residual > 0) {

      assert(elem_blocks.size() > 0);

      auto & last_block = elem_blocks.back();
      q_st <<= (B * elems_size_residual);
      last_block |= q_st;
    } else {
     elem_blocks.push_back(q_st);
    }
    elems_size_residual = (elems_size_residual + 1) & offset_mask;
  }

  void CopyElemsBetweenBlocks(const uint_fast8_t offset_orig, const uint_fast8_t offset_dest,
                              const StorageType block_orig, StorageType & block_dest) {

#ifdef DEBUG
    cout << "block_orig: " << bitset<block_bitsize>(block_orig);
    cout << " \tblock_dest: " << bitset<block_bitsize>(block_dest) << " \toff_o: " << (int)offset_orig << " off_d: " << (int)offset_dest << endl;
#endif

    // Masks to obtain elems to copy
    const StorageType orig_mask = elem_mask << (offset_orig * B);
    const StorageType dest_mask = elem_mask << (offset_dest * B);

    // Get orig element
    StorageType aux = block_orig & orig_mask;
    aux >>= (offset_orig * B); // Place it leftmost
    aux <<= (offset_dest * B); // Place it on destination

    // Write destination
    block_dest &= ~ dest_mask; // Clean destination
    block_dest |= aux;         // Copy

  }

  template <bool stop_first, FindType find_type, k_size_t K> bool const FindElems(
      const Query<K> & q, IdxsContainer * matches) const {
    // calling stop_first and do not provide matches makes no sense
    assert(stop_first ||  matches != nullptr);

    const StorageType q_st = q.template AsStorageType<StorageType, B, true>();

    size_t idx_cur_elem = 0;
    const auto tot_nr_elems = nr_elems();

    bool any_found = false;
    for (const auto & block : elem_blocks) {
      const auto q_and_b = q_st & block;

#ifdef DEBUG
      cout << "Block: " << bitset<block_bitsize>(block) << " \t q_and_b: " <<bitset<block_bitsize>(q_and_b) << endl;
#endif

      // Mask with B Least Significant Bits set
      StorageType mask_q_one = (1 << B) -1;

      for (uint_fast8_t i = 0; i < nr_elems_per_block && idx_cur_elem < tot_nr_elems; ++i, ++idx_cur_elem) {
        bool found = false;

        switch(find_type) {
          case FindType::superset: found = (q_and_b & mask_q_one) == (q_st & mask_q_one);  break;
          case FindType::subset  : found = (q_and_b & mask_q_one) == (block & mask_q_one); break;
          case FindType::equal   : found = (q_st & mask_q_one)    == (block & mask_q_one); break;
        }
        if (found) {
          any_found |= found;
#ifdef DEBUG
      cout << "Found. find_type: " << find_type << " query: " << bitset<block_bitsize>(q_st) << " mask:" << bitset<block_bitsize>(mask_q_one)  << " offset: " << (int)i << endl;
#endif
          if( matches != nullptr) {
            matches->push_back(idx_cur_elem);
          }
          if (stop_first) {
            return true;
          }
        }
        // This if should be solved by compiler so shift only occurs when the mask will be reused
        mask_q_one <<= (nr_elems_per_block > 1)?B:0;
      }
    }
    return any_found;
  };


  void DeleteElems(IdxsContainer & idxs_elems_to_rm) {
    // TODO: DeleteElems will be much more efficient with a set rather than a vector
    if (idxs_elems_to_rm.size() == 0 || nr_elems() == 0)
      return;

    // Sort indexes elements to remove in ascending order
    sort(idxs_elems_to_rm.begin(), idxs_elems_to_rm.end());
    unique(idxs_elems_to_rm.begin(), idxs_elems_to_rm.end());

    // Copy current elements to auxiliar array
    vector<StorageType> aux_elem_blocks(elem_blocks);

    size_t i_dest = idxs_elems_to_rm.front();
    size_t i_orig = i_dest;

    size_t i_rm = 0;

    const size_t nr_elems_orig = nr_elems();
    const size_t nr_elems_rm = idxs_elems_to_rm.size();

    while (i_orig < nr_elems_orig ) {
#ifdef DEBUG
      cout << "i_orig: " << i_orig << " \tnr_elems_orig: " << nr_elems_orig << endl;
#endif
      // Move i_orig until idx next elem to add
      while (i_rm <  nr_elems_rm && i_orig == idxs_elems_to_rm[i_rm]) {
        ++i_rm;
        ++i_orig;
      }
      if (i_orig < nr_elems_orig) {
#ifdef DEBUG
        cout << "loop_after. orig: " << i_orig << " dest: " << i_dest << " rm: " << i_rm << endl; 
#endif
        CopyElemsBetweenBlocks(i_orig & offset_mask, i_dest & offset_mask,
                               aux_elem_blocks[i_orig / nr_elems_per_block], elem_blocks[i_dest / nr_elems_per_block]);
        ++i_dest;
        ++i_orig;
      }
    }
#ifdef DEBUG
    cout << "elems_size_residual: " << (int)elems_size_residual << " \t i_orig: " << i_orig << " \t i_dest: " << i_dest << endl;
#endif
    
    elems_size_residual = (i_dest) & offset_mask;

    // Delete unused blocks
    assert(i_orig > 0);
    size_t nr_blocks_rm = (i_dest == 0) ? elem_blocks.size(): (i_orig - 1) / nr_elems_per_block - (i_dest - 1) / nr_elems_per_block;

#ifdef DEBUG
    cout << "Deleting " << nr_blocks_rm << " blocks." << endl;
#endif

    assert(nr_blocks_rm <= elem_blocks.size());
    elem_blocks.erase(elem_blocks.end() - nr_blocks_rm, elem_blocks.end());
  }

  friend ostream& operator<< (ostream & out, const BlocksVector & bv) {

    size_t iter_elems = 0;
    auto tot_elems = bv.nr_elems();

    for( auto b : bv.elem_blocks) {
      for(uint_fast8_t i = 0; i < bv.nr_elems_per_block && iter_elems < tot_elems; ++i, ++iter_elems) {
        out << iter_elems << ": \t" << bitset<B>(b) << endl;
        b >>= B;
      }
    }

    return out;
  }
};
