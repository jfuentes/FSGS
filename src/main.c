#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "H.h"

typedef int RC; //for errors
/**
 * Typical errors:
 * -10 File not found
 **/

/**
 * methods
 **/

RC findRefutations(int ** relation, H *matrixH, int Y, int numAttributes,
		int numTuples);
RC checkRefutationInH(H *matrixH, unsigned int from, unsigned int to,
		BIT_ARRAY * refutation);
void * checkRefutationInHMulti(void *argsThread);
RC findRefutationsMulti(int ** relation, H *matrixH, int Y, int numAttributes,
		int numTuples);
void printRelation(int **relation, unsigned int numAttributes,
		unsigned int numTuples);
/**
 * sequential method to find refutations for one determined attribute
 **/
RC findRefutations(int ** relation, H *matrixH, int Y, int numAttributes,
		int numTuples) {

	int i, j, X;
	//cuadratic search of refutations
	for (i = 0; i < numTuples; i++) {
		for (j = i + 1; j < numTuples; j++) {

			if (relation[i][Y] != relation[j][Y]) {
				//new refutation found
				BIT_ARRAY *refutation = bit_array_create(numAttributes);
				for (X = numAttributes - 1; X >= 0; X--) {
					if (relation[i][X] == relation[j][X]) {
						bit_array_set_bit(refutation, X);
					}
				}
				if (bit_array_num_bits_set(refutation) == 0)
					continue;
				//printf("refutation found: [%d][%d] ",i,j);
				//bit_array_printf(refutation);
				//printf("\n");

				checkRefutationInH(matrixH, 0, matrixH->numRefutations,
						refutation);

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
RC checkRefutationInH(H *matrixH, unsigned int from, unsigned int to,
		BIT_ARRAY * refutation) {

	unsigned int i;

	if (to == 0) // there is no refutations to compare with, add it
		addHi(matrixH, refutation);
	else {
		for (i = from; i < to; i++) {
			if (matrixH->existRef[i] == 0)
				continue;
			BIT_ARRAY * t = bit_array_create(matrixH->numAttributes);
			BIT_ARRAY * result = bit_array_create(matrixH->numAttributes);

			// t= X and Xi
			bit_array_and(t, refutation, matrixH->matrix[i]);

			//t XOR X
			bit_array_xor(result, t, refutation);

			if (bit_array_num_bits_set(result) == 0) { // if t XOR X = 0
				// X is subset of Xi
				// get away X
				return 0;
			} else {
				//t XOR Xi
				bit_array_xor(result, t, matrixH->matrix[i]);
				if (bit_array_num_bits_set(result) == 0) { // if t XOR Xi == 0
					// Xi is subset of X
					// remove all of Xi which are subset of X
					matrixH->existRef[i] = 0;
					matrixH->numRefutations--;
					bit_array_free(matrixH->matrix[i]);
				}

			}

		}
		addHi(matrixH, refutation);
	}

	//Idea: compact array of Hs
	return 0;

}

//multi-core method to find refutations for one determined attribute
RC findRefutationsMulti(int ** relation, H *matrixH, int Y, int numAttributes,
		int numTuples) {

	int i, j, X;
	//cuadratic search of refutations
	for (i = 0; i < numTuples; i++) {
		for (j = i + 1; j < numTuples; j++) {

			if (relation[i][Y] != relation[j][Y]) {
				//new refutation found
				BIT_ARRAY *refutation = bit_array_create(numAttributes);
				for (X = numAttributes - 1; X >= 0; X--) {
					if (relation[i][X] == relation[j][X]) {
						bit_array_set_bit(refutation, X);
					}
				}
				if (bit_array_num_bits_set(refutation) == 0)
					continue;

				//call multi-threading function
				//PENDIENT..
				// the idea is to split H and define 'from' and 'to' indexes for each thread
				argumentsH *args = (argumentsH *) malloc(sizeof(argumentsH));
				args->matrixH = matrixH;
				args->from = 0;
				args->to = matrixH->numRefutations;
				args->refutation = refutation;
				pthread_t thread;
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
 * multi-core method to check maximality of a refutation vs H
 **/
void *checkRefutationInHMulti(void *argsThread) {
	argumentsH *args = (argumentsH *) argsThread;
	unsigned int i;

	if (args->to == 0) // there is no refutations to compare with, add it
		addHi(args->matrixH, args->refutation);
	else {
		for (i = args->from; i < args->to; i++) {
			if (args->matrixH->existRef[i] == 0)
				continue;
			BIT_ARRAY * t = bit_array_create(args->matrixH->numAttributes);
			BIT_ARRAY * result = bit_array_create(args->matrixH->numAttributes);

			// t= X and Xi
			bit_array_and(t, args->refutation, args->matrixH->matrix[i]);

			//t XOR X
			bit_array_xor(result, t, args->refutation);

			if (bit_array_num_bits_set(result) == 0) { // if t XOR X = 0
				// X is subset of Xi
				// get away X
				pthread_exit(NULL);
			} else {
				//t XOR X
				bit_array_xor(result, t, args->matrixH->matrix[i]);
				if (bit_array_num_bits_set(result) == 0) { // if t XOR Xi = 0
					// Xi is subset of X
					// remove all of Xi which are subset of X
					args->matrixH->existRef[i] = 0;
					bit_array_free(args->matrixH->matrix[i]);
				}

			}

		}
		addHi(args->matrixH, args->refutation);
	}

	pthread_exit(NULL);

}

RC readDataSet(int ** relation, char nameRelation[], unsigned int numAttributes,
		unsigned int numTuples, char * delimiter) {
	char fileName[100];
	sprintf(fileName, "../datasets/%s", nameRelation);
	char line[numAttributes * 4];
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
		}
	} else {
		for (k = numAttributes - 1; k >= 0; k--) {
			H * matrixH = createH(numAttributes, k);
			rc = findRefutationsMulti(relation, matrixH, k, numAttributes,
					numTuples);
		}
	}

	return rc;
}
