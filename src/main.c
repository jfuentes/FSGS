#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "H.h"

typedef int RC; //for errors
/**
 * Typical errors:
 * -10 File not found
 **/

/**
 * methods
 **/
void * checkRefutationInH( void *args);
RC findRefutations(char *** relation, H *matrixH, int Y, int numAttributes, int numTuples);

//method to find refutations for one determined attribute
RC findRefutations(char *** relation, H *matrixH, int Y, int numAttributes, int numTuples){

	int i,j, X,rc;
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
                		if(bit_array_num_bits_set(refutation)==0)
                			continue;
				printf("refutation found: [%d][%d] ",i,j);
				bit_array_printf(refutation);
				printf("\n");
				
				//call multi-threading function
				argumentsH *args = (argumentsH *) malloc(sizeof(argumentsH));
        			args->matrixH=matrixH;
        			args->from=0;
        			args->to=matrixH->numRefutations;
        			args->refutation=refutation;
				pthread_t thread;
      				pthread_create(&thread, NULL, checkRefutationInH, (void *)args);
      				pthread_join(thread, NULL);
      				//show H
      				printH(matrixH);
      				
				
		
			}
			
			
		}
	
	}

	return 0;
}

void * checkRefutationInH( void *argsThread) {
    	argumentsH *args = (argumentsH *) argsThread;
	unsigned int i;
	
	printf("thread dealing with H\n ");
	if(args->to==0) // there is no refutations to compare with, add it
		addHi(args->matrixH,args->refutation );
	else{
		for(i=args->from; i<args->to; i++){
			if(args->matrixH->existRef[i]==0)
				continue;
			printf("checking...\n ");
			BIT_ARRAY * t=bit_array_create(args->matrixH->numAttributes);
			BIT_ARRAY * result=bit_array_create(args->matrixH->numAttributes);
		
			// t= X and Xi
			bit_array_and(t, args->refutation, args->matrixH->matrix[i]);
		
			//t XOR X
			bit_array_xor(result, t, args->refutation);
		
			if(bit_array_num_bits_set(result)==0){ // if t XOR X = 0
				// X is subset of Xi
				// get away X
				printf("----> X subset of Xi\n ");
				return;
			}else{
				//t XOR X
				bit_array_xor(result, t, args->matrixH->matrix[i]);
				if(bit_array_num_bits_set(result)==0){ // if t XOR Xi = 0
					// Xi is subset of X
					// remove all of Xi which are subset of X
					printf("----> Xi subset of X\n ");
					args->matrixH->existRef[i]=0;
					bit_array_free(args->matrixH->matrix[i]);
				}
		
			}
		
		}
		addHi(args->matrixH,args->refutation );
	}
	printf("\n ");	
	
	//compact array of Hs
	return;

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
		H * matrixH = createH(numAttributes, k);
		rc = findRefutations(relation, matrixH, k, numAttributes, numTuples);
	
	
	}
	
	
	return rc;
}


