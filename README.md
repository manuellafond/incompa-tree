# incompa-tree

A fast algorithm to construct the incompatibility graph between two trees.  
From the paper 
Manuel Lafond, "Constructing Incompatibility Graphs of Pairs of Trees in Optimal Output-Sensitive Time", to be published somewhere.

# Compiling

On Unix environments, using the command line, go in the main directory and
```
mkdir build
cd build
cmake ..
make
```
Then run ./incomp with desired arguments.

If you like Visual Studio, try to use the solution in the visual_studio_project directory, though this was not tested.

# Usage 

Basic usage is 
```
./incomp -i [my_tree_file] -o [my_output_directory]
```
The arguments -i and -o are mandatory.  my_tree_file must contain one tree per line in newick format, and it must have at least two trees.  

Then for each lines i and j, the program outputs the file [myoutput_directory]/trees_i_j.edgelist which contains the list of edges of the incompatibility graph.
Each edge is a pair of labels of internal nodes.  The program reads the trees and assigns each internal node a unique label.  The trees with these new labels are exported in 
[myoutput_directory]/trees_relabeled.newick


The detailed arguments are as follows. 

> -i [file]   

Input file containing the newick of two or more trees

> -algo [algo]   

Algo is either 'fast' or 'naive'.  Default is the O(n + d) fast algo, naive is O(n^3/w)

> -o [dir]   

Output directory (existing files are overwritten).  Program outputs your trees with added internal node labels, plus the graph as an edge list for each tree pair.

> -s [file]   

If specified, write csv stats for each pair in given file.  Was useful to produce plots for the paper.

> -x [colname1,...,colnamex=val1,...,valx]   

If specified, in the stats file, colnames will be added and values added on each line.  Also for plots.

> -m [incomp|test_incomp|rnd]   

Different modes. 

"-m incomp" is the default mode, it computes the graph(s).  

"-m test_incomp" is for unit tests.  It tests three implementations to make sure the graphs are identical 

"-m rnd" generates random trees in output file.  In that case, add arguments -t [nb trees] -n [nb leaves], and if -x is set it adds an extra leaf under the root.


# Reproducing experiments

The experiments directory has all the scripts used to generate the data from the paper.  Check the readme therein.