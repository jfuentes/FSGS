#include "util/bit_array.h"

#define INITIAL_SIZE_H 100


typedef struct{
	unsigned int numAttributes;
	unsigned int numRefutations;
	unsigned int size;
	unsigned int Y;
	BIT_ARRAY * matrix[INITIAL_SIZE_H];

	unsigned int existRef[INITIAL_SIZE_H]; //1 indicates there is a refutation, 0 othercase

}H;

typedef struct {
  	H *matrixH;
  	unsigned int from;
  	unsigned int to;
  	BIT_ARRAY * refutation;
} argumentsH;

H* createH(unsigned int numAttributes, unsigned int Y);

void addHi(H *h, BIT_ARRAY * newRef);

void destroyH(H *h);

void printH(H *h);
