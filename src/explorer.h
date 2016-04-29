/****************************************************************************
  File       [ explorer.h ]
  Synopsis   [ Class Explorer ]
  Purpose    [ 2014 ICCAD CAD Contest problem A ]
  Authors    [ Hao-Ning Wu, Kung-Nien Yang, NTUEE ]
****************************************************************************/

#ifndef EXPLORER_H
#define EXPLORER_H

#include <vector>
using namespace std;

#define DISP_PERCENT
#define WRITE_CNF

class Explorer
{
public:
   Explorer() : _encFile(""), _satFile("") {}
   ~Explorer() {}

   void addSample   (const char* fileName) { _sampleList.push_back(fileName); }
   void clearSample ()                     { _sampleList.clear(); }
   void setEncFile  (const char* fileName) { _encFile = fileName; }
   void setSatFile  (const char* fileName) { _satFile = fileName; }

   bool explore(void *pAbc, const vector< vector<char*> >& cmdList);

private:
   vector <const char*> _sampleList;
   const char*          _encFile;
   const char*          _satFile;

   // utility functions
   void incrementComb  (vector<bool>& comb);
   int  getGateCount   (void *pAbc);
   bool cmdExec        (void *pAbc, char *cmd);
#ifdef DISP_PERCENT
   void displayPercent (int part, int all);
#endif // DISP_PERCENT

#ifdef WRITE_CNF
   vector <const char*> _cnfFiles;
   bool writeCnf       (void* pAbc, const vector< vector<char*> >& cmdList,
                        const vector<bool>& minComb, int numIter,
                        const char *sampFile, const char *cnfFile);
#endif // WRITE_CNF

};


#endif // EXPLORER_H
