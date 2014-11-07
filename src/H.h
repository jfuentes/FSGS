#include "util/bit_array.h"

#define INITIAL_SIZE_H 100
#define SEQUENTIAL_ALGORITHM 0
#define MULTICORE_ALGORITHM 1

typedef struct{
	unsigned int numAttributes;
	unsigned int numRefutations;
	unsigned int size;
	unsigned int Y;
	BIT_ARRAY ** matrix;
	struct node *deletedPositions; //simple list to store deleted position for its reutilization
	unsigned int firstAvailablePosition;
}H;

struct node{
	int position;
	struct node * next;
};

typedef struct {
  	H *matrixH;
  	unsigned int from;
  	unsigned int to;
  	BIT_ARRAY * refutation;
} argumentsH;

int type_algorithm;
typedef int RC; //for errors

H* createH(unsigned int numAttributes, unsigned int Y);

RC addHi(H *h, BIT_ARRAY * newRef);

void removeHi(H *h, unsigned int i);

void destroyH(H *h);

void printH(H *h);
