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
/* Module: thot_alig_op.cc                                          */
/*                                                                  */
/* Definitions file: thot_alig_op.cc                                */
/*                                                                  */
/* Description: performs operations over GIZA++ alignment files.    */
/*                                                                  */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "options.h"
#include "ctimer.h"
#include "AlignmentContainer.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

FILE* gen_temp_file(void);
void fileCopy (FILE* from, FILE *to);
int TakeParameters(int argc,char *argv[]);
bool parseAlignOpsFile(AlignmentContainer& alignmentContainer,
                       char * alignOperationsFile,
                       bool verbose);
bool parseAlignOpsFile(AlignmentExtractor& alignmentExtractor,
                       char * alignOperationsFile,
                       bool verbose);
void version(void);
void printUsage(void);
void printDesc(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

char alignOperationsFile[256];
char GizaAligFileName[256];
char outputFilesPrefix[256];
char GIZA_OpFileName[256];
int verbose,transposeFlag,gtFlag,exhaustive;
bool compactOutput,andOp,orOp,sumOp,symmetr1Op,symmetr2Op,growDiagFinalOp;

//--------------- Function Definitions -------------------------------

//--------------- main function

int main(int argc,char *argv[])
{
 char outputFileName[256];	
 AlignmentContainer alignmentContainer;	
 AlignmentExtractor alExt;
 
 if(TakeParameters(argc,argv)==0)
 {
   if(exhaustive) // exhaustive option given
   {
     if(alignmentContainer.extractAlignmentsFromGIZAFile(GizaAligFileName,gtFlag)==THOT_ERROR)
     {
       return THOT_ERROR;
     }
     else
     {
       if(alignOperationsFile[0]!=0)
         parseAlignOpsFile(alignmentContainer,alignOperationsFile,verbose);
       else
       {
         if(andOp) alignmentContainer.intersect(GIZA_OpFileName,transposeFlag);
         if(orOp) alignmentContainer.join(GIZA_OpFileName,transposeFlag);
         if(sumOp) alignmentContainer.sum(GIZA_OpFileName,transposeFlag);
         if(symmetr1Op) alignmentContainer.symmetr1(GIZA_OpFileName,transposeFlag);
         if(symmetr2Op) alignmentContainer.symmetr2(GIZA_OpFileName,transposeFlag);	   
         if(growDiagFinalOp) alignmentContainer.growDiagFinal(GIZA_OpFileName,transposeFlag);	   
       }

#      ifdef _GLIBCXX_USE_LFS
       ofstream outF;
       sprintf(outputFileName,"%s.A3.final",outputFilesPrefix); 
       outF.open(outputFileName,ios::out);
#      else
       FILE *outF;
       outF=fopen(outputFileName,"w");
#      endif
       if(!compactOutput)
       {
         alignmentContainer.printNoCompact(outF);
       }
       else
       {
#        ifdef _GLIBCXX_USE_LFS
         outF<<alignmentContainer;
#        else
         alignmentContainer.printCompact(outF);
#        endif
       }
       alignmentContainer.clear();
       return THOT_OK;
     }
   }
   else // exhaustive option not given
   {
     if(alExt.open(GizaAligFileName)==THOT_ERROR)
     {
       return THOT_ERROR;
     }
     else
     {
       sprintf(outputFileName,"%s.A3.final",outputFilesPrefix); 
       
       if(alignOperationsFile[0]!=0)
         return parseAlignOpsFile(alExt,alignOperationsFile,verbose);
       else
       {
         if(andOp) return alExt.intersect(GIZA_OpFileName,outputFileName,transposeFlag,verbose);
         if(orOp) return alExt.join(GIZA_OpFileName,outputFileName,transposeFlag,verbose);
         if(sumOp) return alExt.sum(GIZA_OpFileName,outputFileName,transposeFlag,verbose);
         if(symmetr1Op) return alExt.symmetr1(GIZA_OpFileName,outputFileName,transposeFlag,verbose);
         if(symmetr2Op) return alExt.symmetr2(GIZA_OpFileName,outputFileName,transposeFlag,verbose);	   
         if(growDiagFinalOp) return alExt.growDiagFinal(GIZA_OpFileName,outputFileName,transposeFlag,verbose);	   
       }
       return THOT_OK;
     } 
   }
 }	 
 else return THOT_ERROR;	
}

//--------------- parseAlignOpsFile function

bool parseAlignOpsFile(AlignmentContainer& alignmentContainer,
                       char * alignOperationsFile,
                       bool verbose)
{
 awkInputStream awk;
 int transpose;
	
 if(awk.open(alignOperationsFile)==THOT_ERROR)
 {
   return THOT_ERROR;
 }
 else
 {
   unsigned int lineno=0;
   
   while(awk.getln())
   {
     ++lineno;
     if(awk.NF==3)
     {
       bool invalid_op=true;
       int ret=THOT_OK;
       transpose=atoi(awk.dollar(3).c_str());
       if(strcmp("-and",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-and "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
         ret=alignmentContainer.intersect((char*)awk.dollar(2).c_str(),transpose);
       }
       if(strcmp("-or",awk.dollar(1).c_str())==0)
       {
         
         invalid_op=false;
         if(verbose) cerr<<"-or "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentContainer.join((char*)awk.dollar(2).c_str(),transpose);	
       }
       if(strcmp("-sum",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-sum "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentContainer.sum((char*)awk.dollar(2).c_str(),transpose);	
       }
       if(strcmp("-sym1",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-sym1 "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentContainer.symmetr1((char*)awk.dollar(2).c_str(),transpose);	
       }
       if(strcmp("-sym2",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-sym2 "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentContainer.symmetr2((char*)awk.dollar(2).c_str(),transpose);	
       }
       if(strcmp("-grd",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-grd "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentContainer.growDiagFinal((char*)awk.dollar(2).c_str(),transpose);	
       }
       if(ret==THOT_ERROR)
       {
         cerr<<"Error while executing alignment operation"<<endl;
         return THOT_ERROR;
       }
       if(invalid_op) cerr<<"Warning! invalid operation at line "<<lineno<<endl;
     }
	 else
     {
       if(awk.NF!=0)
       {
         cerr<<"Error in alignment operations file\n";
         return THOT_ERROR;
       }
     } 
   }
   return THOT_OK;
 }
}

//--------------- parseAlignOpsFile overloaded function for AlignmentExtractor class

bool parseAlignOpsFile(AlignmentExtractor& alignmentExtractor,
                       char * alignOperationsFile,
                       bool verbose)
{
 awkInputStream awk;
 int transpose;
 char outputFileName[512];
 FILE *out_file=0;
 FILE *tmp_file=0;
 
 if(awk.open(alignOperationsFile)==THOT_ERROR)
 {
   return THOT_ERROR;
 }
 else
 {
   unsigned int lineno=0;
   sprintf(outputFileName,"%s.A3.final",outputFilesPrefix);
   while(awk.getln())
   {
         // Process the lines of alignOperationsFile
     ++lineno;
     if(awk.NF==3)
     {
       bool invalid_op=true;
       int ret=THOT_OK;
       transpose=atoi(awk.dollar(3).c_str());
       if(strcmp("-and",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-and "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentExtractor.intersect((char*)awk.dollar(2).c_str(),outputFileName,transpose);
       }
       if(strcmp("-or",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-or "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentExtractor.join((char*)awk.dollar(2).c_str(),outputFileName,transpose);
       }
       if(strcmp("-sum",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-sum "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentExtractor.sum((char*)awk.dollar(2).c_str(),outputFileName,transpose);
       }
       if(strcmp("-sym1",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-sym1 "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentExtractor.symmetr1((char*)awk.dollar(2).c_str(),outputFileName,transpose);
       }
       if(strcmp("-sym2",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-sym2 "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentExtractor.symmetr2((char*)awk.dollar(2).c_str(),outputFileName,transpose);
       }
       if(strcmp("-grd",awk.dollar(1).c_str())==0)
       {
         invalid_op=false;
         if(verbose) cerr<<"-grd "<<awk.dollar(2).c_str()<<" "<<transpose<<endl;
		 ret=alignmentExtractor.growDiagFinal((char*)awk.dollar(2).c_str(),outputFileName,transpose);
       }
       if(ret==THOT_ERROR)
       {
         cerr<<"thot_alig_op aborted due to errors in the given alignment operations file."<<endl;
         exit(THOT_ERROR);
       }
       if(invalid_op) cerr<<"Warning! invalid operation at line "<<lineno<<endl;
       
           // Close alignment extractor (this closes and removes the temporary file)
       alignmentExtractor.close();
           // Open output file
       out_file=fopen(outputFileName,"r");
       if(out_file==NULL)
       {
         cerr<<"Error: Output file "<<outputFileName<<" cannot be created."<<endl;
         exit(THOT_ERROR);
       }
           // Create new temporary file
       tmp_file=gen_temp_file();
           // Copy the result of the last operation to the temporary file
       fileCopy(out_file,tmp_file);
           // Close out_file and rewind the temporary file
       fclose(out_file);
       fseek(tmp_file,0L,SEEK_SET);
           // Open temporary file in order to perform a new operation
       alignmentExtractor.open_stream(tmp_file);
     }
	 else
     {
       if(awk.NF!=0)
       {
         cerr<<"Error in alignment operations file\n";
         return THOT_ERROR;
       }
     } 
   }
   return THOT_OK;
 }
}

//--------------- gen_temp_file function
FILE* gen_temp_file(void)
{
  FILE *tmp_file=tmpfile();

  if(tmp_file!=0)
    return tmp_file;
  else
  {
    cerr<<"Error: temporary file cannot be created!"<<endl;
    exit(THOT_ERROR);
  }
}

//--------------- fileCopy function
void fileCopy (FILE* from, FILE *to)
{
  char ch;
  
      // Copy file
  while(!feof(from))
  {
    ch = fgetc(from);
    if(ferror(from))
    {
      cerr<<"Error reading temporary file."<<endl;
      exit(1);
    }
    if(!feof(from)) fputc(ch, to);
    if(ferror(to))
    {
      cerr<<"Error writing temporary file."<<endl;
      exit(1);
    }
  }  
  fflush(to);
}

//--------------- TakeParameters function
int TakeParameters(int argc,char *argv[])
{
 int err;

 if(argc==1)
 {
   printDesc();
   return 1;   
 }

 /* Verify --help option */
 err=readOption(argc,argv, "--help");
 if(err!=-1)
 {
   printUsage();
   return 1;
 }

 /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return 1;
 }

 /* Takes the GIZA alignment file name */
 gtFlag=0;  
 err=readString(argc,argv, "-g", GizaAligFileName);
 if(err==-1)
 {
   err=readString(argc,argv, "-gt", GizaAligFileName);
   if(err==-1)   
   {
     printUsage();
     return 1;
   }
   else gtFlag=1;  
 }

 /* Takes the alignment-operations file name */
 err=readString(argc,argv, "-f", alignOperationsFile);
 if(err==-1)
 {
   alignOperationsFile[0]=0;  
 }

 /* Takes the output files prefix */
 err=readString(argc,argv, "-o", outputFilesPrefix);
 if(err==-1)
 {
   printUsage();
   return 1;
 }
 
 /* Verify -and option */
 err=readString(argc,argv, "-and", GIZA_OpFileName);
 andOp=true;  
 if(err==-1)
 {
   andOp=false; 
 }
 /* Verify -or option */
 err=readString(argc,argv, "-or", GIZA_OpFileName);
 orOp=true;  
 if(err==-1)
 {
   orOp=false; 
 }  
 /* Verify -sum option */
 err=readString(argc,argv, "-sum", GIZA_OpFileName);
 sumOp=true;  
 if(err==-1)
 {
   sumOp=false; 
 }
  
 /* Verify -sym1 option */
 err=readString(argc,argv, "-sym1", GIZA_OpFileName);
 symmetr1Op=true;  
 if(err==-1)
 {
   symmetr1Op=false; 
 }
  
 /* Verify -sym2 option */
 err=readString(argc,argv, "-sym2", GIZA_OpFileName);
 symmetr2Op=true;  
 if(err==-1)
 {
   symmetr2Op=false; 
 }

  /* Verify -grd option */
 err=readString(argc,argv, "-grd", GIZA_OpFileName);
 growDiagFinalOp=true;  
 if(err==-1)
 {
   growDiagFinalOp=false; 
 }

 /* Verify noCompact option */
 compactOutput=false;
   
 err=readOption(argc,argv, "-compact");
 if(err==0) compactOutput=true;
	 
 /* Verify transpose option */
 transposeFlag=1;
   
 err=readOption(argc,argv, "-no-transpose");
 if(err==0) transposeFlag=0;

 /* Verify exhaustive option */
 exhaustive=0;
   
 err=readOption(argc,argv, "-e");
 if(err==0) exhaustive=1;
	 
 /* Verify verbose option */
 verbose=0;
   
 err=readOption(argc,argv, "-v");
 if(err==0) verbose=1;
	 
 return 0;  
}

//--------------- printDesc() function
void printDesc(void)
{
  cerr<<"thot_alig_op written by Daniel Ortiz\n";
  cerr<<"thot_alig_op allows to operate alignment matrices\n";
  cerr<<"type \"thot_alig_op --help\" to get usage information.\n";
}

//--------------------------------
void printUsage(void)
{
 cerr<<"Usage: thot_alig_op {-g <string> | -gt <string>} \n";
 cerr<<"               {{-and|-or|-sum|-sym1|-sym2|-grd} <string>|\n";
 cerr<<"               -f <string>}\n";	
 cerr<<"               -o <string> [-no-transpose] [-e [-compact]] [-v]\n";
 cerr<<"               [--help] [--version]\n\n";
 cerr<<"-g <string> | -gt <string>\n";
 cerr<<"                             Name of the GIZA-alignment file name.\n";
 cerr<<"                             If -gt, the alignment matrices are transposed.\n\n";
 cerr<<"-and | -or | -sum | -sym1 | -sym2 | -grd <string>\n";
 cerr<<"                             performs and, or, sum, sym1, sym2 or grd\n";
 cerr<<"                             operations with the given GIZA file.\n";
 cerr<<"                             Note: sym1 and sym2 are two different versions\n";
 cerr<<"                             of the alignment symmetrization, and grd.\n";
 cerr<<"                             correspondes to grow-diag-final operation.\n\n";
 cerr<<"-f <string>                  Gives a file with a sequence of alignment\n";
 cerr<<"                             operations over the initial Giza file name.\n";	
 cerr<<"                             Operations: '-and|-or|-sum|-sym1|-sym2|-grd'\n";
 cerr<<"                             The file consists of a set of entries and must\n";
 cerr<<"                             have only one entry per line.\n";
 cerr<<"                             Format of each entry: \n";
 cerr<<"                             '<operation> <fileName> <transposeFlag=0|1>'\n\n";
 cerr<<"-o <string>                  Set output files prefix name.\n\n";
 cerr<<"-no-transpose                Do not transpose the alignment matrix when \n";
 cerr<<"                             using -and | -or | -sum etc.\n\n";	
 cerr<<"-e                           With this flag it is not necessary a one to one\n";
 cerr<<"                             alignment correspondence between files\n";
 cerr<<"                             because the alignments are operated exhaustively\n";
 cerr<<"                             (increases time and space complexity).\n\n";	
 cerr<<"-compact                     Generate the output in a compact format (it can\n";
 cerr<<"                             be applied only if -e option was given).\n\n";
 cerr<<"-v                           Verbose mode\n\n";
 cerr<<"--help                       Display this help and exit\n\n";
 cerr<<"--version                    Output version information and exit\n\n";

}

//--------------- version function

void version(void)
{
  cerr<<"thot_alig_op is part of the Thot toolkit\n";
  cerr<<"Thot version "<<THOT_VERSION<<endl;
  cerr<<"Thot is GNU software written by Daniel Ortiz\n";
}
