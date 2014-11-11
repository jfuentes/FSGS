#include <inttypes.h>
#include <stdio.h>

#define INITIAL_SIZE_H 100
#define SEQUENTIAL_ALGORITHM 0
#define MULTICORE_ALGORITHM 1

typedef uint32_t word_t;

typedef struct{
	unsigned int numAttributes;
	unsigned int numRefutations;
	unsigned int size;
	unsigned int Y;
	word_t * matrix;
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
  	word_t X;
} argumentsH;

int type_algorithm;
typedef int RC; //for errors

H* createH(unsigned int numAttributes, unsigned int Y);

RC addHi(H *h, word_t newRef);

void removeHi(H *h, unsigned int i);

void destroyH(H *h);

void printH(H *h);

void printbits(word_t n, unsigned int length);

void printbitsToFile(FILE *file, word_t x, unsigned int length);

void printIntToFile(FILE *file, word_t x);
