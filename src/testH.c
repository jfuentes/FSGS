#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "H.h"
#include <time.h>

/**
 * Typical errors:
 * -10 File not found
 * -11 out of memory
 **/

/**
 * methods
 **/


RC findRefutations(word_t * refutations, H *matrixH, unsigned int numRefs);
RC checkRefutationInH(H *matrixH, word_t refutation);
RC readDataSet(word_t * refutations, char filename[], unsigned int numRefs);

long timediff(clock_t t1, clock_t t2) {
    long elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
    return elapsed;
}

/**
 * sequential method to find refutations for one determined attribute
 **/
RC findRefutations(word_t * refutations, H *matrixH, unsigned	int numRefs) {
	clock_t start, stop;
	long elapsed;
	
	start=clock();
	unsigned int i=0;


	for(i=0; i<numRefs; i++){
		checkRefutationInH(matrixH, refutations[i]);
	}
	
	//printf("refutation found: [%d][%d] ",i,j);
	//printbits(refutation, numAttributes);
	//printf("  %d",refutation);
	//printf("\n");
	stop=clock();
	elapsed = timediff(start, stop);
    	printf("\nelapsed: %ld ms\n", elapsed);
	
				
	//printH(matrixH);

	return 0;
}



/**
 * senquential method to check maximality of a refutation vs H
 **/
RC checkRefutationInH(H *matrixH, word_t X) {
	RC rc=0;
	unsigned int i, j=0;

	if (matrixH->numRefutations == 0){ // there is no refutations to compare with, add it
		if((rc=addHi(matrixH, X))!=0)
			return rc;
	}else {
		word_t t, result;
		for (i = 0; i < matrixH->size && j<matrixH->numRefutations; i++) {
			if (matrixH->matrix[i]==0)
				continue;

			// t= X and Xi
			t=(X&matrixH->matrix[i]);

			//result= t XOR X
			result= (t^X);

			if (!result) { // if t XOR X = 0
				// X is subset of Xi
				// throw away X
				return 0;
			} else {
				//t XOR Xi
				result= (t^matrixH->matrix[i]);
				if (!result) { // if t XOR Xi == 0
					// Xi is subset of X
					// remove all of Xi which are subset of X
					removeHi(matrixH, i);
				}

			}
			j++;

		}
		addHi(matrixH, X);
	}

	//Idea: compact array of Hs
	return rc;

}



RC readDataSet(word_t * refutations, char filename[], unsigned int numRefs) {

	unsigned int row = 0;
	word_t i;

	printf("\nOpening %s", filename);

	FILE * file = fopen(filename, "r");

	if (file == NULL) {
		return -10;
	}

	 while ((fread(&i, sizeof(word_t), 1, file) == 1) && row<numRefs) {
        	refutations[row]=i;
        	row++;
   	}
	fclose(file);
	return 0;

}


/**
 * main method
 **/
int main(int argc, char* argv[]) {
	unsigned int numAttributes, numRefs, Y;
	RC rc;

	if (argc != 5) {
		printf(	"Error, you must use $./main numAttributes numRefs Y refSet");
		return -1;
	}

	numAttributes = atoi(argv[1]);
	numRefs = atoi(argv[2]);
	Y = atoi(argv[3]);
	char *filename = argv[4];


	//request enough space for the dataset
	word_t * refutations = (word_t *) malloc(sizeof(word_t) * numRefs);

	rc = readDataSet(refutations, filename, numRefs);

	if (rc < 0) {
		printf("Error loading the dataset: %d", rc);
		return -1;
	}

	H * matrixH = createH(numAttributes, Y);
	if((rc = findRefutations(refutations, matrixH, numRefs))!=0){
		return rc;
	}
	destroyH(matrixH);
	

	return rc;
}
