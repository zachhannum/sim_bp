

typedef int bool;
enum { false, true };

unsigned int blockSize;
unsigned int size;
unsigned int numberOfSets;
unsigned int numberOfWays;
char currentTag[64];
char currentIndex[64];

char currentAddressHex[64];
char currentAddressBinary[128];

long long int decimalTag;

struct Block {
  unsigned long long int tag;
  unsigned int LRURank;
  bool isFull;
};

struct Block **cache;


int branch_outcome;
char buff[64];
int predictor; // 0 - bimodal, gshare - 0, hybrid - 2, yehpatt - 3
int branch_counter;
int prediction_counter;
int misprediction_counter;
int btb_misprediction_counter;

//BTB variables
int ibtb; //number of PC bits used to index the BTB
int assoc_btb; //associativity of the BTB
int btb_exists; //should the BTB be created/used?
unsigned long long int btb_tag;
int btb_index;
int way_index;

//Bimodal variables
int ib; //number of PC bits used to index the bimodal table
char bimodal_index_bin[64];
int bimodal_index;
int * bimodal_table;

//Gshare variables
int ig; //number of PC bits used to index the ghare table
int h; //number of index bits used to index the GHR table
int * gshare_table;
int gshare_index_dec;

//yeh patt variables
int h_yp; //number of PC address bits used to index history table
int p; // number of PC address bits used to index the patter table

//hybrid variables
int ic; //number of PC bits used to index the chooser tables


void hex_to_bin();
void add_zeros();
void rank_LRU_hit();
void rank_LRU_miss_cold();
void rank_LRU_miss_replace();


void init_bimodal();
void get_PC_address();
void print_bimodal_table();
int process_bimodal_table();
void update_bimodal_table();

void init_gshare();
int process_gshare_table();
void update_gshare_table();
void print_gshare_table();

void init_hybrid();
void process_hybrid_table();
void print_hybrid_table();

void init_yehpatt();
int process_yehpatt_table();
void update_yehpatt_table();
void print_yehpatt_table();

void init_btb();
int process_btb();
void print_btb();

void print_stats();



#define bimodal 0
#define gshare  1
#define hybrid  2
#define yehpatt 3
#define taken   10
#define not_taken 11
#define hit 20
#define miss 21
