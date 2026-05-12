#pragma once


#include "node.h"
#include "treeinfo.h"
#include "treeutil.h"






typedef unordered_map<Node*, unordered_set<Node*>> AdjList;



/**
* The implementation of the fast algorithm to build incompability graph of trees.
* See main.cpp for examples of usage
* Once compute_incomp_graph has returned, you can access the graph using the "graph" member variable (it's public, yes I know)
**/
class IncompGraph {
private:

	//IG stands for IncompGraph, not InstaGram.  An IGNodeInfo stores preprocessing info for a node
	struct IGNodeInfo {
		int postorder_index;	//postorder position OF A LEAF

		int min_postorder_desc;	//min postorder index of a descendant
		int max_postorder_desc; //max postorder index of a descendant

		Node* diff_anc;			//first ancestor with a different lcamap, or nullptr if non-existant
	};

	unordered_map<Node*, IGNodeInfo> iginfos;
	NodeMap mu12;	//lcamap from tree1 to tree2
	NodeMap nu21;	//lcamap from tree2 to tree1

	//pre-processing for O(1) diff_anc queries
	void compute_diff_anc(Node* v, NodeMap& lcamap) {
		if (v->is_root()) {
			iginfos[v].diff_anc = nullptr;
		}
		else {
			if (lcamap[v] != lcamap[v->get_parent()])
				iginfos[v].diff_anc = v->get_parent();
			else 
				iginfos[v].diff_anc = iginfos[v->get_parent()].diff_anc;
		}

		for (int i = 0; i < v->get_nb_children(); ++i)
			compute_diff_anc(v->get_child(i), lcamap);
	}


public:
	Node* tree1;
	Node* tree2;
	AdjList graph;	//graph[i] = set of neighbors of i


	/* 
	//for debugging purposes
	string get_clade(Node* v) {
		string str = "";
		for (auto it = v->begin(); it != v->end(); ++it) {
			if ((*it)->is_leaf())
				str += (*it)->label + " ";
		}
		return str;
	}
	*/

	/**
	Computes the incomp graph between two trees using the linear time algorithm from ML's paper.
	This function includes the pre-processing.  Result is stored in member variable graph
	**/
	void compute_incomp_graph(Node* tree1, Node* tree2) {


		graph.clear();

		this->tree1 = tree1;
		this->tree2 = tree2;

		TreeInfo treeinfo1(tree1);
		TreeInfo treeinfo2(tree2);

		treeinfo1.compute_nodes_by_label();
		treeinfo2.compute_nodes_by_label();

		//lcamaps and diff_anc functions
		mu12 = TreeUtil::get_lca_map(tree1, tree2, &treeinfo2);
		nu21 = TreeUtil::get_lca_map(tree2, tree1, &treeinfo1);

		compute_diff_anc(tree1, mu12);
		compute_diff_anc(tree2, nu21);

		


		//now check nodes using the two functions from the paper
		for (auto it = tree1->begin(); it != tree1->end(); ++it) {
			check_between(*it);
			check_incompat_child(*it);
		}

	}

	
	void check_between(Node* v) {
		
		for (int i = 0; i < v->get_nb_children(); ++i) {

			Node* w = v->get_child(i);
			Node* x = mu12[w];

		
			while (x && nu21[x]->has_ancestor(v)) {
				x = iginfos[x].diff_anc;
			}
			Node* muv = mu12[v];
			while (x && x->has_strict_ancestor(mu12[v]) && !graph[v].count(x)) {
				
				graph[v].insert(x);
				
				x = x->get_parent();
			}
		}
	}


	void check_incompat_child(Node* v) {
		for (int i = 0; i < v->get_nb_children(); ++i) {

			IGNodeInfo vinfo = iginfos[v];

			Node* w = v->get_child(i);

			//for each incomp neighbor x of w, add {v, x} iff L_x \not\subset L_v
			for (Node* x : graph[w]) {
				if (v->has_strict_ancestor(nu21[x])) {
					graph[v].insert(x);
					
					//cout << "Incomp incom: " << get_clade(v) << "   " << get_clade(x) << endl;
				}
			}
		}
	}


	





	/************************************************************************************************
	* Below is an old version that did check_between and check_incompat_child in a single loop. 
	* For that version, it's much harder to prove correctness, and it's not faster, so no reason to use it,
	* but it's still there.
	****************************************************************************************************/
	
	void compute_incomp_graph_old(Node* tree1, Node* tree2) {


		graph.clear();

		this->tree1 = tree1;
		this->tree2 = tree2;

		TreeInfo treeinfo1(tree1);
		TreeInfo treeinfo2(tree2);

		treeinfo1.compute_nodes_by_label();
		treeinfo2.compute_nodes_by_label();

		//lcamaps and diff_anc functions
		mu12 = TreeUtil::get_lca_map(tree1, tree2, &treeinfo2);
		nu21 = TreeUtil::get_lca_map(tree2, tree1, &treeinfo1);

		compute_diff_anc(tree1, mu12);
		compute_diff_anc(tree2, nu21);

		//in tree1: compute postorder index and [min, max] interval for each node
		int p1index = 0;
		for (auto it = tree1->begin(); it != tree1->end(); ++it) {
			Node* v = *it;

			if (v->is_leaf()) {
				
				iginfos[v].min_postorder_desc = p1index;
				iginfos[v].max_postorder_desc = p1index;
				iginfos[v].postorder_index = p1index;

				++p1index;
			}
			else {
				int minpo = iginfos[v->get_child(0)].min_postorder_desc;
				int maxpo = iginfos[v->get_child(0)].max_postorder_desc;

				//note: could be faster by just taking min fron child 0, max from last child
				for (int i = 1; i < v->get_nb_children(); ++i) {
					minpo = min(minpo, iginfos[v->get_child(i)].min_postorder_desc);
					maxpo = max(maxpo, iginfos[v->get_child(i)].max_postorder_desc);
				}

				iginfos[v].min_postorder_desc = minpo;
				iginfos[v].max_postorder_desc = maxpo;
			}
		}

		//in tree2, find min max, but use post order indices in tree1
		for (auto it = tree2->begin(); it != tree2->end(); ++it) {
			Node* v = *it;

			if (v->is_leaf()) {

				//use pindex of tree1
				Node* v_in_tree1 = treeinfo1.get_node_by_label(v->label);

				if (!v_in_tree1) {
					std::cout << "Problem: " << v->label << " not found in tree1" << std::endl;
				}

				iginfos[v].min_postorder_desc = iginfos[v_in_tree1].postorder_index;
				iginfos[v].max_postorder_desc = iginfos[v_in_tree1].postorder_index;
				iginfos[v].postorder_index = iginfos[v_in_tree1].postorder_index;

				
			}
			else {
				//take min/max among children, just like tree1
				int minpo = iginfos[v->get_child(0)].min_postorder_desc;
				int maxpo = iginfos[v->get_child(0)].max_postorder_desc;

				for (int i = 1; i < v->get_nb_children(); ++i) {
					minpo = min(minpo, iginfos[v->get_child(i)].min_postorder_desc);
					maxpo = max(maxpo, iginfos[v->get_child(i)].max_postorder_desc);
				}

				iginfos[v].min_postorder_desc = minpo;
				iginfos[v].max_postorder_desc = maxpo;
			}
		}


		//now check nodes
		for (auto it = tree1->begin(); it != tree1->end(); ++it) {
			check_node(*it);
		}

	}


	void check_node(Node* v) {

		for (int i = 0; i < v->get_nb_children(); ++i) {

			IGNodeInfo vinfo = iginfos[v];

			Node* w = v->get_child(i);

			//for each incomp neighbor x of w, add {v, x} iff L_x \not\subset L_v
			for (Node* x : graph[w]) {
				IGNodeInfo xinfo = iginfos[x];

				if (xinfo.min_postorder_desc < vinfo.min_postorder_desc || xinfo.max_postorder_desc > vinfo.max_postorder_desc) {
					graph[v].insert(x);
					//cout << "Incomp a: " << get_clade(v) << " " << get_clade(x) << endl;
				}
			}


			Node* x = mu12[w];

			while (x && nu21[x]->has_ancestor(v)) {
				x = iginfos[x].diff_anc;
			}
			Node* muv = mu12[v];
			while (x && x->has_strict_ancestor( mu12[v] ) && !graph[v].count(x)) {
				graph[v].insert(x);

				//cout << "Incomp b: " << get_clade(v) << " " << get_clade(x) << "   w="<<get_clade(w)<<endl;

				x = x->get_parent();
			}

		}
	}







};