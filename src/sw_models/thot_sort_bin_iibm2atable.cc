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
/* Module: thot_sort_bin_iibm2atable.cc                             */
/*                                                                  */
/* Definitions file: thot_sort_bin_iibm2atable.cc                   */
/*                                                                  */
/* Description: Sorts the parameters of a incremental ibm2          */
/*              alignment table.                                    */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include "options.h"
#include "SwDefs.h"
#include "aSource.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------

struct Entry
{
  aSource asIbm2;
  PositionIndex i;
  float numer;
  float denom;
};

struct SortBySrcAndTrg
{
  bool operator() (const Entry& a,
                   const Entry& b)const
    {
      if(b.asIbm2.j<a.asIbm2.j) return 0;
      if(a.asIbm2.j<b.asIbm2.j) return 1;

      if(b.asIbm2.slen<a.asIbm2.slen) return 0;
      if(a.asIbm2.slen<b.asIbm2.slen) return 1;

      if(b.asIbm2.tlen<a.asIbm2.tlen) return 0;
      if(a.asIbm2.tlen<b.asIbm2.tlen) return 1;

      if(b.i<a.i) return 0;
      if(a.i<b.i) return 1;

      return 0;
    }
};

//--------------- Global variables -----------------------------------

std::string iibm2atableFileName;

//--------------- Function Definitions -------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==THOT_OK)
  {
        // Try to open file  
    ifstream inF (iibm2atableFileName.c_str(), ios::in | ios::binary);
    if (!inF)
    {
      cerr<<"Error in file with incremental ibm2 alignment table, file "<<iibm2atableFileName<<" does not exist.\n";
      return ERROR;    
    }
    else
    {
          // Read registers
      std::vector<Entry> entryVec;
      bool end=false;
      while(!end)
      {
            // Read fields of entry
        Entry entry;
        if(inF.read((char*)&entry.asIbm2.j,sizeof(PositionIndex)))
        {
          inF.read((char*)&entry.asIbm2.slen,sizeof(PositionIndex));
          inF.read((char*)&entry.asIbm2.tlen,sizeof(PositionIndex));
          inF.read((char*)&entry.i,sizeof(PositionIndex));
          inF.read((char*)&entry.numer,sizeof(float));
          inF.read((char*)&entry.denom,sizeof(float));

          entryVec.push_back(entry);
        }
        else end=true;
      }

          // Close input file
      inF.close();
        
          // Sort registers
      std::sort(entryVec.begin(),entryVec.end(),SortBySrcAndTrg());
      
          // Print registers
      for(unsigned int i=0;i<entryVec.size();++i)
      {
//        printf("%d %d %d %d %g %g\n",entryVec[i].asIbm2.j,entryVec[i].asIbm2.slen,entryVec[i].asIbm2.tlen,entryVec[i].i,entryVec[i].numer,entryVec[i].denom);
        cout.write((char*)&entryVec[i].asIbm2.j,sizeof(PositionIndex));
        cout.write((char*)&entryVec[i].asIbm2.slen,sizeof(PositionIndex));
        cout.write((char*)&entryVec[i].asIbm2.tlen,sizeof(PositionIndex));
        cout.write((char*)&entryVec[i].i,sizeof(PositionIndex));
        cout.write((char*)&entryVec[i].numer,sizeof(float));
        cout.write((char*)&entryVec[i].denom,sizeof(float));            
      }
      
      return THOT_OK;
    }
  }
  else return ERROR;
}

//--------------- TakeParameters function
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

     /* Takes the table file name */
 err=readSTLstring(argc,argv, "-a", &iibm2atableFileName);
 if(err==-1)
 {
   printUsage();
   return ERROR;
 }

 return THOT_OK;  
}

//--------------- printDesc() function
void printDesc(void)
{
  printf("thot_sort_bin_iibm2atable written by Daniel Ortiz\n");
  printf("A tool to sort binary tables with ibm2 alignment parameters\n");
  printf("type \"thot_sort_bin_iibm2atable --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_sort_bin_iibm2atable  -a <string> [--help]\n\n");
  printf("-a <string>               File with the binary table of ibm2 alignment\n");
  printf("                          parameters.\n");
  printf("--help                    Display this help and exit.\n\n");
}

//--------------------------------
