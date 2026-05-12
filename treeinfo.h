#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <iostream>

#include "node.h"




class TreeInfo {
private:

	Node* root;

	std::unordered_map<std::string, Node*> nodes_by_label;


public:

	
	

	TreeInfo(Node* root) {
		this->root = root;
	}

	void compute_nodes_by_label() {
		nodes_by_label.clear();

		for (auto it = root->begin(); it != root->end(); ++it) {
			Node* v = *it;
			if (v->label != "") {
				if (nodes_by_label.count(v->label)) {
					std::cout << "Warning: label " << v->label << " exists twice in the tree" << std::endl;
				}

				nodes_by_label[v->label] = v;

			}
		}
	}


	/*void compute_postorder_index() {
		
		int i = 0;
		for (auto it = root->begin(); it != root->end(); ++it) {
			node_info[*it].postorder_index = i;
			++i;
		}
	}*/


	Node* get_node_by_label(std::string label, bool compute_nodemap_if_empty = true) {
		if (nodes_by_label.empty()) {
			if (compute_nodemap_if_empty)
				compute_nodes_by_label();
		}
		if (nodes_by_label.count(label))
			return nodes_by_label[label];
		return nullptr;
	}

};