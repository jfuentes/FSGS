#include "common.h"
#include "blocks_vector.h"
#include <map>
#include <memory>
#include <set>
#include <iomanip>

template <typename NodeType>
using IdxsNodeContainer = map<NodeType *, IdxsContainer>;


template <typename StorageType, k_size_t B, k_size_t K> struct BlockRadixTreeNode {

  const k_size_t offset;
  const bool is_last_node;

  BlocksVector<StorageType, B> elems;

  // Cleared on every cool to FindType
  IdxsContainer aux_elem_idxs;

  vector< unique_ptr<BlockRadixTreeNode<StorageType, B, K> > > children;

  BlockRadixTreeNode<StorageType, B, K> * parent;

  BlockRadixTreeNode():BlockRadixTreeNode(nullptr, K - B) {};

  BlockRadixTreeNode(BlockRadixTreeNode<StorageType, B, K> * _parent, const k_size_t _offset): 
      parent(_parent), offset(_offset), is_last_node(_offset == 0){
  };

  template <bool stop_first, FindType find_type> bool const FindElemsInTree(
      const Query<K> & q, IdxsNodeContainer<BlockRadixTreeNode<StorageType, B, K> > * matches_container) {

    aux_elem_idxs.clear();

#ifdef DEBUG
    cout << "FindElemsInTree, is_last_node: " << is_last_node << " query: " << q << endl;
#endif

    auto matches = (is_last_node)? ( (matches_container == nullptr)? nullptr : &( (*matches_container)[this])) : &aux_elem_idxs;
    const bool found = elems.template FindElems<stop_first, find_type>(q, offset,  matches);

#ifdef DEBUG
    cout << "FindElemsInTree, found:" << found << endl;
#endif

    if (found && !is_last_node) {
     bool any_found = false;
     for(auto vect_idx : aux_elem_idxs) {
        any_found |= children[vect_idx]->template FindElemsInTree<stop_first, find_type>(q, matches_container);
      }
      return any_found;
    }
    return found;
  }

  void InsertElement(Query<K> & q) {
    const bool subq_exists = elems.template FindElems<true, FindType::equal, K>(q, offset, &aux_elem_idxs);

    if (subq_exists) {
#ifdef DEBUG
      cout << "InsertElement, subq_exists, aux_elem_idxs.back():" << aux_elem_idxs.back() << endl;
#endif
      if(!is_last_node) {
        children[aux_elem_idxs.back()]->InsertElement(q);
      }
    } else {
      // Subquery does not exists
      auto q_st = q.template AsStorageType<StorageType, B, false>(offset);
      elems.InsertElem(q_st);
      cout << "InsertElement, !subq_exists, q_st: " << bitset<K>(q_st) << endl;
      if(!is_last_node) {
        cout << "New node"<< endl;
        // We have not reached leafs yet
        unique_ptr<BlockRadixTreeNode <StorageType, B, K> > new_node(new BlockRadixTreeNode(this, offset - B));
        new_node->InsertElement(q);
        children.push_back(move(new_node));
      }
    }
  }

  void RemoveEmptyChildren() {
    aux_elem_idxs.clear();

    size_t idx = 0;
    for (auto & child : children) {
      if (child->elems.is_empty()) {
        cout << "RemoveEmptyChildren: " << *child << " \t idx: " << idx << endl;
        aux_elem_idxs.push_back(idx++);
      }
    }
    DeleteElems(aux_elem_idxs);
  }

  void DeleteElems(IdxsContainer & idxs_elems_to_rm) {
    elems.DeleteElems(idxs_elems_to_rm);

    // Remove children as well if it is not a leaf node
    if (children.size() > 0) {
      assert(children.size() >= idxs_elems_to_rm.size());

      // Assuming that indexes are comming sorted in decreasing order from elems.DeleteElems
      auto rit_idx = idxs_elems_to_rm.rbegin();
      while(rit_idx != idxs_elems_to_rm.rend()) {
        // TODO: This is extremely inneficient, fix
        cout << "Removing children at index: " << *rit_idx << endl;
        children.erase(children.begin() + *rit_idx);
        ++rit_idx;
      }
    }
    // TODO: Eliminate empty parents
  }

  vector<bitset<K>> ExtractElementsSubtree(vector<string> & ancestors_elems) {
    size_t iter_elems = 0;
    auto tot_elems = elems.nr_elems();

    vector<bitset<K>> ret;

    for(auto b: elems.elem_blocks) {
      for(uint_fast8_t i = 0; i < elems.nr_elems_per_block && iter_elems < tot_elems; ++i, ++iter_elems, b >>= B) {
        ancestors_elems.push_back(bitset<B>(b).to_string());
        if (is_last_node) {
          ret.push_back(bitset<K>(accumulate(ancestors_elems.begin(), ancestors_elems.end(), string(""))));
          ancestors_elems.pop_back();
        } else {
          auto ret_child = children[iter_elems]->ExtractElementsSubtree(ancestors_elems);
          ret.insert( ret.end(), ret_child.begin(), ret_child.end());
        }
      }
    }
    return ret;
  }

  friend ostream& operator<< (ostream & out, const BlockRadixTreeNode & node) {

    size_t iter_elems = 0;
    auto tot_elems = node.elems.nr_elems();

/*
    // TODO: Are there more reliable ways to check if it is a leaf
    const bool last_node = children.size() == 0 && tot_elems > 0;
*/
/*    cout.width(node.offset + B);
    cout.fill(' ');*/
    for( auto b : node.elems.elem_blocks) {
      for(uint_fast8_t i = 0; i < node.elems.nr_elems_per_block && iter_elems < tot_elems; ++i, ++iter_elems, b >>= B) {
        out << iter_elems << ":" <<setw(18) << &node << "\t" << string(K - B - node.offset, ' ') << bitset<B>(b) << endl;
        if (!node.is_last_node)
          out << *(node.children[iter_elems]);
      }
    }
    return out;
  }
};

template <typename StorageType, k_size_t K> struct BlockRadixTree {

  static const k_size_t B = K/2;
  BlockRadixTreeNode<StorageType, B, K> root;
  IdxsNodeContainer<BlockRadixTreeNode<StorageType, B, K> > idxs_to_delete;


  size_t compact_treshold;

  BlockRadixTree(size_t _compact_treshold = 1000): compact_treshold(_compact_treshold){
  }

  void InsertElement(const bitset<K> & q_bitset) {
    auto q = Query<K>(q_bitset);
    bool q_is_contained = root.template FindElemsInTree<true, FindType::superset > (q, nullptr);

    if ( ! q_is_contained) {
      cout << "no contained " << endl;
      // Query is contained, find elements that are contained in new query and register them for deletion on next compact
      root.template FindElemsInTree<false, FindType::subset>(q, & idxs_to_delete);
      root.InsertElement(q);
      RequestCompact();
    } else {
#ifdef __DEBUG
    cout << "contained " << endl;
#endif
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
    for(const auto & node_pair : idxs_to_delete) {
      cout << "node: " << node_pair.first << " \tidxs: ";
      for( const auto & e: node_pair.second) {
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
//      cout << "delete: " << b.first << endl;
      b.first->DeleteElems(b.second);
      if(b.first->elems.is_empty()) {
//        cout << "is empty: "<< b.first << " parent: " << b.first->parent << endl;
        nodes_to_try_delete.insert((b.first->parent));
      }
    }

    // Delete empty nodes

    while(nodes_to_try_delete.size() > 0) {
      for(auto node : nodes_to_try_delete) {
//        cout << "Loop RemoveEmptyChildren, node: " << node << endl;
        node->RemoveEmptyChildren();
        if( node->elems.is_empty() && node != &root) {
          nodes_to_try_delete.insert(node->parent);
        }
        nodes_to_try_delete.erase(node);
      }
    }
  }

  vector<bitset<K>> ExtractElements() {
    vector<string> ancestors_elems;
    return root.ExtractElementsSubtree(ancestors_elems);
  }
};
