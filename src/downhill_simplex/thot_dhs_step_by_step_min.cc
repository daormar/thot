/*
thot package
Written by Daniel Ortiz-Martinez
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
USA.
*/
 
/********************************************************************/
/*                                                                  */
/* Module: thot_dhs_step_by_step_min.cc                             */
/*                                                                  */
/* Definitions file: thot_dhs_step_by_step_min.cc                   */
/*                                                                  */
/* Description: thot_dhs_step_by_step_min is a program to minimize  */
/*              a given target function by means of a series of     */
/*              steps, thot_dhs_step_by_step_min uses a downhill    */
/*              simplex optimization algorithm that can be freely   */
/*              distributed.                                        */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include "ctimer.h"
#include "options.h"
#include "getline.h"
#include "StrProcUtils.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "ErrorDefs.h"
#include "step_by_step_dhs.h"

using namespace std;

//--------------- Constants ------------------------------------------

#define DEFAULT_FTOL   0.0001
#define SCALE_PAR      1

//--------------- Function Declarations ------------------------------

void genFixedVarsBoolVector(void);
void printVarVec(ostream &outS,double x[]);
Vector<double> xToVarVec(double x[]);
unsigned int getNumDims(void);
FILE* gen_temp_file(void);
void gen_init_sol(double x[]);
int getFirstNonBlankLine(char **lineptr, size_t *n, FILE *stream);
int TakeParameters(int argc,char *argv[]);
void version(void);
void printUsage(void);
void printDesc(void);

//--------------- Global variables -----------------------------------

unsigned int ndim;
double ftol;
Vector<string> fixNonFixVarsStr;
Vector<float> fixNonFixVars;
Vector<float> initVals;
Vector<bool> fixedVarsBoolVec;
std::string imagesFileName;
int verbosity;

//--------------- Function Definitions -------------------------------

//--------------- main function

int main(int argc,char *argv[])
{
  std::string s;
  Vector<string> v;	
    
  if(TakeParameters(argc,argv)==OK)
  {
        // init variables
    double* start=0;
    double* x=0;
    double y;
    int nfunk;
    int ret;
    FILE *images_file;

        // Generate boolean vector to distinguish fixed variables
    genFixedVarsBoolVector();

        // Open images file
    images_file=fopen(imagesFileName.c_str(),"r");
    if(images_file==NULL)
    {
      cerr<<"Error while opening file with images: "<<imagesFileName<<endl;
      return ERROR;
    }

    cerr<<"*** Executing step by step minimization..."<<endl;

        // Print value of ftol
    cerr<<"ftol: "<<ftol<<endl;

        // Set ndim value and allocate memory
    ndim=getNumDims();
    start=(double*) malloc(ndim*sizeof(double));
    x=(double*) malloc(ndim*sizeof(double));
      
        // Generate initial solution
    gen_init_sol(start);

        // Minimize variables
    cerr<<"Executing step by step downhill simplex algorithm..."<<endl;
    ret=step_by_step_simplex(start,
                             ndim,
                             ftol,
                             SCALE_PAR,
                             NULL,
                             images_file,
                             &nfunk,
                             &y,
                             x);

        // Check return code
    if(ret==DSO_EVAL_FUNC)
    {
      cerr<<"Image for x required!"<<endl;
      printVarVec(cout,x);
      return DSO_EVAL_FUNC;
    }
    if(ret==DSO_NMAX_ERROR)
    {
      cerr<<"Maximum number of function evaluations exceeded!"<<endl;
      printVarVec(cout,x);
      return DSO_NMAX_ERROR;
    }
    
    cerr<<"Solution ..."<<endl;
    printVarVec(cerr,start);
    printVarVec(cout,start);
    
        // Release memory
    free(start);
    free(x);
    fclose(images_file);
    
    return OK;
  }
  return ERROR;
}

//--------------- genFixedVarsBoolVector function
void genFixedVarsBoolVector(void)
{
      // Init fixed vars boolean vector
  for(unsigned int i=0;i<fixNonFixVars.size();++i)
  {
     //Check if the value for fixNonFixVarsStr[i] is equal to -0
    if(fixNonFixVars[i]==0 && fixNonFixVarsStr[i][0]=='-')
      fixedVarsBoolVec.push_back(false);
    else
    {
          // if fixNonFixVarsStr[i][0]!=-0 the weight is fixed
      fixedVarsBoolVec.push_back(true);
    }
  }
}

//--------------- printVarVec function
void printVarVec(ostream &outS,double x[])
{
  Vector<double> varVec=xToVarVec(x);
  for(unsigned int i=0;i<varVec.size();++i)
  {
    outS<<varVec[i]<<" ";
  }
  outS<<endl;
}

//--------------- xToVarVec function
Vector<double> xToVarVec(double x[])
{
  Vector<double> varVec;
  unsigned int idx=0;
  for(unsigned int i=0;i<fixedVarsBoolVec.size();++i)
  {
    if(!fixedVarsBoolVec[i])
    {
      varVec.push_back(x[idx]);
      ++idx;
    }
    else
      varVec.push_back(fixNonFixVars[i]);
  }
  return varVec;
}

//--------------- getNumDims function
unsigned int getNumDims(void)
{
  unsigned int nonFixedVars=0;
  
  for(unsigned int i=0;i<fixNonFixVars.size();++i)
  {
    if(!fixedVarsBoolVec[i])
      ++nonFixedVars;
  }
  return nonFixedVars;
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
    exit(ERROR);
  }
}

//--------------- gen_init_sol function
void gen_init_sol(double x[])
{
      // Generate initial solution
  if(initVals.empty())
  {
    for(unsigned int i=0;i<ndim;++i)
    {
      x[i]=(double)1/ndim;
    }    
  }
  else
  {
    unsigned int idx=0;
    for(unsigned int i=0;i<initVals.size();++i)
    {
      if(!fixedVarsBoolVec[i])
      {
        x[idx]=initVals[i];
        ++idx;
      }
    }
  }
  
      // Print initial variables
  cerr<<"Initial variables..."<<endl;
  Vector<double> varVec=xToVarVec(x);
  for(unsigned int i=0;i<varVec.size();++i)
  {
    cerr<<varVec[i]<<" ";
  }
  cerr<<endl<<endl;
}

//--------------- getFirstNonBlankLine function
int getFirstNonBlankLine(char **lineptr, size_t *n, FILE *stream)
{
  bool end=false;
  int read;
  
  while(!end)
  {
    read=getline(lineptr,n,stream);
    if(read>1 || read==-1)
      end=true;
  }
  return read;
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

 err=readOption(argc,argv,"--help");
 if(err!=-1)
 {
   printUsage();
   return ERROR;   
 }      

     /* Verify --version option */
 err=readOption(argc,argv, "--version");
 if(err!=-1)
 {
   version();
   return ERROR;
 }

     /* Take fixed and non-fixed variables */
 err=readStringSeq(argc,argv, "-va", fixNonFixVarsStr);
 if(err==-1)
 {
   version();
   return ERROR;
 }
 else
 {
   fixNonFixVars=StrProcUtils::strVecToFloatVec(fixNonFixVarsStr);
 }

     /* Take initial values for the variables if given */
 err=readFloatSeq(argc,argv, "-iv", initVals);
 if(err==-1)
 {
   initVals.clear();
 }
 else
 {
   if(fixNonFixVars.size()!=initVals.size())
   {
     cerr<<"Error: The number of values given by -va and -iv options are not equal."<<endl;
     return ERROR;
   }
 }

     // Take -i parameter 
 err=readSTLstring(argc,argv, "-i",&imagesFileName);
 if(err==-1)
 {
   cerr<<"Error: parameter -i not given!"<<endl;
   printUsage();
   return ERROR;   
 }

     /* Take the vocabulary file name */
 err=readDouble(argc,argv, "-ftol",&ftol);
 if(err==-1)
 {
   ftol=DEFAULT_FTOL;
 }

 err=readOption(argc,argv,"-v");
 verbosity=1;
 if(err==-1)
 {
   verbosity=0;
 }      

 return OK;
}

//--------------- printDesc() function
void printDesc(void)
{
  cerr << "thot_dhs_step_by_step_min written by Daniel Ortiz"<<endl;
  cerr << "thot_dhs_step_by_step_min allows to minimize a given target function."<<endl;
  cerr << "type \"thot_dhs_step_by_step_min --help\" to get usage information."<<endl;
}

//--------------------------------
void printUsage(void)
{
  cerr << "thot_dhs_step_by_step_min  -va <float> ... <float>"<<endl;
  cerr << "                           [-iv <float> ... <float>]"<<endl;
  cerr << "                           -i <string> [-ftol <float>]"<<endl;
  cerr << "                           [-v] [--help] [--version]"<<endl<<endl;
  cerr << " -va <float>...<float>: Set fixed and non-fixed variable values."<<endl;
  cerr << "                        The number of variables and their meaning depends"<<endl;
  cerr << "                        on the target function you want to minimize."<<endl;
  cerr << "                        Each value equal to -0 is considered a non fixed value."<<endl;
  cerr << " -iv <float>...<float>: Initial values for the variables (fixed values set by"<<endl;
  cerr << "                        -va are not affected by -iv)."<<endl;
  cerr << " -i <string>          : File with images of the target function for each step."<<endl;
  cerr << " -ftol <float>        : Fractional convergence tolerance"<<endl;
  cerr << "                        ("<<DEFAULT_FTOL<<" by default.)"<<endl;
  cerr << " -v                   : Verbose mode."<<endl;
  cerr << " --help               : Display this help and exit."<<endl;
  cerr << " --version            : Output version information and exit."<<endl;
}

//--------------------------------
void version(void)
{
  cerr<<"thot_dhs_step_by_step_min is part of the downhill package "<<endl;
  cerr<<"downhill version "<<THOT_VERSION<<endl;
  cerr<<"downhill is GNU software written by Daniel Ortiz"<<endl;
}
