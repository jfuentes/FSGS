#include <stdlib.h>
#include "H.h"

H* createH(unsigned int numAttributes, unsigned int Y){
	H* matrixH= (H*) malloc(sizeof(H));
	matrixH->matrix= (struct BIT_ARRAY **) malloc(sizeof(struct BIT_ARRAY)*INITIAL_SIZE_H);
	matrixH->numAttributes=numAttributes;
	matrixH->size=INITIAL_SIZE_H ;
	matrixH->numRefutations=0;
	matrixH->Y=Y;
	matrixH->deletedPositions= (struct node *) malloc(sizeof(struct node));
	matrixH->deletedPositions->position=-1; //header
	matrixH->deletedPositions->next=0;
	matrixH->firstAvailablePosition=0;
	return matrixH;
}

RC addHi(H *h, BIT_ARRAY * newRef){
	//first search on deleted position, then on free space
	unsigned int pos;
	
	if(h->deletedPositions->next!=0){ //there is an available position
		pos=h->deletedPositions->next->position;
		//by pass
		h->deletedPositions->next=h->deletedPositions->next->next;
	}else{
		if(h->firstAvailablePosition==h->size){//it is necessary to increse the size matrix?
			h->matrix=(BIT_ARRAY **) realloc(h->matrix, sizeof(BIT_ARRAY)*h->size*2);
			if(h->matrix==NULL)
				return -11; // out of memory
			h->size=h->size*2; //update the matrix size
            printf("\nMatrix H was incresed to %d",h->size);
		}
		pos=h->firstAvailablePosition;
		h->firstAvailablePosition++;
	}
	
	h->matrix[pos]=newRef; //add the new refutation
		
	h->numRefutations++;
	
	return 0;
}

void removeHi(H *h, unsigned int i){
	h->numRefutations--;
	//add deleted position to the list
	
	struct node *newNode=(struct node *) malloc(sizeof(struct node));
	newNode->position=i;
	newNode->next=h->deletedPositions->next; //add new node
	h->deletedPositions->next=newNode;

	bit_array_free(h->matrix[i]);
}

void destroyH(H *h){
	unsigned int i;
	for(i=0; i<h->numRefutations; i++)
		bit_array_free(h->matrix[i]);
    free(h->deletedPositions);
    free(h->matrix);
	free(h);

}

void printH(H *h){
	unsigned int i;
	printf("\nMatrix H for Y=%d\n",h->Y);
	for(i=0; i<h->numRefutations; i++){
		if(h->matrix[i]!=NULL){
			bit_array_printf(h->matrix[i]);
			printf("\n");
		}
	}
}
