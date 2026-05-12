#pragma once

#include <unordered_map>

#include "node.h"
#include "util.h"
#include "treeinfo.h"

typedef std::unordered_map<Node*, Node*> NodeMap;

/**
* Several utility functions for trees, to be used with node.h
**/
class TreeUtil {
public:



    /**
    * Returns a random binary tree obtained by splitting into two random sets of leaves at each node.
    * The input is a tree with a single node (this enforces delegating the "new" creation to the caller).
    * For example 
    * Node* r = new Node();
    * TreeUtil::get_random_binary_tree(r, 100);
    **/
    static void get_random_binary_tree(Node* root, int nb_leaves) {
        set<int> labels;
        for (int i = 1; i <= nb_leaves; ++i)
            labels.insert(i);

        return get_random_binary_tree_rec(root, labels);    //see private part below
    }



    


    /**
    * Contracts a node v with its parent.  If v is the root, does nothing.
    * NOTE: v is deleted, freed from memory!
    **/
    static void contract_parent_edge(Node* v) {
        if (v->is_root())
            return;

        Node* p = v->get_parent();
        p->remove_child(v);
        for (int i = 0; i < v->get_nb_children(); ++i) {
            p->add_subtree(v->get_child(i));
        }
        v->remove_all_children(false);
        delete v;
    }



    /**
    * Applies a random branch length to the parent branch of v, as well as to all of its descendant branches.
    **/
    static void randomize_branch_lengths(Node* v, double min, double max) {
        if (!v->is_root()) {
            double r = (double)rand() / RAND_MAX;
            double b = min + r * (max - min);
            v->branch_length = b;
        }

        for (int i = 0; i < v->get_nb_children(); ++i) {
            randomize_branch_lengths(v->get_child(i), min, max);
        }

    }

    /**
    Creates a degree 2 node between v and its parent, and returns the new node.  If v is the root, does nothing and returns nullptr.
    **/
    static Node* subdivide_parent_edge(Node* v) {
        if (v->is_root())
            return nullptr;

        Node* old_parent = v->get_parent();
        v->get_parent()->remove_child(v);

        Node* w = new Node();
        w->add_subtree(v);
        old_parent->add_subtree(w);

        return w;
    }



    /**
    * Makes v the root
    **/
    static void reroot_on_node(Node* v) {
        vector<Node*> ancestors;

        Node* cur = v;
        while (cur) {
            ancestors.push_back(cur);
            cur = cur->get_parent();
        }


        for (int i = ancestors.size() - 1; i >= 1; --i){
            Node* w = ancestors[i];

            w->remove_child(ancestors[i - 1]);
            ancestors[i - 1]->add_subtree(w);
        }
    }


    /**
    * returns the lca-mapping from r1 to r2.  Corresponding leaves must have the same label.
    * infotree2, if not null, must have computed nodes by label.  If null, the function will do it
    * TODO: pass a separator + position for the leaf-to-leaf map
    **/
    static NodeMap get_lca_map(Node* r1, Node* r2, TreeInfo* infotree2 = nullptr) {
        
        NodeMap lcamap;

        if (!infotree2)
            infotree2->compute_nodes_by_label();

        for (auto it = r1->begin(); it != r1->end(); ++it) {
            Node* v = *it;

            if (v->is_leaf()) {
                Node* w = infotree2->get_node_by_label(v->label);
                lcamap[v] = w;
            }
            else {
                Node* lca = lcamap[v->get_child(0)];

                for (int i = 1; i < v->get_nb_children(); ++i) {
                    lca = lca->get_lca_with( lcamap[v->get_child(i)] );
                }
                lcamap[v] = lca;
            }
        }

        return lcamap;
    }



    /**
    * Assigns a distinct label to internal nodes across ALL given trees.  Nodes with no label are assigned a label 
    * of the form Ixx, where xx is an integer. 
    * Nodes that already had a label keep their label, UNLESS the label is repeated.  The j-th occurrence of a label X with j >= 2
    * becomes X_j
    **/
    static void assign_internal_labels(vector<Node*> &trees) {
        int cpt = 0;

        map<string, int> seen_labels;

        for (size_t i = 0; i < trees.size(); ++i) {
            Node* t = trees[i];

            for (auto it = t->begin(); it != t->end(); ++it) {
                Node* v = *it;

                if (v->is_leaf())
                    continue;

                if (v->label == "") {
                    v->label = "I" + Util::ToString(cpt);
                    cpt++;
                }
                else {
                    if (seen_labels.count(v->label)) {
                        seen_labels[v->label]++;
                        v->label = v->label + "_" + Util::ToString(seen_labels[v->label]);
                    }
                    else {
                        seen_labels[v->label] = 1;
                    }
                }
            }
        }

    }


private:
    static void get_random_binary_tree_rec(Node* v, set<int>& indices) {

        if (indices.size() == 1) {
            v->label = (Util::ToString(*indices.begin()));
            return;
        }

        set<int> left;
        set<int> right;

        bool done = false;

        while (!done) {
            for (auto it = indices.begin(); it != indices.end(); ++it) {
                int x = rand() % 2;
                if (x == 0)
                    left.insert(*it);
                else
                    right.insert(*it);
            }

            //dumb way to ensure no empty child
            if (left.empty() || right.empty()) {
                left.clear();
                right.clear();
            }
            else {
                done = true;
            }
        }


        Node* v1 = new Node();
        get_random_binary_tree_rec(v1, left);
        Node* v2 = new Node();
        get_random_binary_tree_rec(v2, right);

        v->add_subtree(v1);
        v->add_subtree(v2);
    }


};