# 2014 CAD Contest Problem A: <br /> Simultaneous CNF Encoder Optimization with SAT solver Setting Selection

### Introduction
We provide an efficient approach for parameter optimization in SAT solving. Given 3 combinational single-output netlists as samples, our goal is to learn the setting that can generalize to other test cases with similar circuit characteristics. To achieve this goal, [**ABC**](https://people.eecs.berkeley.edu/~alanmi/abc/) and [**Glucose**](http://www.labri.fr/perso/lsimon/glucose/) are incorporated into our framework. **ABC** uses sequential synthesis to generate simplified circuit then encodes it into CNF format, while **Glucose** returns either SAT or UNSAT for each test case. For detailed problem description, please refer to the contest's [webpage](http://cad_contest.ee.ncu.edu.tw/CAD-contest-at-ICCAD2014/problem_a/default.html)

To find the best CNF encoder setting, we interleave standard synthesis scripts provided by **ABC** like ```resyn```, ```compress``` for several iterations. The sequence resulting in the fewest gate count is stored as a vector. This allows us to find the best setting among 3 samples by minimizing the MSE. To find the best SAT solver setting, we exhaustive search through all possible *K* and *R* (**Glucose** parameters) using *nb learnts DL2* as the speed indicator. For detailed method explanation, please refer to ```Report.pdf```.

Our project won an honorable mention (top-10) in 2014 ICCAD Contest.

### Files
```
•
├── abc/                        The ABC package
├── glucose_exp/                The Glucose package for exploration
├── src/                        Source code
│   ├── exploreGlucose.cpp      The function for solver setting exploration
│   ├── explorer.cpp            Implementation of class Explorer
│   ├── explorer.h              Definition of class Explorer
│   ├── generator.cpp           Implementation of class Generator
│   ├── generator.h             Definition of class Generator
│   ├── main.cpp                Main function
│   └── Makefile                Makefile
├── tc/                         Test cases
├── README                      This file
└── Report.pdf                  Final report  
```
\* Notes: Class Explorer is responsible for setting exploration, and class Generator is responsible for CNF generatoin.

### Compilation
1. Compiling the ABC library ```libabc.a```:

   Go to ```abc/```, then use
   ```shell
   make libabc.a READLINE=0 PTHREADS=0
   ```
   \* Notes: This may consume a lot of time
   
2. Compiling ```glucose_exp``` for exploration:

   Go to ```glucose_exp/core```
   ```shell
   make
   mv glucose glucose_exp
   ```

3. Compiling the program ```expcnf```:

   Move the ```libabc.a``` to ```src/```, then use
   ```shell
   make
   ```

### Execution
1. Put ```expcnf``` and ```glucose_exp``` in the same directory
2. Command-line syntax:

   * Exploration
      ```
      expcnf -exp <sample1.v> <sample2.v> <sample3.v> <EncoderSetting> <SATSolverSetting>
      ```
   * Generation
      ```
      expcnf -gen <EncoderSetting> <test.v> <CnfOutput>
      ```
  

