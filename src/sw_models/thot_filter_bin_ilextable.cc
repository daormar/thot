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
/* Module: thot_filter_bin_ilextable.cc                             */
/*                                                                  */
/* Definitions file: thot_filter_bin_ilextable.cc                   */
/*                                                                  */
/* Description: Filters lexical parameters given a lexical table    */
/*              and a file with filtering information generated     */
/*              by means of the gen_bin_lex_filter_info tool.       */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include <iostream>
#include <iomanip>
#include <fstream>
#include "options.h"
#include "SwDefs.h"

//--------------- Constants ------------------------------------------

#define RECORD_READ     0
#define NO_RECORDS_LEFT 1

//--------------- Function Declarations ------------------------------

int readTableRecord(ifstream& inF,
                    WordIndex& s,
                    WordIndex& t,
                    float& numer,
                    float& denom);
int readFiltInfoRecord(ifstream& inF,
                       WordIndex& s,
                       WordIndex& t);
int TakeParameters(int argc,char *argv[]);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------

//--------------- Global variables -----------------------------------

std::string ilextableFileName;
std::string filterInfoFileName;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc,char *argv[])
{
  if(TakeParameters(argc,argv)==THOT_OK)
  {
        // Try to open file with lexical table  
    ifstream lexTableInF (ilextableFileName.c_str(), ios::in | ios::binary);
    if (!lexTableInF)
    {
      std::cerr<<"Error in file with incremental lexical table, file "<<ilextableFileName<<" does not exist.\n";
      return THOT_ERROR;    
    }

    ifstream filtInfoInf (filterInfoFileName.c_str(), ios::in | ios::binary);
    if (!filtInfoInf)
    {
      std::cerr<<"Error in file with filtering information, file "<<filterInfoFileName<<" does not exist.\n";
      return THOT_ERROR;    
    }

        // Filter table registers
    bool end=false;
    WordIndex s;
    WordIndex t;
    WordIndex sFilt;
    WordIndex tFilt;
    float numer;
    float denom;

        // Read first record of file with filtering info
    int ret=readFiltInfoRecord(filtInfoInf,sFilt,tFilt);
    if(ret==NO_RECORDS_LEFT)
      end=true;
    
    while(!end)
    {
          // Read field of table
      int ret=readTableRecord(lexTableInF,s,t,numer,denom);
      if(ret==NO_RECORDS_LEFT)
        end=true;
      else
      {
        if(s==sFilt && t==tFilt)
        {
//          printf("%d %d %g %g\n",s,t,numer,denom);
          cout.write((char*)&s,sizeof(WordIndex));
          cout.write((char*)&t,sizeof(WordIndex));
          cout.write((char*)&numer,sizeof(float));
          cout.write((char*)&denom,sizeof(float));

          int ret=readFiltInfoRecord(filtInfoInf,sFilt,tFilt);
          if(ret==NO_RECORDS_LEFT)
            end=true;
        }
      } 
    }

        // Close input files
    lexTableInF.close();
    filtInfoInf.close();      
    
    return THOT_OK;
  }
  else return THOT_ERROR;
}

//--------------- readTableRecord function
int readFiltInfoRecord(ifstream& inF,
                       WordIndex& s,
                       WordIndex& t)
{
  if(inF.read((char*)&s,sizeof(WordIndex)))
  {
    inF.read((char*)&t,sizeof(WordIndex));
    return RECORD_READ;
  }
  else return NO_RECORDS_LEFT;
}

//--------------- readTableRecord function
int readTableRecord(ifstream& inF,
                    WordIndex& s,
                    WordIndex& t,
                    float& numer,
                    float& denom)
{
  if(inF.read((char*)&s,sizeof(WordIndex)))
  {
    inF.read((char*)&t,sizeof(WordIndex));
    inF.read((char*)&numer,sizeof(float));
    inF.read((char*)&denom,sizeof(float));
    return RECORD_READ;
  }
  else return NO_RECORDS_LEFT;
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

     /* Takes the name of the file with filtering information */
 err=readSTLstring(argc,argv, "-f", &filterInfoFileName);
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
  printf("thot_filter_bin_ilextable written by Daniel Ortiz\n");
  printf("Filters parameters given lexical table and a file with filtering information\n");
  printf("type \"thot_filter_bin_ilextable --help\" to get usage information.\n");
}

//--------------- printUsage() function
void printUsage(void)
{
  printf("Usage: thot_filter_bin_ilextable  -l <ilextable_file> -f <filter_file> [--help]\n\n");
  printf("-l <ilextable_file>       File with the binary table of lexical parameters.\n");
  printf("-f <filter_file>          File with filtering information generated using\n");
  printf("                          the \"gen_bin_lex_filter_info\" tool.\n");
  printf("--help                    Display this help and exit.\n\n");
}

//--------------------------------
