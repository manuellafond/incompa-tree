#include <set>
#include <iostream>
#include <time.h>
#include <map>
#include <string>
#include <fstream>


#ifdef _WIN32
#include <direct.h>
#define MKDIR(x) _mkdir(x)
#else
#include <sys/stat.h>
#define MKDIR(x) mkdir(x, 0777)
#endif




#include "node.h"
#include "newicklex.h"
#include "treeutil.h"

#include <string>

#include "incompgraph.h"
#include "incompgraph_naive.h"
#include "incompgraph_ultranaive.h"


#include <chrono>

using namespace std::chrono;

using namespace std;





/**
* Returns a string of all leaf labels below v.  Quick helper function, mainly used to output string representations of clades
**/
string get_clade(Node* v) {
	string str = "";
	for (auto it = v->begin(); it != v->end(); ++it) {
		if ((*it)->is_leaf())
			str += (*it)->label + " ";
	}
	return str;
}



void output_graph(AdjList& graph, string filename) {
	std::ofstream out(filename);

	if (!out.is_open()) return;


	for (auto& v_nbrs : graph) {
		Node* v = v_nbrs.first;
		auto& nbrs = v_nbrs.second;

		for (Node* w : nbrs) {
			out << v->label << " " << w->label << "\n";
		}
	}
}




/**
* Loads all trees from a file and checks that incomp graph is identical for all methods (fast, naive, ultranaive)
* Counts the number of inconsistencies and outputs it to std (it should be 0)
**/ 
void exec_test_all_incomp(string treefilename) {

	vector<Node*> trees;
	vector<string> lines = Util::GetFileLines(treefilename);
	for (string line : lines) {
		Node* tree = NewickLex::ParseNewickString(line);
		trees.push_back(tree);
	}

	int cpt_pairs = 0;

	microseconds timenaive(0);
	microseconds timefast(0);
	microseconds timeultranaive(0);

	int bad_stuff = 0;

	for (int i = 0; i < trees.size(); ++i) {

		for (int j = i + 1; j < trees.size(); ++j) {
			Node* tree1 = trees[i];
			Node* tree2 = trees[j];


			if (i % 10 == 0 && i > 0) {
				//std::cout << "Currently at tree i=" << i << "  j=" << j << endl;
			}


			//block for ultranaive computation
			auto t1ultranaive = high_resolution_clock::now();

			IncompGraphUltraNaive igun(tree1, tree2);
			igun.compute_incomp_graph();

			auto t2ultranaive = high_resolution_clock::now();

			timeultranaive += duration_cast<microseconds>(t2ultranaive - t1ultranaive);




			//block for naive computation
			auto t1naive = high_resolution_clock::now();

			IncompGraphNaive ign(tree1, tree2);
			ign.compute_incomp_graph();

			auto t2naive = high_resolution_clock::now();

			timenaive += duration_cast<microseconds>(t2naive - t1naive);



			//block for fast computation
			auto t1fast = high_resolution_clock::now();

			IncompGraph ig;
			ig.compute_incomp_graph(tree1, tree2);

			auto t2fast = high_resolution_clock::now();

			timefast += duration_cast<microseconds>(t2fast - t1fast);

			//sanity checks: ensures that naive and computed graphs are identical
			//We verify that 1) all edges of ig are in ign and igun; (2) the number of edges are the same (implies same edges)
			int nbedges_naive = 0;
			int nbedges_fast = 0;
			int nbedges_ultranaive = 0;

			for (auto& v_nbrs : igun.graph) {
				Node* v = v_nbrs.first;
				auto& nbrs = v_nbrs.second;

				nbedges_ultranaive += nbrs.size();
			}

			for (auto& v_nbrs : ign.graph) {
				Node* v = v_nbrs.first;
				auto& nbrs = v_nbrs.second;

				nbedges_naive += nbrs.size();
			}


			for (auto& v_nbrs : ig.graph) {
				Node* v = v_nbrs.first;
				auto& nbrs = v_nbrs.second;

				nbedges_fast += nbrs.size();

				for (Node* nbr : nbrs) {
					if (!ign.graph[v].count(nbr)) {
						std::cout << "Bad neighbor 1 at i=" << i << " j=" << j << endl;
						bad_stuff++;
						//int t; cin >> t;
					}

					if (!igun.graph[v].count(nbr)) {
						std::cout << "Bad neighbor 2 at i=" << i << " j=" << j << endl;
						bad_stuff++;
						//int t; cin >> t;
					}
				}
			}

			if (nbedges_naive != nbedges_fast) {
				std::cout << "Nb edges is not the same 1!" << endl;
				bad_stuff++;
				//int t; cin >> t;
			}

			if (nbedges_ultranaive != nbedges_fast) {
				std::cout << "Nb edges is not the same 1!" << endl;
				bad_stuff++;
				//int t; cin >> t;
			}
			
		}
	}


	//micros to millis
	std::cout << "Ultranaive time  = " << timeultranaive.count() / 1000000.0 << endl;
	std::cout << "Naive time = " << timenaive.count() / 1000000.0 << endl;
	std::cout << "Fast time  = " << timefast.count() / 1000000.0 << endl;
	
	std::cout << "Bad counter = " << bad_stuff << endl;

	for (Node* v : trees) {
		delete v;
	}
	trees.clear();
}











//Disclaimer: that function is mostly chatGPT
map<string, string> parseArguments(int argc, char* argv[]) {
	map<string, string> args;

	for (int i = 1; i < argc; ++i) {
		string arg = argv[i];

		// Check if argument starts with "--" or "-"
		if (arg.rfind("--", 0) == 0 || arg.rfind("-", 0) == 0) {
			// Remove the leading dashes
			string argName = arg.substr(arg.find_first_not_of('-'));

			// Check if the next argument exists and doesn't start with "-"
			if (i + 1 < argc && argv[i + 1][0] != '-') {
				args[argName] = argv[i + 1];
				++i;  // Skip the next argument, as it's the value for the current argument
			}
			else {
				args[argName] = "";  // For flags without a value
			}
		}
	}

	return args;
}









int main(int argc, char** argv) {


	map<string, string> args = parseArguments(argc, argv);

	if (args.count("h") || args.count("help")) {
		cout << "Computes incompatibility graph between ALL pairs of trees given in an input file" << endl;
		cout << "-i [file]    Input file containing the newick of two or more trees" << endl;
		cout << "-algo [algo]   Algo is either 'fast' or 'naive'.  Default is the O(n + d) fast algo, naive is O(n^3/w)" << endl;
		cout << "-o [dir]   Output directory (existing files are overwritten).  Program outputs your trees with added internal node labels, plus the graph as an edge list for each tree pair." << endl;
		cout << "-s [file]   If specified, write csv stats for each pair in given file" << endl;
		cout << "-x [colname1,...,colnamex=val1,...,valx]   If specified, in the stats file, colnames will be added and values added on each line" << endl;
		cout << "-m [incomp|test_incomp|rnd]   incomp is the default, it computes the graph(s).  -m test_incomp tests 3 implementations to make sure the graphs are identical";
		cout << " -m rnd generates random trees in output file.  In that case, add arguments -t [nb trees] -n [nb leaves], and if -x is set it adds an extra leaf under the root.";
	}

	//default mode is 'incomp', other modes are mostly for test
	if (!args.count("m"))
		args["m"] = "incomp";
	


	/********************************************************************************
	* test_incomp mode : tests to compare naive+ultranaive+fast incomp graph on all pairs in file -i
	*********************************************************************************/
	if (args.count("m") && args["m"] == "test_incomp") {
		exec_test_all_incomp(args["i"]);
	}





	/********************************************************************************
	* rnd mode : output a file with random unrooted binary trees
	*********************************************************************************/
	if (args.count("m") && args["m"] == "rnd") {
		
		string outfile = "";
		int nbtrees = 2;
		int nbleaves = 10;

		if (args.count("t"))
			nbtrees = Util::ToInt(args["t"]);

		if (args.count("n"))
			nbleaves = Util::ToInt(args["n"]);

		bool add_extra_leaf = false;
		if (args.count("x"))
			add_extra_leaf = true;

		ofstream outfile_stream;
		if (args.count("o")) {
			outfile = args["o"];
			outfile_stream.open(outfile);
		}

		srand(time(NULL));
		
		vector<Node*> trees;

		for (int i = 0; i < nbtrees; ++i) {
			Node* v = new Node();
			TreeUtil::get_random_binary_tree(v, nbleaves);
			
			//contract any edge to unroot, as long as it doesn't lead to a leaf
			//NOTE: we assume there are more than 2 leaves, and since it's binary one child of the root is not a leaf
			/*if (!v->get_child(0)->is_leaf())
				TreeUtil::contract_parent_edge(v->get_child(0));
			else 
				TreeUtil::contract_parent_edge(v->get_child(1));
			*/

			if (add_extra_leaf) {
				Node* c0 = v->add_child();
				c0->label = "0";
			}

			TreeUtil::randomize_branch_lengths(v, 1.0, 10000.0);
			trees.push_back(v);

			string nw = NewickLex::ToNewickString(v, true);
			nw = Util::ReplaceAll(nw, " ", "");


			if (outfile == "")
				cout << nw << endl;
			else
				outfile_stream << nw << endl;
		}



		if (outfile != "") {
			outfile_stream.close();
			cout << "Wrote "<< nbtrees << " in " << outfile << endl;
		}

		for (Node* v : trees)
			delete v;

		
	}


	/********************************************************************************
	* incomp mode : compute the incompatibility graph for all pairs of trees in a given file
	*********************************************************************************/
	if (args.count("m") && args["m"] == "incomp") {

		if (!args.count("i")) {
			cout << "Please specify an input file with -i [file]" << endl;
			return 0;
		}

		if (!args.count("o")) {
			cout << "Please specify an output directory -o [dir]" << endl;
			return 0;
		}

		string infile = args["i"];
		string outdir = args["o"];

		string algo = "fast";
		if (args.count("algo"))
			algo = args["algo"];


		MKDIR(outdir.c_str());
		

		string statsfile = "";
		if (args.count("s"))
			statsfile = args["s"];

		//map<int, map<int, microseconds>> stats;	//stats[i][j] = time to compare trees i, j

		vector<Node*> trees;
		vector<string> lines = Util::GetFileLines(infile);
		for (string line : lines) {
			Node* tree = NewickLex::ParseNewickString(line);
			trees.push_back(tree);
		}

		TreeUtil::assign_internal_labels(trees);

		cout << "Internal labels were assigned." << endl;

		string str_stats = "filename,tree1,tree2,time,nbleaves,nbedges,nbinternalnodes1,nbinternalnodes2,algo";
		
		string extra_cols = "";
		
		if (args.count("x")){
			auto strz = Util::Split(args["x"], "=");
			extra_cols = strz[1];
			str_stats += "," + strz[0];
		}
		str_stats += "\n";
		

		for (size_t i = 0; i < trees.size(); ++i) {
			for (size_t j = i + 1; j < trees.size(); ++j) {
				microseconds runtime;
				AdjList graph;

				if (algo == "naive") {
					
					auto t1 = high_resolution_clock::now();
					IncompGraphNaive ign(trees[i], trees[j]);

					

					ign.compute_incomp_graph();

					auto t2 = high_resolution_clock::now();
					runtime = duration_cast<microseconds>(t2 - t1);

					graph = ign.graph;
				}
				else {
					auto t1 = high_resolution_clock::now();

					IncompGraph ig;
					ig.compute_incomp_graph(trees[i], trees[j]);

					auto t2 = high_resolution_clock::now();
					runtime = duration_cast<microseconds>(t2 - t1);

					graph = ig.graph;
				}

				int nb_edges = 0;
				for (auto& v_nbrs : graph) {
					Node* v = v_nbrs.first;
					auto& nbrs = v_nbrs.second;

					nb_edges += nbrs.size();
				}


				int nb_internal_nodes1 = 0;
				int nb_internal_nodes2 = 0;
				int nb_leaves = 0;
				for (auto it = trees[i]->begin(); it != trees[i]->end(); ++it) {
					if (!(*it)->is_leaf())
						nb_internal_nodes1++;
					else
						nb_leaves++;
				}
				for (auto it = trees[j]->begin(); it != trees[j]->end(); ++it) {
					if (!(*it)->is_leaf())
						nb_internal_nodes2++;
				}

				str_stats += infile + "," + 
							 std::to_string(i) + "," + std::to_string(j) + "," + 
					         std::to_string(runtime.count()) + "," + 
							 std::to_string(nb_leaves) + "," + 
							 std::to_string(nb_edges) + "," + 
							 std::to_string(nb_internal_nodes1) + "," + 
							 std::to_string(nb_internal_nodes2) + "," + 
							 args["algo"];
							 
				if (extra_cols != ""){
					str_stats += "," + extra_cols;
				}
				
				str_stats += "\n";
				
				//Output the graph
				string graph_filename = "trees_" + Util::ToString((int)i) + "_" + Util::ToString((int)j) + ".edgelist";
				graph_filename = Util::path_join(outdir, graph_filename);

				output_graph(graph, graph_filename);

				//cout << "Graph " << i << "," << j << " output to " << graph_filename << endl;

			}
		}

		string str_trees = "";
		for (int i = 0; i < trees.size(); ++i) {
			str_trees += NewickLex::ToNewickString(trees[i]) + "\n";
		}
		//output the tree
		string tree_filename = "trees_relabeled.newick";
		tree_filename = Util::path_join(outdir, tree_filename);
		Util::WriteFileContent(tree_filename, str_trees);


		if (statsfile != "") {
			Util::WriteFileContent(statsfile, str_stats);
		}
		else {
			//cout << str_stats << endl;
		}

		cout << "Done. " << trees.size() * (trees.size() - 1) / 2 << " graphs output in " << outdir << endl;
		

		
		for (Node* t : trees)
			delete t;
		trees.clear();

	}



	return 0;
}



