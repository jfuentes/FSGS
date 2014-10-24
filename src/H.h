#include "util/bit_array.h"

#define INITIAL_SIZE_H 100

typedef struct{
	unsigned int numAttributes;
	unsigned int numRefutations;
	unsigned int size;
	
	BIT_ARRAY * matrix[INITIAL_SIZE_H];


}H;

H* createH(unsigned int numAttributes);

void destroyH(H *h);
