#include "radix_tree.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/timer.hpp>

typedef uint64_t file_storage_t;

using namespace std;

void load_data_file(const string & filename, vector<file_storage_t> & vec){

  ifstream file(filename, std::ios::in | std::ios::binary);
  if (!file.good()) {
    cout << "Failed to open." << endl;
  }

  cout << "tell:"<<file.tellg() << endl;
  size_t file_size;
  file.seekg(0, ios::end);
  file_size = file.tellg();
  file.seekg(0, ios::beg);

  vec.reserve(file_size);
  cout << endl << "file size: "<< file_size << endl;
  cout << endl << "nr cases: "<< file_size/4 << endl;

  uint32_t tmp;
  while(file.good()) {
    file.read((char*)&tmp, sizeof(tmp));
    vec.push_back(tmp);
  }
}

int main (void) {
  string filename = "../test_cases/23751refSet9_253x10";
//  string filename = "../test_cases/16171584refSet14_6436x15";
 // string filename = "../test_cases/62110620refSet15_12871x16";

  boost::timer timer;

  vector<file_storage_t> vec;
  load_data_file(filename, vec);

  cout << "Loading: " << timer.elapsed() <<endl;

  const k_size_t K = 16;
  auto tree = BlockRadixTree<file_storage_t, K>();

  cout << "Start insertion" << endl;
  timer.restart();
  for(const auto e : vec) {
    tree.InsertElement(bitset<K>(e));
  }
  tree.Compact(0);
  cout << "Inserting: " << timer.elapsed() << endl;
  cout << "Finished. Rows: " << tree.root.elems.nr_elems() << endl;
//  cout << tree.root << endl;
}
