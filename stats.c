#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

extern unsigned int blockSize;
extern unsigned int size;
extern unsigned int numberOfSets;
extern unsigned int numberOfWays;
extern struct Block **cache;

extern char currentAddressHex[64];
extern char currentAddressBinary[128];
extern char currentTag[64];
extern char currentIndex[64];

char * trace;
FILE * out;
extern int predictor; // 0 - bimodal, gshare - 0, hybrid - 2, yehpatt - 3
extern int branch_outcome;
extern int branch_counter;
extern int btb_misprediction_counter;
extern int misprediction_counter;
//BTB variables
extern int ibtb; //number of PC bits used to index the BTB
extern int assoc_btb; //associativity of the BTB
extern int btb_exists; //should the BTB be created/used?

//Bimodal variables
extern int ib; //number of PC bits used to index the bimodal table
extern int * bimodal_table;

//Gshare variables
extern int ig; //number of PC bits used to index the ghare table
extern int h; //number of index bits used to index the GHR table
extern int * gshare_table;

//hybrid variables
extern int ic; //number of PC bits used to index the chooser tables

extern char buff[64];

void print_stats(){

  printf("\nFinal Branch Predictor Statistics:\n");
  fprintf(out, "\nFinal Branch Predictor Statistics:\n");
  printf("a. Number of branches: %d\n", branch_counter);
  fprintf(out, "a. Number of branches: %d\n", branch_counter);
  printf("b. Number of predictions from the branch predictor: %d\n", prediction_counter);
  fprintf(out, "b. Number of predictions from the branch predictor: %d\n", prediction_counter);
  printf("c. Number of mispredictions from the branch predictor: %d\n", misprediction_counter);
  fprintf(out, "c. Number of mispredictions from the branch predictor: %d\n", misprediction_counter);
  printf("d. Number of mispredictions from the BTB: %d\n", btb_misprediction_counter);
  fprintf(out, "d. Number of mispredictions from the BTB: %d\n", btb_misprediction_counter);
  printf("e. Misprediction Rate: %.2f percent\n", (((double)misprediction_counter + (double)btb_misprediction_counter) / (double)branch_counter) * 100);
  fprintf(out, "e. Misprediction Rate: %.2f percent\n", (((double)misprediction_counter + (double)btb_misprediction_counter) / (double)branch_counter) * 100);

}
