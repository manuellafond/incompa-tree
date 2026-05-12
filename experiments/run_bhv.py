import os
import re
import sys
import glob
import time
from pathlib import Path
from ete3 import Tree

#NOTE: this script expects the executable to be in ../build/incomp

#DISCLAIMER: this script is mostly AI generated


workdir = os.path.join("bhv", "work")
os.makedirs(workdir, exist_ok=True)

exe_path = os.path.join("bhv", "bhv-inc.jar")

max_trees = 60   #max number of trees to consider in each file




def add_zero_leaf_to_root(newick_str: str) -> str:
    """
    Load a Newick tree with ete3, add a leaf named "0" as a child
    of the root, and return the updated Newick string.
    """
    # Load the tree
    tree = Tree(newick_str, format=1)
    
    
    #for leaf in tree.iter_leaves():
    #    if leaf.name.endswith("_0_0"):
    #        leaf.name = leaf.name[:-4]  # remove "_0_0"
    

    # Add new leaf to the root
    tree.add_child(name="0")

    # Return updated Newick string
    return tree.write(format=1)






def run_bhv(tree1_str, tree2_str, suffix = ""):
    global workdir
    
    tmp_filename = "tmptrees"
    if suffix != "":
        tmp_filename += suffix
    tmp_filename += ".newick"
        
    tmp_file = os.path.join(workdir, tmp_filename)
    
    
    tree1_str_with_0 = add_zero_leaf_to_root(tree1_str)
    tree2_str_with_0 = add_zero_leaf_to_root(tree2_str)
    
    
    with open(tmp_file, "w") as f:
        f.write(tree1_str_with_0 + "\n")
        f.write(tree2_str_with_0 + "\n")
    
    if os.path.exists("incidence_time.txt"):
        os.remove("incidence_time.txt")    
        
    command = f"java -jar {exe_path} {tmp_file}"
    
    
    print("Running \"" + command + "\"")
    
    
    start = time.perf_counter()
    
    os.system(command)
    
    end = time.perf_counter()
    
    elapsed_microseconds = int((end - start) * 1_000_000)
    
    #get time to compute incidence matrix 
    #if incidence_time does not exist, then the algorithm never needed to compute the incidence matrix
    #because e.g. all edges are common - in that case, we count the total execution time
    if os.path.exists("incidence_time.txt"):
        incidence_time = open("incidence_time.txt").read()
    else:
        incidence_time = str(-1)
    
    print(incidence_time)
    
    lines = incidence_time.split("\n")
    incidence_time = int(lines[0])
    #for line in lines:
    #    if line.strip() != "":
    #        max_incidence_time = max(int(line), max_incidence_time)
        
    
    return (incidence_time, elapsed_microseconds) 


def compute_bhv_stats_random_trees():
    global workdir
    global exe_path
    
    # Match files like trees_1.newick, trees_42.newick, etc.
    pattern = re.compile(r"^trees_(\d+)\.newick$")

    tmp_files = []
    
    outfile = open("combined_bhv.csv", "w")
    outfile.write("filename,i,j,incidence_time,total_time\n")

    # Find matching files in current directory
    for filename in os.listdir("."):
        match = pattern.match(filename)
        if match:
            x = match.group(1)

            with open(filename, 'r') as file:
                lines = file.readlines()
                
            
            for i in range(len(lines)):
                for j in range(i+1, len(lines)):
                    if i > max_trees or j > max_trees:
                        continue
                    
                    minioutfilename = os.path.join(workdir, f"{os.path.basename(filename)}_{i}_{j}.csv")
                    if os.path.exists(minioutfilename):
                        print(f"{minioutfilename} exists, skipped")
                        continue
                    
                    
                    line_i = lines[i].strip()
                    line_j = lines[j].strip()
                    
                    if line_i != "" and line_j != "":
                        print(f"treefile={filename} i={i} j={j}")
                        (incidence_time, total_time) = run_bhv(line_i, line_j, "_" + str(i) + "_" + str(j) + "_" + str(x))
                        
                        if int(incidence_time) > 0:
                            outfile.write(f"{filename},{i},{j},{incidence_time},{total_time}\n")
                            
                            minioutfile = open(minioutfilename, "w")
                            minioutfile.write(f"{filename},{i},{j},{incidence_time},{total_time}")
                            minioutfile.close()
                        

    outfile.close()
    
    
    
    
def compute_bhv_stats_simphy_trees():
    global workdir
    global exe_path
    
    
    
    outfile = open("combined_simphy_bhv.csv", "w")
    outfile.write("filename,i,j,incidence_time,total_time\n")

    
    for n in [20,50,100,200,500,1000]:
        for pop in [8,9]:
            #if n != 20 or pop != 7:
            #    continue
            filename = os.path.join("simphyout", f"pop_{pop}_n_{n}_1.newick")
            print(filename)
            with open(filename, 'r') as file:
                lines = file.readlines()
            
            for i in range(len(lines)):
                for j in range(i+1, len(lines)):
                    if i > max_trees or j > max_trees:
                        continue
                        
                    minioutfilename = os.path.join(workdir, f"{os.path.basename(filename)}_{i}_{j}.csv")
                    if os.path.exists(minioutfilename):
                        print(f"{minioutfilename} exists, skipped")
                        continue
                        
                    line_i = lines[i].strip()
                    line_j = lines[j].strip()
                    
                    if line_i != "" and line_j != "":
                        print(f"treefile={filename} i={i} j={j}")
                        (incidence_time, total_time) = run_bhv(line_i, line_j, f"_simphy_{pop}_{n}")
                        
                        if incidence_time > 0:
                            outfile.write(f"{filename},{i},{j},{incidence_time},{total_time}\n")
                        
                            minioutfile = open(minioutfilename, "w")
                            minioutfile.write(f"{filename},{i},{j},{incidence_time},{total_time}")
                            minioutfile.close()
            
            
    outfile.close()

    
    
#compute_bhv_stats_random_trees()
compute_bhv_stats_simphy_trees()

