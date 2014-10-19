#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef int RC; //for errors
/**
* Typical errors:
* -10 File not found
**/

/**
* methods
**/
RC readDataSet( char *** relation, char  nameRelation[], unsigned int numAttributes, unsigned int numTuples, char* delimiter);
void printRelation( char ***relation, unsigned int numAttributes, unsigned int numTuples);


/**
* main method
**/
int main(int argc, char* argv[]){
	int numAttributes, numTuples;
	char delimiter[]=",\n"; //IMPORTANT: set the delimiter from the dataset file
	RC rc;

	if(argc!=4){
		printf("Error, you must use $./main datasetName numAttributes numTuples");
		return -1;
	}

	numAttributes= atoi(argv[1]);
	numTuples=atoi(argv[2]);
	char *nameRelation=argv[3];
	printf("Dataset: %s", nameRelation);
	printf(" size=%d x %d",numTuples, numAttributes);


	//request enough space for the dataset
	char *** relation= (char ***) malloc(sizeof(char**)*numTuples);
	unsigned int i, j;
	for(i=0; i<numTuples; i++){
		relation[i]= (char **) malloc(sizeof(char *)*numAttributes);
		for(j=0; j<numAttributes; j++)
			relation[i][j]= (char *) malloc(sizeof(char)*30);
		}
		rc=readDataSet(relation, nameRelation, numAttributes, numTuples, delimiter);

		if(rc<0){
			printf("Error loading the dataset: %d", rc);
			return -1;
		}

		printRelation(relation, numAttributes, numTuples);
		return 0;
	}

	RC readDataSet(char *** relation, char nameRelation[], unsigned int numAttributes, unsigned int numTuples, char * delimiter){
		char fileName[100];
		sprintf(fileName, "../datasets/%s",nameRelation);
		char line[numAttributes*30]; //30 is de maximun size for each field (attribute)
		unsigned int col=0, row=0;

		printf("Opening %s", fileName);

		FILE * file= fopen(fileName,"r");

		if(file==NULL){
			return -10;
		}

		col=0, row=0;
		while(row<numTuples && fgets(line, sizeof(line), file)){
			char * ds=strdup(line);
			relation[row][col]=  strtok(ds,delimiter);
			while(col<numAttributes && relation[row][col]){
				col++;
				relation[row][col]=strtok(NULL, delimiter);
			}
			col=0;
			row++;
		}

		fclose(file);
		return 0;

	}


	void printRelation(char ***relation, unsigned int numAttributes, unsigned int numTuples){
		unsigned int row, col;
		printf("Dataset:\n");
		for(row=0; row<numTuples; row++){
			for(col=0; col<numAttributes; col++)
				printf("%s ", relation[row][col]);
				printf("\n");
			}
		}
