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

//yeh patt variables
extern int h_yp; //number of PC address bits used to index history table
extern int p; // number of PC address bits used to index the patter table
int hist_index_dec;
int pred_index_dec;
int * history_table;
int * prediction_table;
int hist_table_size;
int pred_table_size;


void init_yehpatt(){
  //create history and prediction tables
  hist_table_size = pow(2, h_yp);
  pred_table_size = pow(2, p);
  history_table = calloc(hist_table_size, sizeof(int));
  prediction_table = calloc(pred_table_size, sizeof(int));

  //initialize predictions for all entries to 2 (weak taken)
  for( int i =0; i < pred_table_size; i++){
    prediction_table[i] = 2;
  }
}

int process_yehpatt_table(){
  int length = strlen(currentAddressBinary);
  char index_bin[h_yp];

  //get history table index
  int j = 0;
  for(int i = length - 2 - h_yp; i < length - 2; i ++){
    index_bin[j++] = currentAddressBinary[i];
  }
  hist_index_dec = strtol(index_bin, NULL, 2);

  //get prediction table index
  pred_index_dec = history_table[hist_index_dec];

  //return prediction
  if(prediction_table[pred_index_dec] < 2){
    return not_taken;
  }
  else {
    return taken;
  }

}

void update_yehpatt_table(){

  //update history register
  history_table[hist_index_dec] = history_table[hist_index_dec] >> 1;
  if(branch_outcome == taken){
    history_table[hist_index_dec] = history_table[hist_index_dec] | (int) pow(2, p - 1);
  }

  //increment misprediction_counter if need be
  switch (branch_outcome){
    case taken:
      if(prediction_table[pred_index_dec] < 2)
        misprediction_counter++;
      break;
    case not_taken:
      if(prediction_table[pred_index_dec] >= 2)
        misprediction_counter++;
      break;
  }

  //update prediction table
  switch (branch_outcome){
    case taken:
      if(prediction_table[pred_index_dec] < 3)
        prediction_table[pred_index_dec]++;
      break;
    case not_taken:
      if(prediction_table[pred_index_dec] > 0)
        prediction_table[pred_index_dec]--;
      break;
  }

}

void print_yehpatt_table(){

  fprintf(out, "\nFinal History Table Contents:\n");
  for(int i = 0; i < pow(2, h_yp); i++){
    fprintf(out, "table[%d]: 0x %x\n", i, history_table[i]);
  }

  fprintf(out, "\nFinal Prediction Table Contents:\n");
  for(int i = 0; i < pow(2, p); i++){
    fprintf(out, "table[%d]: %d\n", i, prediction_table[i]);
  }
}
