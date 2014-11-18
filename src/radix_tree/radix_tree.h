#include "common.h"
#include "blocks_vector.h"
#include <map>
#include <memory>
#include <set>

template <typename NodeType>
using IdxsNodeContainer = map<NodeType *, IdxsContainer>;


template <typename StorageType, k_size_t B, k_size_t K> struct BlockRadixTreeNode {

  BlocksVector<StorageType, B> elems;

  // Cleared on every cool to FindType
  IdxsContainer aux_elem_idxs;

  vector< unique_ptr<BlockRadixTreeNode<StorageType, B, K> > > children;

  const BlockRadixTreeNode<StorageType, B, K> * const parent;

  BlockRadixTreeNode():parent(nullptr){};
  BlockRadixTreeNode(BlockRadixTreeNode<StorageType, B, K> * _parent): parent(_parent) {
  };

  template <bool stop_first, FindType find_type> bool const FindElemsInTree(
      const Query<K> & q, IdxsNodeContainer<BlockRadixTreeNode<StorageType, B, K> > * matches_container) {

    Query<K> new_q (q);
    new_q.k_offset += B;
    const bool is_last_node = new_q.k_offset == K;

    aux_elem_idxs.clear();

#ifdef DEBUG
    cout << "FindElemsInTree, is_last_node: " << is_last_node << " query: " << q << endl;
#endif

    auto matches = (is_last_node)? ( (matches_container == nullptr)? nullptr : &( (*matches_container)[this])) : &aux_elem_idxs;
    const bool found = elems.template FindElems<stop_first, find_type>(q, matches);

#ifdef DEBUG
    cout << "FindElemsInTree, found:" << found << endl;
#endif

    if (found && !is_last_node) {
      bool any_found = false;
      for(auto vect_idx : aux_elem_idxs){
        any_found |= children[vect_idx]->template FindElemsInTree<stop_first, find_type>(new_q, matches_container);
      }
      return any_found;
    }
    return found;
  }

  void InsertElement(Query<K> & q) {
    aux_elem_idxs.clear();
    const bool subq_exists = elems.template FindElems<true, FindType::equal, K>(q, &aux_elem_idxs);

    Query<K> new_q (q);
    new_q.k_offset += B;
    const bool is_last_node = new_q.k_offset == K;

    if (subq_exists) {
#ifdef DEBUG
      cout << "InsertElement, subq_exists, aux_elem_idxs.back():" << aux_elem_idxs.back() << endl;
#endif
      if(!is_last_node)
        children[aux_elem_idxs.back()]->InsertElement(new_q);
    } else {
      // Subquery does not exists
      auto q_st = q.template AsStorageType<StorageType, B, false>();
      elems.InsertElem(q_st);

      if(!is_last_node) {
         // We have not reached leafs yet

        // TODO: use smart_ptrs
        unique_ptr<BlockRadixTreeNode <StorageType, B, K> > new_node(new BlockRadixTreeNode(this));
        new_node->InsertElement(new_q);
        children.push_back(move(new_node));
      }
    }
  }

  void CleanChildren() {
    aux_elem_idxs.clear();

    size_t idx = 0;
    for (auto & child : children) {
      if (child.elems.size() == 0)
        aux_elem_idxs.push_back(idx++);
    }
    DeleteElems(aux_elem_idxs);
  }

  void DeleteElems(IdxsContainer & idxs_elems_to_rm) {
    elems.DeleteElems(idxs_elems_to_rm);

    // Remove children as well if it is not a leaf node
    if (children.size() > 0) {
      assert(children.size() < idxs_elems_to_rm.size());
      auto rit_idx = idxs_elems_to_rm.rbegin();
      while(rit_idx != idxs_elems_to_rm.rend())
        children.erase(*rit_idx);
    }
    // TODO: Eliminate empty parents
  }

  friend ostream& operator<< (ostream & out, const BlockRadixTreeNode & node) {

    size_t iter_elems = 0;
    auto tot_elems = node.elems.nr_elems();

/*
    // TODO: Are there more reliable ways to check if it is a leafi
    const bool last_node = children.size() == 0 && tot_elems > 0;
*/
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
  IdxsNodeContainer<BlockRadixTreeNode<StorageType, B, K> > idxs_to_delete;


  size_t compact_treshold;

  BlockRadixTree(size_t _compact_treshold = 1000): compact_treshold(_compact_treshold){
  }

  void InsertElement(const bitset<K> & q_bitset) {
    auto q = Query<K>(q_bitset, 0);
    bool q_is_contained = root.template FindElemsInTree<true, FindType::superset > (q, nullptr);

    if ( ! q_is_contained) {
      // Query is contained, find elements that are contained in new query and register them for deletion on next compact
      root.template FindElemsInTree<false, FindType::subset>(q, & idxs_to_delete);
      root.InsertElement(q);
      RequestCompact();
    }
  }
  inline void RequestCompact() {
    if (__builtin_expect(idxs_to_delete.size() > compact_treshold, false)) {
      Compact();
    }
  }

  void Compact() {
#ifdef DEBUG
    cout << "indexes to delete" << endl;
    for(auto node_pair : idxs_to_delete) {
      cout << "node: " << node_pair.first << " \tidxs: ";
      for( auto e: node_pair.second) {
        cout << e << " ,";
      }
      cout << endl;
    }
    cout << "end " << endl;
#endif
    DeleteElems();
    idxs_to_delete.clear();
  }

  void DeleteElems() {
    set <BlockRadixTreeNode<StorageType, B, K> *> nodes_to_try_delete;

    for(auto & b: idxs_to_delete) {
      b.first->DeleteElems(b.second);
      if(b.first->elems.nr_elems() == 0) {
//        nodes_to_try_delete.insert(b.first->parent);
      }
    }

    // Delete empty nodes

 this is woring review and simplify
    while(nodes_to_try_delete.size() > 0) {
      for(auto node : nodes_to_try_delete) {
        node->CleanChildren();
        if( node->elems.nr_elems() == 0)
          nodes_to_try_delete.insert(node);
      }
    }
  }
};
