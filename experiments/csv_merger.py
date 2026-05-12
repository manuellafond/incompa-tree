import csv
import os

rnd_lookup = {}

with open("combined_rnd.csv", newline="") as f:
    reader = csv.DictReader(f)

    for row in reader:
        key = (row["filename"], row["tree1"], row["tree2"])
        
        if not key in rnd_lookup:
            rnd_lookup[key] = [row]
        else:
            rnd_lookup[key].append(row)

outfile = open("merged.csv", "w")
outfile.write("filename,tree1,tree2,nbleaves,nbedges,nbinternalnodes1,nbinternalnodes2,fasttime,naivetime,bhvtime\n")

nbdone = 0
for key in rnd_lookup:
    
    
    
    #eg trees_500.newick_16_36.csv
    bhv_filename = os.path.join("bhv", "work", key[0] + "_" + key[1] + "_" + key[2] + ".csv")
    
    if not os.path.exists(bhv_filename):
        print(bhv_filename + " does not exist, skipping")
        continue
    
    rnd_rows = rnd_lookup[key]
    
    fastrow = rnd_rows[0]
    naiverow = rnd_rows[1]
    
    if rnd_rows[0]["algo"] == "naive":
        fastrow = rnd_rows[1]
        naiverow = rnd_rows[0]
        
    newrow = [ fastrow["filename"], fastrow["tree1"], fastrow["tree2"], 
               fastrow["nbleaves"], fastrow["nbedges"],
               fastrow["nbinternalnodes1"], fastrow["nbinternalnodes2"] ]
               
    newrow.append(fastrow["time"])
    newrow.append(naiverow["time"])
    
    with open(bhv_filename, 'r') as file:
        content = file.read()
        pz = content.split(",")
        newrow.append(pz[3])    
        
        nbdone += 1
        if nbdone % 100 == 0:
            print(str(nbdone) + " rows parsed")
            
    
    
    outfile.write(",".join(newrow))
    outfile.write("\n")
    
outfile.close()
    
    