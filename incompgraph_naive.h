#pragma once

#include "node.h"
#include "incompgraph.h"

#include "ewah/ewah.h"
typedef ewah::EWAHBoolArray<uint32_t> bitmap;



struct NodeCladeInfo {
	int id;
	bitmap clade;
	bitmap clade_comp;	//complement of the clade
};

/**
Computes the incomp graph between two trees using the naive algorithm.  It goes through each clade of t1 and t2
and checks for incompatibility using the intersections.
**/
class IncompGraphNaive{

public:	//screw this, everything public (TODO)

	Node* t1;
	Node* t2;
	unordered_map<string, int> label_to_leafid;
	unordered_map<Node*, NodeCladeInfo> infos;

	AdjList graph;

	bitmap _all_ones;	//must contain 1111...11, nb of ones = nb of leaves, needed to calculate intersections

	int cpt_id, leaf_id;
	int nb_leaves;

	IncompGraphNaive(Node* t1, Node* t2) : t1(t1), t2(t2) {
		cpt_id = 1;
		leaf_id = 1;


		//count leaves to build an all ones bitmap
		nb_leaves = 0;
		for (auto it = t1->begin(); it != t1->end(); ++it) {
			if ((*it)->is_leaf()) {
				nb_leaves++;
				_all_ones.set(nb_leaves);
			}
		}


		preprocess_tree_rec(t1, true);

		preprocess_tree_rec(t2, false);
	}

	void preprocess_tree_rec(Node* v, bool is_tree1) {

		infos[v].id = cpt_id;
		cpt_id++;

		if (v->is_leaf()) {

			if (is_tree1) {
				infos[v].id = leaf_id;

				infos[v].clade.set(leaf_id);
				infos[v].clade_comp = _all_ones.logicalandnot(infos[v].clade);

				label_to_leafid[v->label] = leaf_id;
				leaf_id++;
			}
			else {
				infos[v].id = label_to_leafid[v->label];
				infos[v].clade.set(infos[v].id);
				infos[v].clade_comp = _all_ones.logicalandnot(infos[v].clade);
			}

		}
		else {
			for (int i = 0; i < v->get_nb_children(); ++i) {
				preprocess_tree_rec(v->get_child(i), is_tree1);

				if (i == 0)
					infos[v].clade = infos[v->get_child(i)].clade;
				else
					infos[v].clade = infos[v].clade | infos[v->get_child(i)].clade;
			}
			
			infos[v].clade_comp = _all_ones.logicalandnot(infos[v].clade);
		}
	}



	void compute_incomp_graph() {

		
		int cpt = 0;
		for (auto it1 = t1->begin(); it1 != t1->end(); ++it1) {

			//cpt++;

			//if (cpt % 100 == 0)
			//	cout << "cpt=" << cpt << endl;
			Node* v1 = (*it1);


			bitmap& A = infos[v1].clade;
			bitmap& B = infos[v1].clade_comp;

			for (auto it2 = t2->begin(); it2 != t2->end(); ++it2) {

				Node* v2 = (*it2);

				bitmap& C = infos[v2].clade;
				bitmap& D = infos[v2].clade_comp;



				//incompatible iff nb intersections is one or four
				/*int nbinter = 0;
				if (A.intersects(C))
					++nbinter;
				if (A.intersects(D))
					++nbinter;
				if (B.intersects(C))
					++nbinter;
				if (B.intersects(D))
					++nbinter;

				//if (nbinter != 2 && nbinter != 3) {
				if (nbinter == 4) {
					graph[v1].insert(v2);
					cout << "Comparing v=" << A << "|" << B << "  w=" << C << "|" << D << "      nbi=" << nbinter << endl;
				}
				*/
				if (A.intersects(C) && A.intersects(D) && C.intersects(B)) {
					graph[v1].insert(v2);
				}

			}
		}

	}
};