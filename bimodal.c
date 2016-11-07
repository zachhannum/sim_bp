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

extern int * bimodal_table;
extern FILE * out;

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
int index_dec;

//Gshare variables
extern int ig; //number of PC bits used to index the ghare table
extern int h; //number of index bits used to index the GHR table


void init_bimodal(){
  //create bimodal predictor table
  int table_size = pow(2,ib);
  bimodal_table = calloc(table_size, sizeof(int));
  //initialize predictions for all entries to 2 (weak taken)
  for( int i =0; i < table_size; i++){
    bimodal_table[i] = 2;
  }

}

int process_bimodal_table(){
  int length = strlen(currentAddressBinary);
  char index_bin[ib];
  int j = 0;
  for(int i = length - 2 - ib; i < length - 2; i ++){
    index_bin[j++] = currentAddressBinary[i];
  }
  index_dec = strtol(index_bin, NULL, 2);

  if(bimodal_table[index_dec] < 2){
    return not_taken;
  }
  else {
    return taken;
  }

}

void update_bimodal_table(){

  //increment misprediction_counter if need be
  switch (branch_outcome){
    case taken:
      if(bimodal_table[index_dec] < 2)
        misprediction_counter++;
      break;
    case not_taken:
      if(bimodal_table[index_dec] >= 2)
        misprediction_counter++;
      break;
  }

  switch (branch_outcome){
    case taken:
      if(bimodal_table[index_dec] < 3)
        bimodal_table[index_dec]++;
      break;
    case not_taken:
      if(bimodal_table[index_dec] > 0)
        bimodal_table[index_dec]--;
      break;
  }

}

void print_bimodal_table(){

  fprintf(out, "\nFinal Bimodal Table Contents:\n");
  for(int i = 0; i < pow(2, ib); i++){
    fprintf(out, "table[%d]: %d\n", i, bimodal_table[i]);
  }

}
