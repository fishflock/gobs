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