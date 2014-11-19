#include <stdlib.h>
#include "H.h"

H* createH(unsigned int numAttributes, unsigned int Y){
  H* matrixH= (H*) malloc(sizeof(H));
  matrixH->matrix= (word_t *) malloc(sizeof(word_t)*INITIAL_SIZE_H);
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

RC addHi(H *h, word_t newRef){
  //first search on deleted position, then on free space
  unsigned int pos;

  if(h->deletedPositions->next!=0){ //there is an available position
    pos=h->deletedPositions->next->position;
    //by pass
    h->deletedPositions->next=h->deletedPositions->next->next;
  }else{
    if(h->firstAvailablePosition==h->size){//it is necessary to increse the size matrix?
      h->matrix=(word_t *) realloc(h->matrix, sizeof(word_t)*h->size*2);
      if(h->matrix==NULL)
        return -11; // out of memory
      h->size=h->size*2; //update the matrix size
      //printf("\nMatrix H was incresed to %d",h->size);
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
  h->matrix[i]= 0x0000;
}

void destroyH(H *h){
	struct node * it;
	while(h->deletedPositions->next!=NULL){
			it=h->deletedPositions;
			h->deletedPositions=h->deletedPositions->next;
			free(it);
	}
	free(h->deletedPositions);
  free(h->matrix);
  free(h);

}

void printH(H *h){
  unsigned int i,j=0;
  printf("\nMatrix H for Y=%d |H|=%d\n",h->Y, h->numRefutations);
  for(i=0; i<h->size && j<h->numRefutations;i++){
    if(h->matrix[i]){
      printbits(h->matrix[i], h->numAttributes);
      printf("\n");
      j++;
    }
  }
}

void printbits(word_t x, unsigned int length) {
      unsigned int i=0;
     do
     { // fill in array from right to left
        printf("%c",(x & 1) ? '1':'0');
        x>>=1;  // shift right 1 bit
        i++;
     } while( i < length);
}

void printbitsToFile(FILE *file, word_t x, unsigned int length){
     unsigned int i=0;
     do
     { // fill in array from right to left
        fprintf(file,"%c",(x & 1) ? '1':'0');
        x>>=1;  // shift right 1 bit
        i++;
     } while( i < length);
     fprintf(file,"\n");
}

void printIntToFile(FILE *file, word_t x){
     fwrite(&x, sizeof(x), 1, file);
     //fprintf(file, "%d\n", x);
}
