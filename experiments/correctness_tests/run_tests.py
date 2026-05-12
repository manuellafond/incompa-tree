import os

resfile = "results.txt"

if os.path.exists(resfile):
    os.remove(resfile)

files = []

for n in [20, 50, 100, 200, 500, 1000]:
    files.append("../trees_" + str(n) + ".newick")
    
    for pop in [6,7,8,9]:
        files.append("../simphyout/pop_" + str(pop) + "_n_" + str(n) + "_1.newick")
        
        
for f in files:

    os.system("echo " + f + " >> " + resfile)

    command = "../../build/incomp -m test_incomp -i " + f + " >> " + resfile
    
    print(command)
    os.system(command)

