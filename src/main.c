#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "H.h"


/**
 * Typical errors:
 * -10 File not found
 * -11 out of memory
 **/

/**
 * methods
 **/

RC findRefutations(int ** relation, H *matrixH, int Y, int numAttributes,	int numTuples);
RC checkRefutationInH(H *matrixH, word_t refutation);
void * checkRefutationInHMulti(void *argsThread);
RC findRefutationsMulti(int ** relation, H *matrixH, int Y, int numAttributes, int numTuples);
void printRelation(int **relation, unsigned int numAttributes, unsigned int numTuples);
/**
 * sequential method to find refutations for one determined attribute
 **/
RC findRefutations(int ** relation, H *matrixH, int Y, int numAttributes,	int numTuples) {

	int i, j, X;
	//cuadratic search of refutations
	for (i = 0; i < numTuples; i++) {
		for (j = i + 1; j < numTuples; j++) {

			if (relation[i][Y] != relation[j][Y]) {
				//new refutation found
				word_t refutation = 0x00000000;
				word_t mask = 1;
				for (X = numAttributes - 1; X >= 0; X--) {
					if (Y!=X &&relation[i][X] == relation[j][X]) {
						mask = 1;
						mask<<=X;
						refutation|=mask;
					}
				}
				if (!refutation){
					continue;
        		}
				//printf("refutation found: [%d][%d] ",i,j);
				//printbits(refutation, numAttributes);
				//printf("  %d",refutation);
				//printf("\n");

				checkRefutationInH(matrixH, refutation);

			}

		}

	}

	//show H
	printH(matrixH);

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
			if (!matrixH->matrix[i])
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

		}
		addHi(matrixH, X);
	}

	//Idea: compact array of Hs
	return rc;

}

/**
* PENDIENT
**/
//multi-core method to find refutations for one determined attribute
RC findRefutationsMulti(int ** relation, H *matrixH, int Y, int numAttributes,
		int numTuples) {

	int i, j, X;
	//cuadratic search of refutations
	for (i = 0; i < numTuples; i++) {
		for (j = i + 1; j < numTuples; j++) {

			if (relation[i][Y] != relation[j][Y]) {
				//new refutation found
				word_t refutation = 0x00000000;
				word_t mask = 1;
				for (X = numAttributes - 1; X >= 0; X--) {
					if (Y!=X &&relation[i][X] == relation[j][X]) {
						mask = 1;
						mask<<=X;
						refutation|=mask;
					}
				}
				if (!refutation){
					continue;
				}
				//call multi-threading function
				//PENDIENT..
				// the idea is to split H and define 'from' and 'to' indexes for each thread
				argumentsH *args = (argumentsH *) malloc(sizeof(argumentsH));
				args->matrixH = matrixH;
				args->from = 0;
				args->to = matrixH->numRefutations;
				args->X = refutation;
				pthread_t thread;
				/**
				*PENDIENT
				*/
				pthread_create(&thread, NULL, checkRefutationInHMulti, args);
				pthread_join(thread, NULL);
				//show H
				printH(matrixH);

			}

		}

	}

	return 0;
}

/**
* PENDIENT
**/
//multi-core method to check maximality of a refutation vs H


void *checkRefutationInHMulti(void *argsThread) {
	argumentsH *args = (argumentsH *) argsThread;
	unsigned int i,j=0;
	int rc=0;
	if (args->matrixH->numRefutations == 0){ // there is no refutations to compare with, add it
		if((rc=addHi(args->matrixH, args->X))!=0)
			return NULL;
	}else {
		word_t t, result;
		for (i = 0; i < args->matrixH->size && j<args->matrixH->numRefutations; i++) {
			if (!args->matrixH->matrix[i])
				continue;

			// t= X and Xi
			t=(args->X&args->matrixH->matrix[i]);

			//result= t XOR X
			result= (t^args->X);

			if (!result) { // if t XOR X = 0
				// X is subset of Xi
				// throw away X
				return 0;
			} else {
				//t XOR Xi
				result= (t^args->matrixH->matrix[i]);
				if (!result) { // if t XOR Xi == 0
					// Xi is subset of X
					// remove all of Xi which are subset of X
					removeHi(args->matrixH, i);
				}

			}

		}
		addHi(args->matrixH, args->X);
	}


	pthread_exit(NULL);

}


RC readDataSet(int ** relation, char nameRelation[], unsigned int numAttributes,
		unsigned int numTuples, char * delimiter) {
	char fileName[100];
	sprintf(fileName, "../datasets/%s", nameRelation);
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

/**
 * main method
 **/
int main(int argc, char* argv[]) {
	unsigned int numAttributes, numTuples;
	char delimiter[] = ",\n"; //IMPORTANT: set the delimiter from the dataset file
	RC rc;

	if (argc != 5) {
		printf(
				"Error, you must use $./main numAttributes numTuples typeAlgorithm{seq=0,parall=1} datasetName");
		return -1;
	}

	numAttributes = atoi(argv[1]);
	numTuples = atoi(argv[2]);
	type_algorithm = atoi(argv[3]);
	char *nameRelation = argv[4];

	printf("Dataset: %s", nameRelation);
	printf(" size=%dx%d\n", numTuples, numAttributes);

	//request enough space for the dataset
	int ** relation = (int **) malloc(sizeof(int*) * numTuples);
	unsigned int i;
	for (i = 0; i < numTuples; i++) {
		relation[i] = (int *) malloc(sizeof(int) * numAttributes);
	}

	rc = readDataSet(relation, nameRelation, numAttributes, numTuples,
			delimiter);

	if (rc < 0) {
		printf("Error loading the dataset: %d", rc);
		return -1;
	}

	printRelation(relation, numAttributes, numTuples);

	//for each attribute, fin its refutations and build H
	int k = 0;
	if (type_algorithm == SEQUENTIAL_ALGORITHM) {
		for (k = numAttributes - 1; k >= 0; k--) {
			H * matrixH = createH(numAttributes, k);
			rc = findRefutations(relation, matrixH, k, numAttributes,
					numTuples);
			destroyH(matrixH);
		}
	} else {
//		for (k = numAttributes - 1; k >= 0; k--) {
//			H * matrixH = createH(numAttributes, k);
//			rc = findRefutationsMulti(relation, matrixH, k, numAttributes,numTuples);
//		}
	}

	return rc;
}
