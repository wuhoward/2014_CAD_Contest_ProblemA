/****************************************************************************
  File       [ generator.h ]
  Synopsis   [ Class Generator ]
  Purpose    [ 2014 ICCAD CAD Contest problem A ]
  Authors    [ Hao-Ning Wu, Kung-Nien Yang, NTUEE ]
****************************************************************************/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
using namespace std;

class Generator
{
public:
   Generator() : _encFile(""), _cnfFile("") {}
   ~Generator() {}

   void setEncFile  (const char* fileName) { _encFile = fileName; }
   void setCnfFile  (const char* fileName) { _cnfFile = fileName; }
   void setTestFile (const char* fileName) { _testFile = fileName; }

   bool generate(void *pAbc, const vector< vector<char*> >& cmdList);

private:
   const char*            _encFile;
   const char*            _cnfFile;
   const char*            _testFile;
   vector< vector<bool> > _cmdComb;  // command combinations loaded from...
                                     // ...the encoder setting file
   vector<int>            _numIter;  // number of iterations loaded from...
                                     // ...the encoder setting file

   // utility functions
   void incrementComb      (vector<bool>& comb);
   bool cmdExec            (void *pAbc, char *cmd);
   int  squaredHammingDist (const vector<bool>& b1, const vector<bool>& b2);
   int  getGateCount       (void *pAbc);
   int  getPiCount         (void *pAbc);

};


#endif // GENERATOR_H
