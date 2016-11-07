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

//hybrid variables
extern int ic; //number of PC bits used to index the chooser tables
int * chooser_table; //int array to hold chooser table


void init_hybrid(){
  //create bimodal predictor table
  init_bimodal();
  //create gshare predictor table
  init_gshare();
  //create chooser table and initialize
  int table_size = pow(2, ic);
  chooser_table = calloc(table_size, sizeof(int));
  for(int i = 0; i < table_size; i++){
    chooser_table[i] = 1;
  }
}

void process_hybrid_table(){
  int length = strlen(currentAddressBinary);
  char chooser_i_bin[ic];
  int chooser_i;

  int bimodal_result = process_bimodal_table();
  int gshare_result = process_gshare_table();

  /*printf("\nactual result:  %d\n", branch_outcome);
  printf("bimodal result: %d\n" , bimodal_result);
  printf("gshare result: %d\n", gshare_result);*/

  //get chooser index
  int j = 0;
  for(int i = length - 2 - ic; i < length - 2; i ++){
    chooser_i_bin[j++] = currentAddressBinary[i];
  }
  chooser_i = strtoll(chooser_i_bin, NULL, 2);

  if(chooser_table[chooser_i] >= 2){
    update_gshare_table();
  }
  else if(chooser_table[chooser_i] < 2){
    update_bimodal_table();
  }

  //update chooser table
  switch (branch_outcome){
    case taken:
    if((gshare_result == taken) && (bimodal_result != taken)){
      if(chooser_table[chooser_i] < 3){
        chooser_table[chooser_i]++;
      }
    }
    else if ((gshare_result != taken) && (bimodal_result == taken)){
      if(chooser_table[chooser_i] > 0){
        chooser_table[chooser_i]--;
      }
    }
    break;
    case not_taken:
    if((gshare_result == not_taken) && (bimodal_result != not_taken)){
      if(chooser_table[chooser_i] < 3){
        chooser_table[chooser_i]++;
      }
    }
    else if((gshare_result != not_taken) && (bimodal_result == not_taken)){
      if(chooser_table[chooser_i] > 0){
        chooser_table[chooser_i]--;
      }
    }
    break;
  }

}

void print_hybrid_table(){


  fprintf(out, "\nFinal Bimodal Table Contents:\n");
  for(int i = 0; i < pow(2, ib); i++){
    fprintf(out, "table[%d]: %d\n", i, bimodal_table[i]);
  };

  fprintf(out, "\nFinal GShare Table Contents:\n");
  for(int i = 0; i < pow(2, ig); i++){

    fprintf(out, "table[%d]: %d\n", i, gshare_table[i]);
  }

  fprintf(out, "\nFinal GHR Contents: 0x       %X\n" , (unsigned int) strtoll(ghr , NULL, 2));

  //print chooser table
  fprintf(out, "\nFinal Chooser Table Contents:\n");
  for(int i = 0; i < pow(2, ic); i++){
    fprintf(out, "Choice table[%d]: %d\n", i, chooser_table[i]);
  }


}
