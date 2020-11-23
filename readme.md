# Geometry of Behavioral Spaces Framework

### Running Main
To run the program, run the "make" command from the gobs directory. Then run ./gobs with the following command line arguments:

1. Input file path
2. Output file path
3. Normalization flag (0 or 1)
4. Alpha value
5. Vector history length (5-15) 
6. Number of directions (4 or 8)
7. Statistical Comparison Method:
   1 for X Squared test
   2 for G-Test

Example run command: "./gobs Main/input/basic.csv output.txt 1 1 .1 4 1"


### NetLogo Data Generation
1. Download NetLogo from https://ccl.northwestern.edu/netlogo/
2. Open a nlogo model (i.e. Attractors.nlogo)
3. Click Setup
3. Click Go
4. Wait for sim to finish
5. Verify that data.txt was generated (should be same directory as the nlogo model you are running)


### Running NetworkX
The following packages need to be installed with pip:
- networkx
- fa2 (https://github.com/bhargavchippada/forceatlas2)
- numpy
- matplotlib

Run 'extract.py' either in cmd or IDLE with the following arguments:
1. Input file path (.txt file)
2. Output file path (.png file)
3. Scale Metric ('weight', 'close', 'eigen', or 'between')
4. Sorting Algorithm ('lpa' or 'blondel')
5. Layout ('spring' or 'fa2')
6. Scale Multiplier (float between 0 and 1)

Example run command: 'python extract.py input.txt output.txt weight blondel spring 0.5'