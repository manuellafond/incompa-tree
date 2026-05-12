#pragma once

#include "node.h"
#include "incompgraph.h"

#include <set>

using namespace std;






/**
Computes the incomp graph between two trees using the ultra naive algorithm, using sets.
**/
class IncompGraphUltraNaive{

public:	//screw this, everything public (TODO)

	Node* t1;
	Node* t2;
	unordered_map<Node*, set<string>> clades;

	AdjList graph;

	int cpt_id, leaf_id;
	int nb_leaves;

	IncompGraphUltraNaive(Node* t1, Node* t2) : t1(t1), t2(t2) {
		this->t1 = t1;
		this->t2 = t2;
		
		
	}




	bool hasEmptyIntersection(const set<string>& a, const set<string>& b)
	{
		auto itA = a.begin();
		auto itB = b.begin();

		while (itA != a.end() && itB != b.end())
		{
			if (*itA < *itB)
				++itA;
			else if (*itB < *itA)
				++itB;
			else
				return false; // found common element
		}

		return true;
	}



	bool isSubset(const set<string>& subset, const set<string>& superset)
	{
		return std::includes(
			superset.begin(), superset.end(),
			subset.begin(), subset.end());
	}
	


	void compute_incomp_graph() {
		//compute clades of t1
		for (auto it = t1->begin(); it != t1->end(); ++it) {
			Node* v = (*it);
			
			set<string> vclade;
			if (v->is_leaf()) {
				vclade.insert(v->label);
				clades[v] = vclade;
			}
			else {
				for (int i = 0; i < v->get_nb_children(); ++i) {
					set<string> chclade = clades[v->get_child(i)];
					vclade.insert(chclade.begin(), chclade.end());
					clades[v] = vclade;
				}
			}
		}

		//compute clades of t2
		for (auto it = t2->begin(); it != t2->end(); ++it) {
			Node* v = (*it);

			set<string> vclade;
			if (v->is_leaf()) {
				vclade.insert(v->label);
				clades[v] = vclade;
			}
			else {
				for (int i = 0; i < v->get_nb_children(); ++i) {
					set<string> chclade = clades[v->get_child(i)];
					vclade.insert(chclade.begin(), chclade.end());
					clades[v] = vclade;
				}
			}
		}




		
		int cpt = 0;
		for (auto it1 = t1->begin(); it1 != t1->end(); ++it1) {

			
			Node* v1 = (*it1);

			set<string> v1_clade = clades[v1];

			for (auto it2 = t2->begin(); it2 != t2->end(); ++it2) {

				Node* v2 = (*it2);

				set<string> v2_clade = clades[v2];


				if (!(hasEmptyIntersection(v1_clade, v2_clade) ||
					  isSubset(v1_clade, v2_clade) || 
					  isSubset(v2_clade, v1_clade))) {
					graph[v1].insert(v2);
				}
			}
		}

	}
};