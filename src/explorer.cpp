/****************************************************************************
  File       [ explorer.cpp ]
  Synopsis   [ Class Explorer ]
  Purpose    [ 2014 ICCAD CAD Contest problem A ]
  Authors    [ Hao-Ning Wu, Kung-Nien Yang, NTUEE ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>  // setw()
#include <string>
#include <vector>
#include <cassert>
#include "explorer.h"
using namespace std;

#define MAX_SYN_ITER  8

#ifdef WRITE_CNF
#include <sstream>
#endif // WRITE_CNF

// struct and typedef
struct Abc_Ntk_t_;
struct Abc_Frame_t_;
typedef struct Abc_Ntk_t_    Abc_Ntk_t;
typedef struct Abc_Frame_t_  Abc_Frame_t;

// external functions
extern "C" int        Cmd_CommandExecute      (void *pAbc, char *sCommand);
extern "C" Abc_Ntk_t* Abc_FrameReadNtk        (Abc_Frame_t *p);
extern "C" int        Abc_NtkNodeNumNonStatic (Abc_Ntk_t *pNtk);
#ifdef WRITE_CNF
extern     bool       exploreGlucose          (const char *satFile,
                                               const vector<const char*>& cnfFiles);
#else // WRITE_CNF
// ...
#endif // WRITE_CNF

// member functions
bool Explorer::explore(void *pAbc, const vector< vector<char*> >& cmdList)
{
   assert(cmdList.size() == 2);

   int numCmd = cmdList[0].size();
   int numSamp = _sampleList.size();
   ofstream foutEnc(_encFile, ios::out);
   if(!foutEnc.is_open())
   {
      cerr << "*ERROR*: Can't open the encoder setting file." << endl;
      return false;
   }

   // file header
   foutEnc << numSamp << " " << numCmd << endl;

   // exploration
   for(int nSamp = 0; nSamp < numSamp; ++nSamp)
   {
      cout << "Exploring " << _sampleList[nSamp] << " ..." << endl;
#ifdef DISP_PERCENT
      cout << "                 ";  // 17 spaces
      displayPercent(0, 1);
#endif // DISP_PERCENT

      string readCmd("read_verilog ");
      readCmd += _sampleList[nSamp];

      vector<bool> comb(numCmd, 0), minComb(numCmd, 0);
      int area = 0, minArea = 0;

      for(int i = 0, n = (1 << numCmd); i < n; ++i)
      {
         if(!cmdExec(pAbc, (char*)readCmd.c_str())) return false;
         if(!cmdExec(pAbc, "strash")) return false;
         for(int nCmd = 0; nCmd < numCmd; ++nCmd)
            if(!cmdExec(pAbc, cmdList[comb[nCmd]][nCmd])) return false;
         area = getGateCount(pAbc);
         if(minArea == 0 || area < minArea)
         {
            minArea = area;
            minComb = comb;
         }
#ifdef DISP_PERCENT
         displayPercent(i+1, n);
#endif // DISP_PERCENT
         incrementComb(comb);
      }  // for nCmd

      for(int i = 0; i < numCmd; ++i)
         foutEnc << minComb[i] << " ";

      if(!cmdExec(pAbc, (char*)readCmd.c_str())) return false;
      if(!cmdExec(pAbc, "strash")) return false;
      int numIter = 1;
      for(int i = 0; i < MAX_SYN_ITER; ++i)
      {
         for(int nCmd = 0; nCmd < numCmd; ++nCmd)
            if(!cmdExec(pAbc, cmdList[minComb[nCmd]][nCmd])) return false;
         area = getGateCount(pAbc);
         if(i == 0)
            minArea = area;
         else
         {
            if(area < minArea)
            {
               minArea = area;
               numIter = i + 1;
            }
         }
      }  // for i, numIter

      foutEnc << numIter << endl;

      cout << endl;

#ifdef WRITE_CNF
      // write CNF file for exploreGlucose
      stringstream ss;
      ss << "sample" << nSamp + 1 << ".tmp.cnf";
      string str = ss.str();
      char *cnfFile = (char*)str.c_str();
      // Problem:
      //    char *cnfFile = (char*)ss.str.c_str();
      //    will produce cnfFile = "sample2.0" when nSamp = 1
      _cnfFiles.push_back(cnfFile);
      if(!writeCnf(pAbc, cmdList, minComb, numIter,
                   _sampleList[nSamp], _cnfFiles[nSamp]))
         return false;
#endif // WRITE_CNF

   }  // for nSamp

   // explore SAT solver settings
   cout << "Exploring SAT solver settings ..." << endl;
#ifdef WRITE_CNF
   if(!exploreGlucose(_satFile, _cnfFiles)) return false;
#else // WRITE_CNF
   // ...
#endif // WRITE_CNF

   // close all files
   foutEnc.close();

   return true;
}

void Explorer::incrementComb(vector<bool>& comb)
{
   for(int i = 0, s = comb.size(), carry = 1; i < s && carry == 1; ++i)
   {
     if(comb[i] == 0) { comb[i] = 1; carry = 0; }
     else             { comb[i] = 0; }
   }
}

int Explorer::getGateCount(void *pAbc)
{
   return Abc_NtkNodeNumNonStatic(Abc_FrameReadNtk((Abc_Frame_t*)pAbc));
}

bool Explorer::cmdExec(void *pAbc, char *cmd)
{
   if(Cmd_CommandExecute(pAbc, cmd))
   {
      cerr << "ABC command execution error: " << cmd << endl;
      return false;
   }
   return true;
}

#ifdef DISP_PERCENT
void Explorer::displayPercent(int part, int all)
{
   // format: [||||      ]  40%
   static int curPercent = -1;
   if(part * 100 / all > curPercent)
   {
      curPercent = part * 100 / all;
      for(int i = 0; i < 17; ++i) cout << "\b";
      //cout << "\r";
      cout << "[";
      int i = 0, n = curPercent / 10;
      for(; i < n; ++i) cout << '|';
      for(; i < 10; ++i) cout << ' ';
      cout << "] " << setw(3) << curPercent << "%";
      cout.flush();
   }
   if(curPercent == 100)
      curPercent = -1;  // reset
}
#endif // DISP_PERCENT

#ifdef WRITE_CNF
bool Explorer::writeCnf(void* pAbc, const vector< vector<char*> >& cmdList,
                        const vector<bool>& minComb, int numIter,
                        const char *sampFile, const char *cnfFile)
{
   // Note: No error messages in this function.
   assert(cmdList.size() == 2);

   int numCmd = cmdList[0].size();
   string readCmd("read_verilog "), writeCmd("write_cnf ");
   readCmd += sampFile;
   writeCmd += cnfFile;
   if(Cmd_CommandExecute(pAbc, (char*)readCmd.c_str())) return false;
   if(Cmd_CommandExecute(pAbc, "strash")) return false;
   for(int i = 0; i < numIter; ++i)
   {
      for(int j = 0; j < numCmd; ++j)
         if(Cmd_CommandExecute(pAbc, cmdList[minComb[j]][j])) return false;
   }
   if(Cmd_CommandExecute(pAbc, (char*)writeCmd.c_str())) return false;

   return true;
}
#endif // WRITE_CNF

