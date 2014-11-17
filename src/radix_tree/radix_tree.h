#include "common.h"
#include "blocks_vector.h"
#include <map>

template <typename StorageType, k_size_t B, k_size_t K> struct BlockRadixTreeNode {

  BlocksVector<StorageType, B> elems;

  // Cleared on every cool to FindType
  vector<blocks_vector_index_t> aux_elem_idxs;

  vector< BlockRadixTreeNode<StorageType, B, K> *> children;

  BlockRadixTree<StorageType, B, K> * tree;
  BlockRadixTreeNode( BlockRadixTree<StorageType, B, K> * _tree):tree(_tree) {}

  template <bool stop_first, FindType find_type> bool const FindElems(
      const Query<K> & q, vector<blocks_vector_index_t> * matches) {

    Query<K> new_q (q);
    new_q.k_offset += B;
    const bool is_last_node = new_q.k_offset == K;

    aux_elem_idxs.clear();

    const bool found = elems.template FindElems<stop_first, find_type>(q, (is_last_node)? matches: &aux_elem_idxs);

    if (found & !is_last_node) {
      bool any_found = false;
      for(auto vect_idx : aux_elem_idxs){
        any_found |= children[vect_idx]->template FindElems<stop_first, find_type>(new_q, matches);
      }
      return any_found;
    }
    return found;
  }

  bool const containsSubset(const Query<K> & q) const {
    return elems.template FindElems<true, FindType::subset>(q, nullptr);
  }

 bool const containsSuperset(const Query<K> & q) const {
    return FindElems<true, FindType::superset>(q, nullptr);
  }

  bool const findSubsets(const Query<K> & q, vector<blocks_vector_index_t> * matches) const {
    return FindElems<false, FindType::subset>(q, matches);
  }

  bool const findSupersets(const Query<K> & q, vector<blocks_vector_index_t> * matches) const {
    return FindElems<false, FindType::superset>(q, matches);
  }

  void InsertElement(Query<K> & q) {
    aux_elem_idxs.clear();
    const bool subq_exists = elems.template FindElems<true, FindType::equal, K>(q, &aux_elem_idxs);

    Query<K> new_q (q);
    new_q.k_offset += B;
    const bool is_last_node = new_q.k_offset == K;

    if (subq_exists) {
      cout << aux_elem_idxs.back() << endl;
      if(!is_last_node)
        children[aux_elem_idxs.back()]->InsertElement(new_q);
    } else {
      // Subquery does not exists
      auto q_st = q.template AsStorageType<StorageType, B, false>();
      elems.InsertElem(q_st);

      if(!is_last_node) {
        // We have not reached leafs yet
        auto new_node = new BlockRadixTreeNode();
        new_node->InsertElement(new_q);
        children.push_back(new_node);
      } else {
        // Terminal Node, find subsets and set them to remove

/*        auto iter_idxs_to_del = tree.idxs_to_delete.find(this);
        if (iter_idxs_to_del == map::end) {
          iter_idxs_to_del.emplace(v
        }*/
        Query<K> q_from_top(q);
        q_from_top.k_offset = 0;
        tree->root.findSubsets(q, &tree->idxs_to_delete[this]);
      }
    }
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

  map<BlockRadixTreeNode<StorageType, B, K> *, vector<blocks_vector_index_t> > idxs_to_delete;

  size_t compact_treshold;

  BlockRadixTree(size_t _compact_treshold = 1000): compact_treshold(_compact_treshold){
  }

  void InsertElement(const bitset<K> & q_bitset) {
    auto q = Query<K>(q_bitset, 0);
    root.InsertElement(q);
    Compact();
  }
  inline void Compact() { Compact(compact_treshold);}

  inline void Compact(size_t cur_compact_treshold) {
#ifdef DEBUG
    cout << "indexes to delete" << endl;
    for(auto e : idxs_to_delete)
      cout << e << endl;
    cout << "end " << endl;
#endif
    if ( idxs_to_delete.size() > cur_compact_treshold) {
      DeleteElems();
      idxs_to_delete.clear();
    }
  }
  void DeleteElems() {
    for(auto b: idxs_to_delete) {
      b.DeleteElems(idxs_to_delete[*b]);
    }
  }
};
