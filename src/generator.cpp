/****************************************************************************
  File       [ generator.cpp ]
  Synopsis   [ Class Generator ]
  Purpose    [ 2014 ICCAD CAD Contest problem A ]
  Authors    [ Hao-Ning Wu, Kung-Nien Yang, NTUEE ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "generator.h"
using namespace std;

#define COLLAPSE
#define COLLAPSE_CONDITION\
        ((getGateCount(pAbc) < 2000 && getPiCount(pAbc) < 30) ||\
         (getGateCount(pAbc) < 1100))

// struct and typedef
struct Abc_Ntk_t_;
struct Abc_Frame_t_;
typedef struct Abc_Ntk_t_    Abc_Ntk_t;
typedef struct Abc_Frame_t_  Abc_Frame_t;

// external functions
extern "C" int        Cmd_CommandExecute      (void *pAbc, char *sCommand);
extern "C" Abc_Ntk_t* Abc_FrameReadNtk        (Abc_Frame_t *p);
extern "C" int        Abc_NtkNodeNumNonStatic (Abc_Ntk_t *pNtk);
extern "C" int        Abc_NtkPiNumNonStatic   (Abc_Ntk_t *pNtk);

// member functions
bool Generator::generate(void *pAbc, const vector< vector<char*> >& cmdList)
{
   assert(cmdList.size() == 2);

   ifstream finEnc(_encFile, ios::in);
   if(!finEnc.is_open())
   {
      cerr << "*ERROR*: Can't open the encoder setting file." << endl;
      return false;
   }

   // read data from the setting file
   // Note: The validity of the file is not checked.
   cout << "Parsing encoder setting file ..." << endl;
   int numSamp = 0, numCmd = 0;
   finEnc >> numSamp >> numCmd;
   _cmdComb.insert(_cmdComb.begin(), numSamp, vector<bool>(numCmd, 0));
   _numIter.insert(_numIter.begin(), numSamp, 0);
   for(int i = 0; i < numSamp; ++i)
   {
      for(int j = 0; j < numCmd; ++j)
      {
         bool b = 0;
         finEnc >> b;
         _cmdComb[i][j] = b;
      }
      int iter = 0;
      finEnc >> iter;
      _numIter[i] = iter;
   }

   // determine minComb and iter
   cout << "Determining encoding scheme ..." << endl;
   vector<bool> comb(numCmd, 0), minComb(numCmd, 0);
   int iter = 0, minErr = 0;
   // - iter
   for(int i = 0; i < numSamp; ++i)
      iter += _numIter[i];
   iter /= numSamp;
   // - minComb
   for(int i = 0, n = (1 << numCmd); i < n; ++i)
   {
      int error = 0;
      for(int j = 0; j < numSamp; ++j)
         error += squaredHammingDist(comb, _cmdComb[j]);

      if(i == 0 || error < minErr)
      {
         minErr = error;
         minComb = comb;
      }

      incrementComb(comb);
   }

   // generate CNF file
   cout << "Generating CNF file for " << _testFile << " ..." << endl;
   string readCmd("read_verilog "), writeCmd("write_cnf ");
   readCmd += _testFile;
   writeCmd += _cnfFile;
   if(!cmdExec(pAbc, (char*)readCmd.c_str())) return false;
   if(!cmdExec(pAbc, "strash")) return false;
#ifdef COLLAPSE
   if(COLLAPSE_CONDITION)
   {
      cout << "(Note: CNF file is generated directly through collapsing.)" << endl;
      if(!cmdExec(pAbc, "collapse")) return false;
      if(!cmdExec(pAbc, "strash")) return false;  // convert the circuit to an AIG
   }
   else
   {
#endif // COLLAPSE
      for(int i = 0; i < iter; ++i)
      {
         for(int j = 0; j < numCmd; ++j)
            if(!cmdExec(pAbc, cmdList[minComb[j]][j])) return false;
      }
#ifdef COLLAPSE
      if(COLLAPSE_CONDITION)
      {
         cout << "(Note: The circuit is collapsed after synthesis.)" << endl;
         if(!cmdExec(pAbc, "collapse")) return false;
         if(!cmdExec(pAbc, "strash")) return false;  // convert the circuit to an AIG
      }
   }
#endif // COLLAPSE
   if(!cmdExec(pAbc, (char*)writeCmd.c_str())) return false;

   // close all files
   finEnc.close();

   return true;
}

void Generator::incrementComb(vector<bool>& comb)
{
   for(int i = 0, s = comb.size(), carry = 1; i < s && carry == 1; ++i)
   {
     if(comb[i] == 0) { comb[i] = 1; carry = 0; }
     else             { comb[i] = 0; }
   }
}

bool Generator::cmdExec(void *pAbc, char *cmd)
{
   if(Cmd_CommandExecute(pAbc, cmd))
   {
      cerr << "ABC command execution error: " << cmd << endl;
      return false;
   }
   return true;
}

int Generator::squaredHammingDist(const vector<bool>& b1, const vector<bool>& b2)
{
   assert(b1.size() == b2.size());
   int dist = 0;
   for(int i = 0, n = b1.size(); i < n; ++i)
      if(b1[i] != b2[i]) ++dist;

   return dist * dist;
}

int Generator::getGateCount(void *pAbc)
{
   return Abc_NtkNodeNumNonStatic(Abc_FrameReadNtk((Abc_Frame_t*)pAbc));
}

int Generator::getPiCount(void *pAbc)
{
   return Abc_NtkPiNumNonStatic(Abc_FrameReadNtk((Abc_Frame_t*)pAbc));
}

