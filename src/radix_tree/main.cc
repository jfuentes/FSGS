#include "radix_tree.h"
#include <vector>
#include <iostream>
#include <fstream>
//#include <boost/timer.hpp>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <string.h>
#include <sys/time.h>

using namespace std;
#define NUMBER_THREADS 16

typedef uint32_t word_t;
typedef uint32_t file_storage_t;
const k_size_t K = 16;

struct info{
   int from;
   int to;
   int Y;
};

//functions
void findRefutationsThread(struct info * pars) ;
void printbits(word_t x, unsigned int length);





int ** relation;
int numAttributes;
int numTuples;
//current pivot tuple
int current_tuple=0;

mutex mtx;           // mutex for critical section
BlockRadixTree<file_storage_t, K> *tree;

void findRefutations(int Y) {

   int j, from=0, to=numTuples, X;
	//cuadratic search of refutations
	for (j = from; j < to; j++) {
      if(current_tuple!=j){
         if (relation[current_tuple][Y] != relation[j][Y]) {
   				//new refutation found
   				word_t refutation = 0;
   				word_t mask = 1;
   				for (X = numAttributes - 1; X >= 0; X--) {
   					if (Y!=X &&relation[current_tuple][X] == relation[j][X]) {
   						mask = 1;
   						mask<<=X;
   						refutation|=mask;
   					}
   				}
   				if (!refutation){
   					continue;
           		}
   				//printf("\nrefutation found: [%d][%d]  ",current_tuple,j);
   				//printbits(refutation, numAttributes);
   				//printf("  %d",refutation);
   				//printf("\n");


   				//check the refutation on radix tree
               if(!tree[Y].containsSuperset(bitset<K>(refutation))){
                  //mtx.lock();
                  //printf("\ninserting  %d",refutation);
                  tree[Y].InsertElement(bitset<K>(refutation));
                  //mtx.unlock();
               }
   		}
      }
      //tree.Compact(0);
   }

}

void findRefutationsMultiAttribute(int *Y) {
   int j, from=0, to=numTuples, X;
	//cuadratic search of refutations
	for (j = from; j < to; j++) {
      if(current_tuple!=j){
         if (relation[current_tuple][*Y] != relation[j][*Y]) {
   				//new refutation found
   				word_t refutation = 0;
   				word_t mask = 1;
   				for (X = numAttributes - 1; X >= 0; X--) {
   					if (*Y!=X &&relation[current_tuple][X] == relation[j][X]) {
   						mask = 1;
   						mask<<=X;
   						refutation|=mask;
   					}
   				}
   				if (!refutation){
   					continue;
           		}
   				//printf("\nrefutation found: [%d][%d]  ",current_tuple,j);
   				//printbits(refutation, numAttributes);
   				//printf("  %d",refutation);
   				//printf("\n");


   				//check the refutation on radix tree
               if(!tree[*Y].containsSuperset(bitset<K>(refutation))){
                  //mtx.lock();
                  //printf("\ninserting  %d",refutation);
                  tree[*Y].InsertElement(bitset<K>(refutation));
                  //mtx.unlock();
               }
   		}
      }
      //tree.Compact(0);
   }

   tree[*Y].Compact(0);
   cout << "\nFinished. Rows: " << tree[*Y].root.elems.nr_elems() << endl;

}

void findRefutationsMulti(int Y) {
   vector<thread> threads;
   int split= numTuples/NUMBER_THREADS;
	int i;
   int cont=0;

	for(i=0; i<NUMBER_THREADS; i++){
      struct info *pars= (struct info *) malloc(sizeof(struct info));
      pars->from = cont;

      if(i==NUMBER_THREADS-1)
         pars->to=numTuples;
      else
         pars->to= cont + split;

      pars->Y=Y;
      threads.push_back(thread(findRefutationsThread, pars));  //launch the thread;
      cont= pars->to;
   }


   for (auto& th : threads) th.join();

}


void findRefutationsThread(struct info * pars) {
	int j, from=pars->from, to=pars->to, Y=pars->Y, X;
	//cuadratic search of refutations
	for (j = from; j < to; j++) {
      if(current_tuple!=j){
         if (relation[current_tuple][Y] != relation[j][Y]) {
   				//new refutation found
   				word_t refutation = 0;
   				word_t mask = 1;
   				for (X = numAttributes - 1; X >= 0; X--) {
   					if (Y!=X &&relation[current_tuple][X] == relation[j][X]) {
   						mask = 1;
   						mask<<=X;
   						refutation|=mask;
   					}
   				}
   				if (!refutation){
   					continue;
           		}
   				//printf("\nrefutation found: [%d][%d]  ",current_tuple,j);
   				//printbits(refutation, numAttributes);
   				//printf("  %d",refutation);
   				//printf("\n");


   				//check the refutation on radix tree
               if(!tree[Y].containsSuperset(bitset<K>(refutation))){
                  mtx.lock();
                  //printf("\ninserting  %d",refutation);
                  tree[Y].InsertElement(bitset<K>(refutation));
                  mtx.unlock();
               }
   		}
      }
      //tree.Compact(0);
   }

}


int readDataSet(char nameRelation[], char * delimiter) {
	char fileName[100];
	sprintf(fileName, "../../datasets/%s", nameRelation);
	char line[numAttributes * 6];
	unsigned int col = 0, row = 0;

	printf("\nOpening %s", fileName);

	FILE * file = fopen(fileName, "r");

	if (file == NULL) {
		return -10;
	}

	col = 0, row = 0;
	while (row < numTuples && numAttributes > 0
			&& fgets(line, sizeof(line), file)) {
		char * ds = strdup(line);

		relation[row][col++] = atoi(strtok(ds, delimiter));

		while (col < numAttributes) {
			relation[row][col] = (int) atoi(strtok(NULL, delimiter));
			col++;
		}
		col = 0;
		row++;

	}
	fclose(file);
	return 0;

}

void printRelation(int **relation, unsigned int numAttributes,
		unsigned int numTuples) {
	unsigned int row, col;
	printf("\nDataset:\n");
	for (row = 0; row < numTuples; row++) {
		for (col = 0; col < numAttributes; col++)
			printf("%d ", relation[row][col]);
		printf("\n");
	}
}

void printbits(word_t x, unsigned int length) {
      unsigned int i=0;
     do
     { // fill in array from right to left
        printf("%c",(x & 1) ? '1':'0');
        x>>=1;  // shift right 1 bit
        i++;
     } while( i < length);
}



int main (int argc, char* argv[]) {

   //boost::timer timer;
   struct timespec start, finish;
   double elapsed;

   char delimiter[] = ",\n"; //IMPORTANT: set the delimiter from the dataset file
   int rc, typeAlgorithm=0;

   if (argc != 5) {
      printf(
            "Error, you must use $./main numAttributes numTuples typeAlgorithm{seq=0,parall=1} datasetName");
      return -1;
   }

   numAttributes = atoi(argv[1]);
   numTuples = atoi(argv[2]);
   typeAlgorithm = atoi(argv[3]);
   char *nameRelation = argv[4];


   printf("Dataset: %s", nameRelation);
   printf(" size=%dx%d\n", numTuples, numAttributes);

   //request enough space for the dataset
   relation = (int **) malloc(sizeof(int*) * numTuples);
   unsigned int i;
   for (i = 0; i < numTuples; i++) {
      relation[i] = (int *) malloc(sizeof(int) * numAttributes);
   }

   rc = readDataSet(nameRelation, delimiter);

   if (rc < 0) {
      printf("Error loading the dataset: %d", rc);
      return -1;
   }

   tree = (BlockRadixTree<file_storage_t, K> *) malloc(sizeof(BlockRadixTree<file_storage_t, K>)*numAttributes);
   for (int k = numAttributes - 1; k >= 0; k--) {
      tree[k] = BlockRadixTree<file_storage_t, K>();
   }

  clock_gettime(CLOCK_MONOTONIC, &start);

   if(typeAlgorithm==0){
      for (int k = numAttributes - 1; k >= 0; k--) {

         findRefutations(k);
         tree[k].Compact(0);
         cout << "\nFinished. Rows: " << tree[k].root.elems.nr_elems() << endl;

      }
   }else{
      if(typeAlgorithm==1){
         for (int k = numAttributes - 1; k >= 0; k--) {
            findRefutationsMulti(k);
            tree[k].Compact(0);
            cout << "\nFinished. Rows: " << tree[k].root.elems.nr_elems() << endl;

         }
      }else{
      vector<thread> threads;
      for (int k = numAttributes - 1; k >= 0; k--) {
         int * a = (int *) malloc(sizeof(int));
         *a=k;
         threads.push_back(thread(findRefutationsMultiAttribute, a));  //launch the thread;
      }
      for (auto& th : threads) th.join();
   }
}
   clock_gettime(CLOCK_MONOTONIC, &finish);
   elapsed = finish.tv_sec - start.tv_sec;
   elapsed += (finish.tv_nsec - start.tv_nsec)/1000000000.0;
   cout << "Time: " << elapsed << endl;
   //cout << "Time: " << timer.elapsed() << endl;
}
