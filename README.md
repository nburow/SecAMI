SecAMI
======

Smart Grid Research Project

build using our make file.

To run:

graph: 4 command line arguments: # nodes, # connections, RNG Seed (9 digits), #Graphs to generate
attackSim: # of Start graph, # end graph, graph directory, result file, # nodes

Notes:

graph creates a Graphs directory with files graph0.txt to graph#.txt.  The number in these file names is what the 1st and 2nd argument to attackSim refers to.

the #nodes argument to attackSim is the maximum node # to use as a starting point.

Results:

are in the result files.  We moved these to the analysis folder, and used script.sh to generate files called "postX.txt" where X was a numeric designation we used
in naming the results file.  We then used Excel to plot these files.  The first column of numbers is the ratio of compromise time to detection time.  The second
column is the average % of the network alive over all the graphs used by attackSim to generate that results file.
