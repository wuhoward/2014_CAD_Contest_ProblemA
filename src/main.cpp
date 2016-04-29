/****************************************************************************
  File       [ main.cpp ]
  Synopsis   [ Main function ]
  Purpose    [ 2014 ICCAD CAD Contest problem A ]
  Authors    [ Hao-Ning Wu, Kung-Nien Yang, NTUEE ]
  Version    [  ]
****************************************************************************/

#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cstring>  // strcmp()
#include "explorer.h"
#include "generator.h"

using namespace std;

/***********************
/ Constant Definitions /
***********************/
// these constants are for reference only
#define NUM_CMD       5
#define NUM_SAMPLE    3


/*********************
/ External Functions /
*********************/
// procedures to start and stop the ABC framework
// (should be called before and after the ABC procedures are called)
extern "C" void  Abc_Start();
extern "C" void  Abc_Stop();

// procedures to get the ABC framework and execute commands in it
extern "C" void* Abc_FrameGetGlobalFrame();
extern "C" int   Cmd_CommandExecute(void *pAbc, char *sCommand);

/************************
/ Function Declarations /
************************/
bool      parseArguments (int argc, char **argv);
void      printUsage     ();
void      abcAlias       ();

/*******************
/ Global Variables /
*******************/
enum ExecMode { EXP_MODE = 0, GEN_MODE }
                        mode;
void*                   pAbc;
vector< vector<char*> > cmdList(2, vector<char*>());
Explorer*               pExp = 0;
Generator*              pGen = 0;

/****************
/ Main function /
****************/
// Command-line syntax:
//    expcnf -exp <sample1.v> <sample2.v> <sample3.v> <EncoderSetting> <SATSolverSetting>
//    expcnf -gen <EncoderSetting> <test.v> <CnfOutput>
int main(int argc, char **argv)
{
   // start timer
   clock_t clk = clock();

   // parse arguments
   if(!parseArguments(argc, argv))
   {
      cerr << "Exiting..." << endl;
      exit(1);
   }

   // start the ABC framework
   Abc_Start();

   // initialization
   pAbc = Abc_FrameGetGlobalFrame();
   abcAlias();

   // exploration, generation
   if(mode == EXP_MODE)
   {
      if(!pExp->explore(pAbc, cmdList))
         cerr << "Exploration fails." << endl;
   }
   else  // mode == GEN_MODE
   {
      if(!pGen->generate(pAbc, cmdList))
         cerr << "Generation fails." << endl;
   }

   // stop the ABC framework
   Abc_Stop();

   // stop timer
   clk = clock() - clk;
   cout << "Execution time = " << (double(clk)) / CLOCKS_PER_SEC << " sec" << endl;

   return 0;
}

/***********************
/ Function Definitions /
***********************/
bool parseArguments(int argc, char **argv)
{
   // Command-line syntax:
   //    expcnf -exp <sample1.v> <sample2.v> <sample3.v> <EncoderSetting> <SATSolverSetting>
   //    expcnf -gen <EncoderSetting> <test.v> <CnfOutput>

   // usage
   if(argc == 1) { printUsage(); return false; }

   // flag (-exp, -gen)
   if     (!strcmp(argv[1], "-exp")) mode = EXP_MODE;
   else if(!strcmp(argv[1], "-gen")) mode = GEN_MODE;
   else
   {
      cerr << "*ERROR*: Unknown flag \"" << argv[1] << "\"." << endl;
      printUsage();
      return false;
   }

   // arguments
   if(mode == EXP_MODE)
   {
      if(argc != 7)
      {
         cerr << "*ERROR*: Incorrect argument list for exploration mode." << endl;
         printUsage();
         return false;
      }
      pExp = new Explorer;
      pExp->addSample(argv[2]);
      pExp->addSample(argv[3]);
      pExp->addSample(argv[4]);
      pExp->setEncFile(argv[5]);
      pExp->setSatFile(argv[6]);
   }
   else  // mode == GEN_MODE
   {
      if(argc != 5)
      {
         cerr << "*ERROR*: Incorrect argument list for generation mode." << endl;
         printUsage();
         return false;
      }
      pGen = new Generator;
      pGen->setEncFile(argv[2]);
      pGen->setTestFile(argv[3]);
      pGen->setCnfFile(argv[4]);
   }

   return true;
}

void printUsage()
{
   cout << "Usage:" << endl;
   cout << "   expcnf -exp <sample1.v> <sample2.v> <sample3.v>"
        << " <EncoderSetting> <SATSolverSetting>" << endl;
   cout << "   expcnf -gen <EncoderSetting> <test.v> <CnfOutput>" << endl;
}

void abcAlias()
{
   // ABC command aliases
   Cmd_CommandExecute(pAbc, "alias b balance");
   Cmd_CommandExecute(pAbc, "alias f fraig");
   Cmd_CommandExecute(pAbc, "alias fsto fraig_store");
   Cmd_CommandExecute(pAbc, "alias fres fraig_restore");
   Cmd_CommandExecute(pAbc, "alias rw rewrite");
   Cmd_CommandExecute(pAbc, "alias rwz rewrite -z");
   Cmd_CommandExecute(pAbc, "alias rf refactor");
   Cmd_CommandExecute(pAbc, "alias rfz refactor -z");
   Cmd_CommandExecute(pAbc, "alias st strash");
   Cmd_CommandExecute(pAbc, "alias resyn \"b; rw; rwz; b; rwz; b\"");
   Cmd_CommandExecute(pAbc, "alias resyn2 \"b; rw; rf; b; rw; rwz; b; rfz; rwz; b\"");
   Cmd_CommandExecute(pAbc, "alias compress \"b -l; rw -l; rwz -l; b -l; rwz -l; b -l\"");
   Cmd_CommandExecute(pAbc, "alias compress2 \"b -l; rw -l; rf -l; b -l; rw -l; rwz -l; b -l; rfz -l; rwz -l; b -l\"");
   Cmd_CommandExecute(pAbc, "alias choice \"fsto; resyn; fsto; resyn2; fsto; fres\"");
   Cmd_CommandExecute(pAbc, "alias choice2 \"fsto; b; fsto; resyn; fsto; resyn2; fsto; resyn2; fsto; fres\"");
   Cmd_CommandExecute(pAbc, "alias drwsat2 \"st; drw; b -l; drw; drf; ifraig -C 20; drw; b -l; drw; drf\"");

   // initialize cmdList
   //    cmdList[0]: resyn2    resyn2    resyn2    resyn2    drwsat2
   //    cmdList[1]: compress2 compress2 compress2 compress2 (none)
   for(int i = 0; i < 4; ++i)
   {
      cmdList[0].push_back("resyn2");
      cmdList[1].push_back("compress2");
   }
   cmdList[0].push_back("drwsat2");
   cmdList[1].push_back("");
}

