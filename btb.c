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

extern char buff[64];

extern unsigned long long int btb_tag;
extern int btb_index;
extern int way_index;


void init_btb(){
  //calculate sets and ways
  numberOfWays = assoc_btb;
  numberOfSets = pow(2, ibtb);
  //create btb cache
  cache = calloc(numberOfSets, sizeof(struct Block *));
  for(int i=0; i< numberOfSets; i++){
    cache[i] = calloc(numberOfWays, sizeof(struct Block));
  }

}

int process_btb(){
  int length = strlen(currentAddressBinary);
  char tag_bin[length - ibtb - 2];
  char index_bin[ibtb];
  way_index = 0;

  //get tag from PC address
  for (int i = 0; i < (length - 2 - ibtb); i ++){
    tag_bin[i] = currentAddressBinary[i];
  }
  btb_tag = strtol(tag_bin, NULL, 2);

  //get index from PC address
  int j = 0;
  for(int i = strlen(tag_bin); i < length - 2; i++){
    index_bin[j++] = currentAddressBinary[i];
  }
  btb_index = strtol(index_bin, NULL, 2);

  //check cache at index value for hit
  for(int i = 0; i < numberOfWays; i++){
    if(cache[btb_index][i].isFull){
      if(cache[btb_index][i].tag == btb_tag){
        //hit!
        rank_LRU_hit();
        return hit;
      }
      way_index++;
    }
  }

  //check if miss is cold
  if(way_index < numberOfWays){
    rank_LRU_miss_cold();
    cache[btb_index][way_index].tag = btb_tag;
    cache[btb_index][way_index].isFull = true;
    cache[btb_index][way_index].LRURank = 1;
  }
  else if(way_index == numberOfWays){ //otherwise it's a miss replace
    rank_LRU_miss_replace();
}


return miss;
}

void print_btb(){
  fprintf(out, "\nFinal BTB Tag Array Contents {valid, pc}:");
  for(int i = 0; i < numberOfSets; i++){
    fprintf(out, "\nSet     %d: " , i);
    for(int j=0; j < numberOfWays; j++){
      unsigned long long int pc_address;
      unsigned long long int btb_tag = cache[i][j].tag;
      int btb_index = i;
      pc_address = (btb_tag << ibtb) | btb_index;
      pc_address = pc_address << 2;
      fprintf(out, "  {1, 0x  %llx}" , pc_address);

    }
  }
  fprintf(out, "\n\n");

}

void rank_LRU_hit(){
  for(int i=0; i<numberOfWays;i++){
    if(cache[btb_index][i].isFull && (cache[btb_index][i].LRURank < cache[btb_index][way_index].LRURank)){
      cache[btb_index][i].LRURank++;
    }
  }
  cache[btb_index][way_index].LRURank = 1;

}

void rank_LRU_miss_cold(){
  for(int i=0; i<numberOfWays; i++){
    if(cache[btb_index][i].isFull){
      cache[btb_index][i].LRURank++;
    }
  }
}

void rank_LRU_miss_replace(){

  int i = 0;
  while(i < numberOfWays){
    if(cache[btb_index][i].LRURank == numberOfWays){break;}
    i++;
  }
  for(int j=0; j < numberOfWays; j++){
    if(cache[btb_index][j].LRURank != numberOfWays){
      cache[btb_index][j].LRURank++;
    }
  }
  cache[btb_index][i].LRURank = 1;
  cache[btb_index][i].tag = btb_tag;
}
