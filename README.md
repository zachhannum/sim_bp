# branch predictor

This was a project completed for ECE463, Advanced Microprocessor design to simulate a branch predictor. The simulator takes a trace file
(examples found in /traces/) in which all addresses are branches, and compares the actual result of the branch (taken or not taken) with
the predicted result.

### Command line arguments:
Command line arguments differ depending on which type of predictor is being used.
####bimodal
The bimodal predictor uses
`>./sim_bp <i^B^> <L1_SIZE> <L1_ASSOC> <L2SIZE> <L2_ASSOC> <REPL_POLICY> <INCLUSION> <TRACE_FILE>`

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
