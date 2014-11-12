#include "common.h"
#include "blocks_vector.h"

template <typename StorageType, k_size_t B, k_size_t K> struct BlockRadixTreeNode {

  BlocksVector<StorageType, B, K> elems;

  void InsertElement(const Query<K> & q, vector<blocks_vector_index_t> & idx_subsets) {

    if ( ! containsSuperset(q) ) {
      elems.template FindElems<false, decltype(elems)::FindType::subset>(q, & idx_subsets);
      auto q_st = q.template AsStorageType<StorageType, B, false>();
      elems.InsertElem(q_st);
    }
  }

  void DeleteElems(vector<blocks_vector_index_t> & idxs_elems_to_rm) {
    elems.DeleteElems(idxs_elems_to_rm);
  }

  bool const containsSubset(const Query<K> & q) const {
    return elems.template FindElems<true, decltype(elems)::FindType::subset>(q, nullptr);
  }

  bool const containsSuperset(const Query<K> & q) const {
    return elems.template FindElems<true, decltype(elems)::FindType::superset>(q, nullptr);
  }

  friend ostream& operator<< (ostream & out, const BlockRadixTreeNode & node) {

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

template <typename StorageType, k_size_t K> struct BlockRadixTree {
  static const k_size_t B = K;

  BlockRadixTreeNode<StorageType, B, K> root;

  vector<blocks_vector_index_t> idx_subsets;

  void InsertElement(const bitset<K> & q_bitset) {
    auto q = Query<K>(q_bitset, 0);
    root.InsertElement(q, idx_subsets);
    Compact(1000);
  }

  inline void Compact(size_t compact_treshold) {
#ifdef DEBUG
    cout << "indexes to delete" << endl;
    for(auto e : idx_subsets)
      cout << e << endl;
    cout << "end " << endl;
#endif
    if ( idx_subsets.size() > compact_treshold) {
      root.DeleteElems(idx_subsets);
      idx_subsets.clear();
    }
  }
};
