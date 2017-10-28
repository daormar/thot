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
/* Module: thot_sort_bin_ihmmatable.cc                              */
/*                                                                  */
/* Definitions file: thot_sort_bin_ihmmatable.cc                    */
/*                                                                  */
/* Description: Sorts the parameters of a incremental hmm           */
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
#include "aSourceHmm.h"

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------

struct Entry
{
  aSourceHmm asHmm;
  PositionIndex i;
  float numer;
  float denom;
};

struct SortBySrcAndTrg
{
  bool operator() (const Entry& a,
                   const Entry& b)const
    {
      if(b.asHmm.prev_i<a.asHmm.prev_i) return 0;
      if(a.asHmm.prev_i<b.asHmm.prev_i) return 1;

      if(b.asHmm.slen<a.asHmm.slen) return 0;
      if(a.asHmm.slen<b.asHmm.slen) return 1;

      if(b.i<a.i) return 0;
      if(a.i<b.i) return 1;

      return 0;
    }
};

//--------------- Global variables -----------------------------------

std::string ihmmatableFileName;

//--------------- Function Definitions -------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==THOT_OK)
  {
        // Try to open file  
    std::ifstream inF (ihmmatableFileName.c_str(), std::ios::in | std::ios::binary);
    if (!inF)
    {
      std::cerr<<"Error in file with incremental hmm alignment table, file "<<ihmmatableFileName<<" does not exist.\n";
      return THOT_ERROR;    
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
        if(inF.read((char*)&entry.asHmm.prev_i,sizeof(PositionIndex)))
        {
          inF.read((char*)&entry.asHmm.slen,sizeof(PositionIndex));
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
//        printf("%d %d %d %g %g\n",entryVec[i].asHmm.prev_i,entryVec[i].asHmm.slen,entryVec[i].i,entryVec[i].numer,entryVec[i].denom);
        std::cout.write((char*)&entryVec[i].asHmm.prev_i,sizeof(PositionIndex));
        std::cout.write((char*)&entryVec[i].asHmm.slen,sizeof(PositionIndex));
        std::cout.write((char*)&entryVec[i].i,sizeof(PositionIndex));
        std::cout.write((char*)&entryVec[i].numer,sizeof(float));
        std::cout.write((char*)&entryVec[i].denom,sizeof(float));            
      }
      
      return THOT_OK;
    }
  }
  else return THOT_ERROR;
}

//--------------- TakeParameters function
int TakeParameters(int argc,char *argv[])
{
 int err;

 if(argc==1)
 {
   printDesc();
   return THOT_ERROR;   
 }

     /* Verify --help option */
 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return THOT_ERROR;
 }

     /* Takes the table file name */
 err=readSTLstring(argc,argv, "-a", &ihmmatableFileName);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;
 }

 return THOT_OK;  
}

//--------------- printDesc() function
void printDesc(void)
{
  printf("thot_sort_bin_ihmmatable written by Daniel Ortiz\n");
  printf("A tool to sort binary tables with hmm alignment parameters\n");
  printf("type \"thot_sort_bin_ihmmatable --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_sort_bin_ihmmatable  -a <ihmmatable_file> [--help]\n\n");
  printf("-a <string>               File with the binary table of hmm alignment\n");
  printf("                          parameters.\n");
  printf("--help                    Display this help and exit.\n\n");
}

//--------------------------------
