# branch predictor

This was a project completed for ECE463, Advanced Microprocessor design to simulate a branch predictor. The simulator takes a trace file
(examples found in /traces/) in which all addresses are branches, and compares the actual result of the branch (taken or not taken) with
the predicted result.

### Command line arguments:
Command line arguments differ depending on which type of predictor is being used.
####bimodal
The bimodal predictor uses a single 2<sup>i</sup>-bit array of 2-bit counters, where i<sub>B</sub> is the number of bits used to index the PC address. All predictors also have an option to utilize a branch target buffer, which stores previously used branches, and only uses the predictor if there is a hit in the buffer.

`./sim_bp  bimodal <i_B> <i_BTB> <assoc_BTB> <tracefile>`

####gshare
The gshare predictor uses the same 2<sup>i</sup>-bit array of 2-bit counters (i<sub>G</sub>), however the index for the predictor table is also determined based on a global branch history register (GHR), an h-bit array holding a history of previous branch outcomes.

`./sim_bp  gshare <i_G> <h> <i_BTB> <assoc_BTB> <tracefile>`

####hybrid
The hybrid predictor instatiates both a bimodal and gshare predictor, and uses a 2<sup>i<sub>C</sub></sup>-bit array of 2-bit counters to compare the performance of the two predictors, and use the best one.

`./sim_bp  hybrid <i_C> <i_G> <h> <i_B> <i_BTB> <assoc_BTB> <tracefile>`

####yeh/patt
The yeah/patt predictor uses a 2<sup>h</sup>>-bit array of integers called the history table and a 2<sup>p</sup>-bit array of 2-bit counters for prediction. The history table acts like the GHR in a gshare predictor, but has a history for every h index. Each value in the history table is used to index the predictor table.

`./sim_bp  yehpatt <h> <p> <i_BTB> <assoc_BTB> <tracefile>`


**Note that for this project, the BTB does not really do anything, as all addresses are branches anyways.*

#### main.c:
Contains main.

#### add_zeros.c
Adds leading zeros to a binary char array.

#### binary_tree.c
Provides functions that travel through a binary tree array, using Eytzinger's method, either starting with the top node, or one of the bottom nodes.

#### cache_process.c
Contains all functions processing the cache instance, hits, misses, replacements, etc.

#### hex_to_bin.c
Converts hex char array to binary char array.

#### parse.c
Parses binary char array into integer tags and indexes.
