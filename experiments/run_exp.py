import os
import re
import sys
import glob


#NOTE: this script expects the executable to be in ../build/incomp

#DISCLAIMER: this script is mostly AI generated


workdir = "work"
os.makedirs(workdir, exist_ok=True)

exe_path = os.path.join("..", "build", "incomp")


do_rnd = True 
do_simphy = True


def compute_combined_stats_random_trees(algo, write_mode = "w"):
    global workdir
    global exe_path
    
    # Match files like trees_1.newick, trees_42.newick, etc.
    pattern = re.compile(r"^trees_(\d+)\.newick$")

    tmp_files = []

    # Find matching files in current directory
    for filename in os.listdir("."):
        match = pattern.match(filename)
        if match:
            x = match.group(1)

            tmp_filename = os.path.join(workdir, f"stats_{x}_{algo}.csv")
            tmp_files.append(tmp_filename)

            
            command = f"{exe_path} -i {filename} -s {tmp_filename} -algo {algo}"
            print("Launching " + command)
            os.system(command)

            

    # Concatenate all
    output_file = f"combined_rnd.csv"

    with open(output_file, write_mode) as outfile:
        for tmp_file in sorted(tmp_files):
            with open(tmp_file, "r") as infile:
                outfile.write(infile.read())

    # Delete all tmp_X.txt files
    #for tmp_file in tmp_files:
    #    os.remove(tmp_file)

    print(f"Created {output_file}")
    
    
    
if do_rnd:
    compute_combined_stats_random_trees("fast", "w")
    compute_combined_stats_random_trees("naive", "a")








def compute_combined_stats_simphy_trees(algo, pop, n, write_mode = "w"):
    global workdir
    global exe_path
    
    filename = f"simphyout/pop_{pop}_n_{n}_1.newick"
    stats_filename = os.path.join(workdir, f"stats_{algo}_simphy_pop_{pop}_n_{n}_1.csv")
    
    extra_col_string = f"pop,n={pop},{n}"
    command = f"{exe_path} -i {filename} -s {stats_filename} -algo {algo} -x {extra_col_string}"
    print("Launching " + command)
    os.system(command)
        


    output_file = f"combined_simphy.csv"

    with open(output_file, write_mode) as outfile:
        with open(stats_filename, "r") as infile:
            outfile.write(infile.read())


    print(f"Created {output_file}")
    

if do_simphy:

    first_done = False 

    for pop in [6,7,8,9]:
        for n in [20,50,100,200,500,1000]:
               
            compute_combined_stats_simphy_trees("fast", pop, n, write_mode = "w" if not first_done else "a")
            compute_combined_stats_simphy_trees("naive", pop, n, write_mode = "a")
        
            first_done = True