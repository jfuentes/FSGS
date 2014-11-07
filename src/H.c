#include <stdlib.h>
#include "H.h"

H* createH(unsigned int numAttributes, unsigned int Y){
	H* matrixH= (H*) malloc(sizeof(H));
	matrixH->numAttributes=numAttributes;
	matrixH->size=INITIAL_SIZE_H ;
	matrixH->numRefutations=0;
	matrixH->Y=Y;
	//matrixH->matrix= (BIT_ARRAY*) malloc(sizeof(BIT_ARRAY)*size);

	return matrixH;
}

void addHi(H *h, BIT_ARRAY * newRef){
	unsigned int i=0;
	while(i < h->size && h->existRef[i])
		i++;
	
	if(i < h->size){
		h->matrix[i]=newRef;
		h->existRef[i]=1;
	}
	//else add more space in matrix
	
	h->numRefutations++;
	

}
void destroyH(H *h){
	unsigned int i;
	for(i=0; i<h->numRefutations; i++)
		bit_array_free(h->matrix[i]);
	free(h);

}

void printH(H *h){
	unsigned int i;
	printf("\nMatrix H for Y=%d\n",h->Y);
	for(i=0; i<h->numRefutations; i++){
		if(h->existRef[i]){
			bit_array_printf(h->matrix[i]);
			printf("\n");
		}
	}
}
