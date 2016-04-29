#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iomanip>  // setw()
using namespace std;

#define CMD_NUM 6  // number of commands

bool exploreGlucose(const char *satFile, const vector<const char*>& cnfFiles)
{
    int best=0;
    int best_index=0;
    int samp_num = cnfFiles.size();
    //int samp_num = 1;
    char *cnf_in[]={"sample1.tmp.cnf","sample2.tmp.cnf","sample3.tmp.cnf"};
    char command[CMD_NUM*samp_num][200];
    int prog[CMD_NUM]={0};
    char line[200];
    fstream glu_setting;
    char cmd_buffer[200];

    for(int i=0;i<samp_num;i++){
            strcpy (cmd_buffer,"./glucose_exp ");
            strcat (cmd_buffer,cnf_in[i]);
            strcat (cmd_buffer," > case");
            strcpy (command[CMD_NUM*i],cmd_buffer);
            strcpy (cmd_buffer,"./glucose_exp ");
            strcat (cmd_buffer,cnf_in[i]);
            strcat (cmd_buffer," -K=0.75 > case");
            strcpy (command[CMD_NUM*i+1],cmd_buffer);
            strcpy (cmd_buffer,"./glucose_exp ");
            strcat (cmd_buffer,cnf_in[i]);
            strcat (cmd_buffer," -K=0.85 > case");
            strcpy (command[CMD_NUM*i+2],cmd_buffer);
            strcpy (cmd_buffer,"./glucose_exp ");
            strcat (cmd_buffer,cnf_in[i]);
            strcat (cmd_buffer," -R=1.5 > case");
            strcpy (command[CMD_NUM*i+3],cmd_buffer);
            strcpy (cmd_buffer,"./glucose_exp ");
            strcat (cmd_buffer,cnf_in[i]);
            strcat (cmd_buffer," -R=1.5 -K=0.75 > case");
            strcpy (command[CMD_NUM*i+4],cmd_buffer);
            strcpy (cmd_buffer,"./glucose_exp ");
            strcat (cmd_buffer,cnf_in[i]);
            strcat (cmd_buffer," -R=1.5 -K=0.85 > case");
            strcpy (command[CMD_NUM*i+5],cmd_buffer);
            }

    cout << " 0 / 18" << flush;
    for(int i=0;i<CMD_NUM*samp_num;i++){
            fstream fin_case;
            system(command[i]);
            fin_case.open("case",ios::in);
            while(fin_case.getline(line,sizeof(line),'\n'))
            {
             char *p;
             p = strtok(line," ");
             int count=0;
             while(p!=NULL){
                       if(count==3){
                                    count=0;
                                    //cout<<p<<" "<<flush;
                                    prog[i%CMD_NUM]+=atoi(p);
                                    }
                       if(count==2)count++;
                       //search parameter for progress
                       if(strcmp(p,"DL2")==0&&count==1)count++;
					   if(strcmp(p,"DL2")!=0&&count==1)count=0;
                       if(strcmp(p,"learnts")==0&&count==0)count++;
                       p = strtok(NULL," ");
                       }
            }
            //if(i%CMD_NUM==CMD_NUM-1) cout << endl;
            cout << "\b\b\b\b\b\b\b" << setw(2) << i+1 << " / " << CMD_NUM*samp_num << flush;
            fin_case.close();
            }
    cout << endl;


    remove("case");
    for(int i=0;i<samp_num;i++)
       remove(cnf_in[i]);

    //write setting file
    glu_setting.open(satFile,ios::out);
    glu_setting<<"./glucose ";

    for(int i=1;i<CMD_NUM;i++){
            if(prog[i]>best){
                             best=prog[i];
                             best_index=i;
                             }
            }

    switch(best_index){
                    case 0:glu_setting<<"using default setting"<<endl;
                         break;

                    case 1:glu_setting<<"-K=0.75"<<endl;
                         break;

                    case 2:glu_setting<<"-K=0.85"<<endl;
                         break;

                    case 3:glu_setting<<"-R=1.5"<<endl;
                         break;

                    case 4:glu_setting<<"-R=1.5 -K=0.75"<<endl;
                         break;

                    case 5:glu_setting<<"-R=1.5 -K=0.85"<<endl;
                         break;

             }
    glu_setting.close();

    return true;
}
