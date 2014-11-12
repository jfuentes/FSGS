#include "common.h"
#include "blocks_vector.h"
#include <map>

template <typename StorageType, k_size_t B, k_size_t K> struct BaseBlockRadixTreeNode {

  BlocksVector<StorageType, B, K> elems;

  vector<blocks_vector_index_t> aux_elem_idxs;

  bool const containsSubset(const Query<K> & q) const {
    return elems.template FindElems<true, decltype(elems)::FindType::subset>(q, nullptr);
  }

  bool const containsSuperset(const Query<K> & q) const {
    return elems.template FindElems<true, decltype(elems)::FindType::superset>(q, nullptr);
  }

  bool const findSubsets(const Query<K> & q) const {
    return elems.template FindElems<false, decltype(elems)::FindType::subset>(q, &aux_elem_idxs);
  }

  bool const findSupersets(const Query<K> & q) const {
    return elems.template FindElems<false, decltype(elems)::FindType::superset>(q, &aux_elem_idxs);
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

template <typename StorageType, k_size_t B, k_size_t K> struct BlockRadixTreeNode: BaseBlockRadixTreeNode<StorageType, B, K> {

  vector<BlockRadixTreeNode*> children;

  void InsertElement(Query<K> & q, vector<blocks_vector_index_t> & idx_subsets) {
    aux_idx_subsets.clear();

    auto q_st = q.template AsStorageType<StorageType, B, false>();

    // Go one level lower
    assert(q.k_offset >= B);
    q.k_offset -= B;

    bool found = findSupersets(q);

    if (found) {
      for (idx_children : aux_elem_idxs) {
        children[idx_children]->InsertElement(q);
      }
    } else {
      elems.InsertElem(q_st);
   
      if(q.k_offset == 0) {
        // We've reached leafs
        auto new_node = new BlockRadixTreeLeaf();
      } else {
        auto new_node = new BlockRadixTreeNode();
      }
      new_node.InsertElement(q);
      children.push_back(new_node); 
    }
  }
};

template <typename StorageType, k_size_t B, k_size_t K> struct BlockRadixTreeLeaf: BaseBlockRadixTreeNode<StorageType, B, K> {

  void InsertElement(const Query<K> & q) {
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

  map<BlockRadixTreeLeaf<StorageType, B, K> *, vector<blocks_vector_index_t> > idx_subsets;

  size_t compact_treshold;

  BlockRadixTree(size_t _compact_treshold = 1000): compact_treshold(_compact_treshold){
  }

  void InsertElement(const bitset<K> & q_bitset) {
    auto q = Query<K>(q_bitset, 0);
    root.InsertElement(q, idx_subsets);
    Compact();
  }
  inline void Compact() { Compact(compact_treshold);}

  inline void Compact(size_t cur_compact_treshold) {
#ifdef DEBUG
    cout << "indexes to delete" << endl;
    for(auto e : idx_subsets)
      cout << e << endl;
    cout << "end " << endl;
#endif
    if ( idx_subsets.size() > cur_compact_treshold) {
      root.DeleteElems(idx_subsets);
      idx_subsets.clear();
    }
  }
};
