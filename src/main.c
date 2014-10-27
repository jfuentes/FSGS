#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "H.h"

typedef int RC; //for errors
/**
 * Typical errors:
 * -10 File not found
 **/

/**
 * methods
 **/

//method to find refutations for one determined attribute
RC findRefutations(char *** relation, H *matrixH, int Y, unsigned int numAttributes, unsigned int numTuples){

	int i,j, X;
	//cuadratic search of refutations
	for(i=0; i< numTuples; i++){
		for(j=i+1; j<numTuples; j++){
			
			
			if(strcmp(relation[i][Y], relation[j][Y])!=0){
				//new refutation found
				BIT_ARRAY *refutation = bit_array_create(numAttributes);	
				for(X=numAttributes-1;X>=0;X--){
                    			if(strcmp(relation[i][X],relation[j][X])==0){
                        			bit_array_set_bit(refutation, X);
                    			}
                		}
				printf("refutation found: [%d][%d] ",i,j);
				bit_array_printf(refutation);
				printf("\n");
				
				//call to multi-threading function
				
		
			}
			
			
		}
	
	}

	return 0;
}

RC readDataSet(char *** relation, char nameRelation[],
		unsigned int numAttributes, unsigned int numTuples, char * delimiter) {
	char fileName[100];
	sprintf(fileName, "../datasets/%s", nameRelation);
	char line[numAttributes * 30]; //30 is de maximun size for each field (attribute)
	unsigned int col = 0, row = 0;

	printf("Opening %s", fileName);

	FILE * file = fopen(fileName, "r");

	if (file == NULL) {
		return -10;
	}

	col = 0, row = 0;
	while (row < numTuples && fgets(line, sizeof(line), file)) {
		char * ds = strdup(line);
		relation[row][col] = strtok(ds, delimiter);
		while (col < numAttributes && relation[row][col]) {
			col++;
			relation[row][col] = strtok(NULL, delimiter);
		}
		col = 0;
		row++;
	}

	fclose(file);
	return 0;

}

void printRelation(char ***relation, unsigned int numAttributes,
		unsigned int numTuples) {
	unsigned int row, col;
	printf("Dataset:\n");
	for (row = 0; row < numTuples; row++) {
		for (col = 0; col < numAttributes; col++)
			printf("%s ", relation[row][col]);
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

	if (argc != 4) {
		printf(
				"Error, you must use $./main numAttributes numTuples datasetName");
		return -1;
	}

	numAttributes = atoi(argv[1]);
	numTuples = atoi(argv[2]);
	char *nameRelation = argv[3];
	printf("Dataset: %s", nameRelation);
	printf(" size=%d x %d", numTuples, numAttributes);

	//request enough space for the dataset
	char *** relation = (char ***) malloc(sizeof(char**) * numTuples);
	unsigned int i, j;
	for (i = 0; i < numTuples; i++) {
		relation[i] = (char **) malloc(sizeof(char *) * numAttributes);
		for (j = 0; j < numAttributes; j++)
			relation[i][j] = (char *) malloc(sizeof(char) * 30);
	}

	rc = readDataSet(relation, nameRelation, numAttributes, numTuples,
			delimiter);

	if (rc < 0) {
		printf("Error loading the dataset: %d", rc);
		return -1;
	}

	printRelation(relation, numAttributes, numTuples);
	
	//for each attribute, fin its refutations and build H
	int k=0;
	for(k=numAttributes-1; k>=0 ; k--){
		H * matrixH = createH(numAttributes);
		rc = findRefutations(relation, matrixH, k, numAttributes, numTuples);
	
	
	}
	
	
	return rc;
}


