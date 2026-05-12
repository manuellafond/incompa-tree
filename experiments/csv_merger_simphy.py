import csv
import os

cpp_lookup = {}

with open("combined_simphy.csv", newline="") as f:
    reader = csv.DictReader(f)

    for row in reader:
        key = (os.path.basename(row["filename"]), row["tree1"], row["tree2"])
        
        if not key in cpp_lookup:
            cpp_lookup[key] = [row]
        else:
            cpp_lookup[key].append(row)

outfile = open("merged_simphy.csv", "w")
outfile.write("filename,tree1,tree2,nbleaves,nbedges,nbinternalnodes1,nbinternalnodes2,pop,n,fasttime,naivetime,bhvtime\n")

nbdone = 0
for key in cpp_lookup:
    
    
    
    #eg pop_6_n_20_1.newick_0_1.csv
    bhv_filename = os.path.join("bhv", "work", key[0] + "_" + key[1] + "_" + key[2] + ".csv")
    
    if not os.path.exists(bhv_filename):
        print(bhv_filename + " does not exist, skipping")
        continue
    
    cpp_row = cpp_lookup[key]
    
    fastrow = cpp_row[0]
    naiverow = cpp_row[1]
    
    if cpp_row[0]["algo"] == "naive":
        fastrow = cpp_row[1]
        naiverow = cpp_row[0]
        
    newrow = [ fastrow["filename"], fastrow["tree1"], fastrow["tree2"], 
               fastrow["nbleaves"], fastrow["nbedges"],
               fastrow["nbinternalnodes1"], fastrow["nbinternalnodes2"], 
               fastrow["pop"], fastrow["n"] ]
               
    newrow.append(fastrow["time"])
    newrow.append(naiverow["time"])
    
    with open(bhv_filename, 'r') as file:
        content = file.read()
        pz = content.split(",")
        newrow.append(pz[3])    
        
        nbdone += 1
        if nbdone % 100 == 0:
            print(str(nbdone) + " / " + str(len(cpp_lookup)) + " rows parsed")
            
    
    
    outfile.write(",".join(newrow))
    outfile.write("\n")
    
outfile.close()
    
    