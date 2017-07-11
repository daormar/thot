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
/* Module: thot_sort_bin_ilextable.cc                               */
/*                                                                  */
/* Definitions file: thot_sort_bin_ilextable.cc                     */
/*                                                                  */
/* Description: Sorts the parameters of an incremental lexical      */
/*              table.                                              */
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
//#include <stxxl.h>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------

struct Entry
{
  WordIndex s;
  WordIndex t;
  float numer;
  float denom;
};

struct SortBySrcAndTrg
{
  bool operator() (const Entry& a,
                   const Entry& b)const
    {
      if(b.s<a.s) return 0;
      if(a.s<b.s) return 1;

      if(b.t<a.t) return 0;
      if(a.t<b.t) return 1;

      return 0;
    }

  static Entry min_value()
    {
      Entry e;
      e.s=(std::numeric_limits<WordIndex>::min)();
      e.t=(std::numeric_limits<WordIndex>::min)();
      return e;
    }

  static Entry max_value()
    {
      Entry e;
      e.s=(std::numeric_limits<WordIndex>::max)();
      e.t=(std::numeric_limits<WordIndex>::max)();
      return e;
    }
};

//--------------- Global variables -----------------------------------

std::string ilextableFileName;

//--------------- Function Definitions -------------------------------


//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==THOT_OK)
  {
        // Try to open file  
    ifstream inF (ilextableFileName.c_str(), ios::in | ios::binary);
    if (!inF)
    {
      cerr<<"Error in file with incremental lexical table, file "<<ilextableFileName<<" does not exist.\n";
      return THOT_ERROR;    
    }
    else
    {
          // Read registers
      std::vector<Entry> entryVec;
//      stxxl::vector<Entry> entryVec;
      bool end=false;
      while(!end)
      {
            // Read fields of entry
        Entry entry;
        if(inF.read((char*)&entry.s,sizeof(WordIndex)))
        {
          inF.read((char*)&entry.t,sizeof(WordIndex));
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
//      const unsigned M = 128*1024*1024;
//      stxxl::sort(entryVec.begin(),entryVec.end(),SortBySrcAndTrg(),M);
      
          // Print registers
      for(unsigned int i=0;i<entryVec.size();++i)
      {
//        printf("%d %d %g %g\n",entryVec[i].s,entryVec[i].t,entryVec[i].numer,entryVec[i].denom);
        cout.write((char*)&entryVec[i].s,sizeof(WordIndex));
        cout.write((char*)&entryVec[i].t,sizeof(WordIndex));
        cout.write((char*)&entryVec[i].numer,sizeof(float));
        cout.write((char*)&entryVec[i].denom,sizeof(float));
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
 err=readSTLstring(argc,argv, "-l", &ilextableFileName);
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
  printf("thot_sort_bin_ilextable written by Daniel Ortiz\n");
  printf("A tool to sort binary tables with lexical parameters\n");
  printf("type \"thot_sort_bin_ilextable --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_sort_bin_ilextable  -l <ilextable_file> [--help]\n\n");
  printf("-l <string>               File with the binary table of lexical parameters.\n");
  printf("--help                    Display this help and exit.\n\n");
}

//--------------------------------
