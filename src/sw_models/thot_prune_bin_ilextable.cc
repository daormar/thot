/*
thot package for statistical machine translation
Copyright (C) 2013 Daniel Ortiz-Mart\'inez
 
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with this program; If not, see <http://www.gnu.org/licenses/>.
*/
 
/********************************************************************/
/*                                                                  */
/* Module: thot_prune_bin_ilextable.cc                              */
/*                                                                  */
/* Definitions file: thot_prune_bin_ilextable.cc                    */
/*                                                                  */
/* Description: Prunes lexical parameters given a cutoff            */
/*              probability value.                                  */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include <algorithm>
#include "options.h"
#include <MathFuncs.h>
#include <myVector.h>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "SwDefs.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

void printCounts(WordIndex firstSrc,
                 Vector<WordIndex> trgWordVec,
                 float lcSrc,
                 Vector<float> lcSrcTrgVec);
int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------

struct TrgWordLogCount
{
  WordIndex trgWidx;
  float lcSrcTrg;
};

struct SortByLogCount
{
  bool operator() (const TrgWordLogCount& a,
                   const TrgWordLogCount& b)const
    {
      if(b.lcSrcTrg<a.lcSrcTrg) return 1;
      if(a.lcSrcTrg<b.lcSrcTrg) return 0;

      return 0;
    }
};
  
//--------------- Global variables -----------------------------------

std::string ilextableFileName;
unsigned int n_val;
float c_val;

//--------------- Function Definitions -------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==OK)
  {
        // Try to open file  
    ifstream inF (ilextableFileName.c_str(), ios::in | ios::binary);
    if (!inF)
    {
      cerr<<"Error in file with incremental lexical table, file "<<ilextableFileName<<" does not exist.\n";
      return ERROR;    
    }
    else
    {
          // Read registers
      bool end=false;
      bool first_entry=true;
      WordIndex firstSrc=0;
      float lcSrc=-99;
      Vector<WordIndex> trgWordVec;
      Vector<float> lcSrcTrgVec;
      
      while(!end)
      {
        WordIndex src;
        WordIndex trg;
        float numer;
        float denom;
        
        if(inF.read((char*)&src,sizeof(WordIndex)))
        {
              // Read fields
          inF.read((char*)&trg,sizeof(WordIndex));
          inF.read((char*)&numer,sizeof(float));
          inF.read((char*)&denom,sizeof(float));
        }
        else break;

            // verify if it is the first entry of the table
        if(first_entry==1)
        {
          firstSrc=src;
          first_entry=false;
          trgWordVec.push_back(trg);
          lcSrcTrgVec.push_back(numer);
          lcSrc=numer;
        }
        else
        {
              // a new source word has appeared?
          if(firstSrc!=src)
          {
                // print counts
            printCounts(firstSrc,trgWordVec,lcSrc,lcSrcTrgVec);

                // reset variables
            firstSrc=src;
            trgWordVec.clear();
            lcSrcTrgVec.clear();
            trgWordVec.push_back(trg);
            lcSrcTrgVec.push_back(numer);
            lcSrc=numer;
          }
          else
          {
            trgWordVec.push_back(trg);
            lcSrcTrgVec.push_back(numer);
            lcSrc=MathFuncs::lns_sumlog(lcSrc,lcSrcTrgVec.back());
          }
        }
      }
          // print last group of counts
      printCounts(firstSrc,trgWordVec,lcSrc,lcSrcTrgVec);

      return OK;
    }  
  }
  else return ERROR;
}

//--------------- printCounts() function
void printCounts(WordIndex firstSrc,
                 Vector<WordIndex> trgWordVec,
                 float lcSrc,
                 Vector<float> lcSrcTrgVec)
{
      // Sort counts for source word
  std::vector<TrgWordLogCount> trgWordLogCountVec;

  for(unsigned int n=0;n<trgWordVec.size();++n)
  {
    TrgWordLogCount trgWordLogCount;
    trgWordLogCount.trgWidx=trgWordVec[n];
    trgWordLogCount.lcSrcTrg=lcSrcTrgVec[n];
    trgWordLogCountVec.push_back(trgWordLogCount);
  }
  std::sort(trgWordLogCountVec.begin(),trgWordLogCountVec.end(),SortByLogCount());
      
      // Determine number of counts to print
  float newLcSrc=-99;
  unsigned int numFiltTrgWords=0;
  
  for(unsigned int n=0;n<trgWordLogCountVec.size();++n)
  {
    float prob=exp(trgWordLogCountVec[n].lcSrcTrg-lcSrc);
    if((numFiltTrgWords<n_val || n_val<=0) && prob>=c_val) 
    {
      newLcSrc=MathFuncs::lns_sumlog(newLcSrc,trgWordLogCountVec[n].lcSrcTrg);
      ++numFiltTrgWords;
    }
    else break;
  }
      // Print counts
  for(unsigned int n=0;n<numFiltTrgWords;++n)
  {
//    printf("%d %d %g %g %g\n",firstSrc,trgWordLogCountVec[n].trgWidx,trgWordLogCountVec[n].lcSrcTrg,newLcSrc,exp(trgWordLogCountVec[n].lcSrcTrg-newLcSrc));
    cout.write((char*)&firstSrc,sizeof(WordIndex));
    cout.write((char*)&trgWordLogCountVec[n].trgWidx,sizeof(WordIndex));
    cout.write((char*)&trgWordLogCountVec[n].lcSrcTrg,sizeof(float));
    cout.write((char*)&newLcSrc,sizeof(float));
  }
}

//--------------- TakeParameters() function
int TakeParameters(int argc,char *argv[])
{
 int err;

 if(argc==1)
 {
   printDesc();
   return ERROR;   
 }

     /* Verify --help option */
 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return ERROR;
 }

     /* Takes the model file name */
 err=readSTLstring(argc,argv, "-l", &ilextableFileName);
 if(err==-1)
 {
   printUsage();
   return ERROR;
 }

     /* Takes the -c option */
 err=readFloat(argc,argv, "-c", &c_val);
 if(err==-1)
 {
   printUsage();
   return ERROR;
 }

     /* Takes the -n option */
 err=readUnsignedInt(argc,argv, "-n", &n_val);
 if(err==-1)
 {
   printUsage();
   return ERROR;
 }

 return OK;  
}

//--------------- printDesc() function
void printDesc(void)
{
  printf("thot_prune_bin_ilextable written by Daniel Ortiz\n");
  printf("A tool to prune sorted binary tables with lexical parameters\n");
  printf("NOTE: this tool renormalizes the lexical parameters\n");
  printf("type \"thot_prune_bin_ilextable --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_prune_bin_ilextable  -l <string> -n <int>\n");
  printf("                            -c <float> [--help]\n\n");
  printf("-l <string>       File with the sorted binary table of lexical\n");
  printf("                          parameters.\n");
  printf("-n <int>                  Maximum number of translations per word.\n");
  printf("-c <float>                Cut-off probability.\n"); 
  printf("--help                    Display this help and exit.\n\n");
}

//--------------------------------
