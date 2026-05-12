# incompa-tree experiments

trees_20.newick, ..., trees_1000.newick contain the random trees that were used in the paper.

The simphyout directory contains the simphy gene trees used in the paper.  
The files are names as pop_[p]_n_[n]_1.newick, where p is the population size (i.e., p=6 means pop size 1e6) and n is the number of leaves.

run_exp.py generates csv files for the fast and naive algorithms.  Change the exe_path variable to make sure it points to your executable.  
The work directories are not in the repo because they contain lots of files.  

run_bhv.py generates csv files for the BHV algorithm.  It assumes that bhv/bhv-inc.jar exists. 
The latter needs to be a modified version of the Owen-Provan algorithm from https://github.com/megan-owen/BHVtreespace

This modified version needs to output a file "incidence_time.txt" with a single number, which is the time it took to build the incompatibility graph.
If you need my modified version, contact me.  

Once all that stuff is done, csv_merger.py produces a single csv file with all the data on random trees.

Then, csv_merger_simphy.py does the same but with simphy data.
