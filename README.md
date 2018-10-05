Simultaneous CNF Encoder Optimization with SAT solver Setting Selection
===

### FILES
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

### COMPILATION
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

### EXECUTION
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
      
3. For more information, please refer to the problem's [webpage](http://cad_contest.ee.ncu.edu.tw/CAD-contest-at-ICCAD2014/problem_a/default.html)

