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

//yeh patt variables
extern int h_yp; //number of PC address bits used to index history table
extern int p; // number of PC address bits used to index the patter table

//hybrid variables
extern int ic; //number of PC bits used to index the chooser tables

extern char buff[64];

int main(int argc, char **argv){

  //initialize counters
  branch_counter = 0;
  prediction_counter = 0;
  misprediction_counter = 0;
  btb_misprediction_counter = 0;

  /* Open output file for validation */
  out = fopen("output.txt" , "w");

  /*Get command line arguments*/
  if(!strcmp(argv[1], "bimodal")){
    predictor = bimodal;
    ib = atoi(argv[2]);
    ibtb = atoi(argv[3]);
    assoc_btb = atoi(argv[4]);
    trace = argv[5];
  }
  else if(!strcmp(argv[1], "gshare")){
    predictor = gshare;
    ig = atoi(argv[2]);
    h = atoi(argv[3]);
    ibtb = atoi(argv[4]);
    assoc_btb = atoi(argv[5]);
    trace = argv[6];
  }
  else if(!strcmp(argv[1], "hybrid")){
    predictor = hybrid;
    ic = atoi(argv[2]);
    ig = atoi(argv[3]);
    h = atoi(argv[4]);
    ib = atoi(argv[5]);
    ibtb = atoi(argv[6]);
    assoc_btb = atoi(argv[7]);
    trace = argv[8];
  }
  else if(!strcmp(argv[1], "yehpatt")){
    predictor = yehpatt;
    h_yp = atoi(argv[2]);
    p = atoi(argv[3]);
    ibtb = atoi(argv[4]);
    assoc_btb = atoi(argv[5]);
    trace = argv[6];
  }
  else{
    return 0;
  }

  //print command line to validation output file
  switch (predictor){
    case bimodal:
      fprintf(out, "Command Line:\n %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
      break;
    case gshare:
      fprintf(out, "Command Line:\n %s %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
      break;
    case hybrid:
      fprintf(out, "Command Line:\n %s %s %s %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
      break;
    case yehpatt:
      fprintf(out, "Command Line:\n %s %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
      break;
  }


  //set btb_exists
  if(!ibtb | !assoc_btb){
    btb_exists = 0;
  }
  else{
    btb_exists = 1;
  }

  //initialize predictor values/arrays
  switch (predictor){
    case bimodal:
      init_bimodal();
      break;
    case gshare:
      init_gshare();
      break;
    case hybrid:
      init_hybrid();
      break;
    case yehpatt:
      init_yehpatt();
      break;
  }

  if(btb_exists){
    init_btb();
  }

  /* Open trace file */
  FILE *fp = fopen(trace, "r");


//========== Main Loop ===============
  while(fgets(buff, sizeof buff, fp) != NULL){
    //Clear all arrays for next memory location
    memset(currentAddressBinary, 0, 128);

    //increment branch counter
    branch_counter++;
    //Fill PC address array
    get_PC_address(); // get pc address also obtains the actual branch outcome
    //Convert PC address to binary
    hex_to_bin();

    int btb_return;
    if(btb_exists){
      btb_return = process_btb();
      if(branch_outcome == taken){
        if(btb_return == miss){
          btb_misprediction_counter++;
        }
      }
    }
    else{
      btb_return = hit;
    }


    //if there was a hit in the BTB, run the chosen predictor
    if(btb_return == hit){
      //increment prediction counter
      prediction_counter++;
      switch (predictor){
        case bimodal:
          process_bimodal_table();
          update_bimodal_table();
          break;
        case gshare:
          process_gshare_table();
          update_gshare_table();
          break;
        case hybrid:
          process_hybrid_table();
          break;
        case yehpatt:
          process_yehpatt_table();
          update_yehpatt_table();
          break;
      }
    }

  }
  //========== End Main Loop =============

  //TODO: Print contents of BTB if needed
  if(btb_exists){
    print_btb();
  }

  //Print contents and outputs of predictors
  switch (predictor){
    case bimodal:
      print_bimodal_table();
      break;
    case gshare:
      print_gshare_table();
      break;
    case hybrid:
      print_hybrid_table();
      break;
    case yehpatt:
      print_yehpatt_table();
      break;
  }

  print_stats();

  printf("\nFull output also written to output.txt for validation purposes.\n\n");

  /* End program */
  fclose(fp);
  fclose(out);
  free(cache);

  return 0;

}
