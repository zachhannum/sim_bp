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
extern int * bimodal_table;

//Gshare variables
extern int ig; //number of PC bits used to index the ghare table
extern int h; //number of index bits used to index the GHR table
extern int * gshare_table;
 char * ghr;
extern int gshare_index_dec;

void init_gshare(){
  //create bimodal predictor table
  int table_size = pow(2, ig);
  gshare_table = calloc(table_size, sizeof(int));
  ghr = (char *) calloc(h, sizeof(char));
  for(int i = 0; i<h; i++){
    ghr[i] = '0';
  }
  //initialize predictions for all entries to 2 (weak taken)
  for( int i =0; i < table_size; i++){
    gshare_table[i] = 2;
  }
}

int process_gshare_table(){
  int length = strlen(currentAddressBinary);
  char index_bin[ig];
  char h_index[h];
  char remainder_index[ig - h];
  int h_index_dec;
  int ghr_dec;
  int j = 0;

  //get h index from PC address
  for(int i = length - 2 - ig; i < (length - 2 - ig + h); i ++){
    h_index[j++] = currentAddressBinary[i];
  }
  h_index[j] = '\0';
  j = 0;
  //get ig - h index from PC address
  for(int i = length - 2 - ig + h; i < (length - 2); i ++){
    remainder_index[j++] = currentAddressBinary[i];
  }
  remainder_index[j] = '\0';

  //XOR h index with ghr
  ghr_dec = strtoll(ghr, NULL, 2);
  h_index_dec = strtoll(h_index, NULL, 2);

  h_index_dec = h_index_dec ^ ghr_dec;
  //convert XOR'ed value back to binary array
  unsigned long long int mask = 1U << (h-1);
  for(int i = 0; i < h; i++){
    /* assert: count <= sizeof(int)*CHAR_BIT */
    h_index[i] = (h_index_dec & mask) ? '1' : '0';
    h_index_dec <<= 1;
  }
  strcpy(index_bin, h_index);
  strcat(index_bin, remainder_index);

  gshare_index_dec = strtoll(index_bin, NULL, 2);

  //shift ghr to right
  for(int i = h - 1; i > 0; i--){
    ghr[i] = ghr[i - 1];
  }

  //update ghr
  switch (branch_outcome){
    case taken:
      ghr[0] = '1';
      break;
    case not_taken:
      ghr[0] = '0';
      break;
  }

  if(gshare_table[gshare_index_dec] < 2){
    return not_taken;
  }
  else{
    return taken;
  }
}

void update_gshare_table(){

  //increment misprediction_counter if need be
  switch (branch_outcome){
    case taken:
      if(gshare_table[gshare_index_dec] < 2)
        misprediction_counter++;
      break;
    case not_taken:
      if(gshare_table[gshare_index_dec] >= 2)
        misprediction_counter++;
      break;
  }

  switch (branch_outcome){
    case taken:
      if(gshare_table[gshare_index_dec] < 3){
        gshare_table[gshare_index_dec]++;
      }
      break;
    case not_taken:
      if(gshare_table[gshare_index_dec] > 0){
        gshare_table[gshare_index_dec]--;
      }
      break;
  }

}

void print_gshare_table(){


  fprintf(out, "\nFinal GShare Table Contents:\n");
  for(int i = 0; i < pow(2, ig); i++){

    fprintf(out, "table[%d]: %d\n", i, gshare_table[i]);
  }

  fprintf(out, "\nFinal GHR Contents: 0x       %X\n" , (unsigned int) strtoll(ghr , NULL, 2));

}
