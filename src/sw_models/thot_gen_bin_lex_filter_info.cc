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
/* Module: thot_gen_bin_lex_filter_info.cc                          */
/*                                                                  */
/* Definitions file: thot_gen_bin_lex_filter_info.cc                */
/*                                                                  */
/* Description: Extracts information to filter lexical parameters   */
/*              in binary format.                                   */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include "options.h"
#include "SwDefs.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

std::string lexTableFileName;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==THOT_OK)
  {
        // Try to open file  
    ifstream inF (lexTableFileName.c_str(), ios::in | ios::binary);
    if (!inF)
    {
      cerr<<"Error in file with incremental lexical table, file "<<lexTableFileName<<" does not exist.\n";
      return ERROR;    
    }
    else
    {
          // Read registers
      bool end=false;
      while(!end)
      {
            // Read fields of entry
        WordIndex s;
        WordIndex t;
        float numer;
        float denom;
        if(inF.read((char*)&s,sizeof(WordIndex)))
        {
          inF.read((char*)&t,sizeof(WordIndex));
          inF.read((char*)&numer,sizeof(float));
          inF.read((char*)&denom,sizeof(float));

//                     printf("%d %d\n",s,t);
          cout.write((char*)&s,sizeof(WordIndex));
          cout.write((char*)&t,sizeof(WordIndex));
        }
        else end=true;
      }
          // Close input file
      inF.close();      

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
 err=readSTLstring(argc,argv, "-l", &lexTableFileName);
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
  printf("thot_gen_bin_lex_filter_info written by Daniel Ortiz\n");
  printf("A tool to extract information to filter lexical parameters in binary format\n");
  printf("type \"thot_gen_bin_lex_filter_info --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_gen_bin_lex_filter_info  -l <lexTable_file> [--help]\n\n");
  printf("-l <lexTable_file>        File with the binary table of lexical parameters.\n");
  printf("--help                    Display this help and exit.\n\n");
}

//--------------------------------
