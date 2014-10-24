#include <stdlib.h>
#include "H.h"

H* createH(unsigned int numAttributes){
	H* matrixH= (H*) malloc(sizeof(H));
	matrixH->numAttributes=numAttributes;
	matrixH->size=INITIAL_SIZE_H ;
	matrixH->numRefutations=0;
	//matrixH->matrix= (BIT_ARRAY*) malloc(sizeof(BIT_ARRAY)*size);

	return matrixH;
}


void destroyH(H *h){
	unsigned int i;
	for(i=0; i<h->size; i++)
		bit_array_free(h->matrix[i]);
	free(h);

}
