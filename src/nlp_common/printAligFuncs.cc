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
/* Module: printAligFuncs                                           */
/*                                                                  */
/* Definitions file: printAligFuncs.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files -------------------------------------

#include "printAligFuncs.h"

//--------------- Function definitions


//-------------------------
void printAlignmentInGIZAFormat(ostream &outS,
                                const Vector<string> &ns,
                                const Vector<string> &t,
                                WordAligMatrix waMatrix,
                                const char *header)
{
  unsigned int i,j,n;
	
  outS<<header<<endl;
  for(i=0;i<t.size();++i)
  {
    if(i<t.size()-1) outS<<t[i]<<" ";
    else outS<<t[i]; 
  }
  outS<<endl;
  
  for(i=0;i<ns.size();++i)
  {
    outS<<ns[i]<<" ({ ";
    for(j=0;j<waMatrix.get_J();++j) 
    {
      if(i==0)
      {
        if(!waMatrix.jAligned(j)) outS<<j+1<<" ";
      }
      else
      {
        if(waMatrix.getValue(i-1,j)!=0)
        {
          for(n=0;n<waMatrix.getValue(i-1,j);++n) outS<<j+1<<" ";
        }
      }
    }
    outS<<"}) ";	  
  }
  outS<<endl; 
}

//-------------------------
void printAlignmentInMyFormat(ostream &outS,
                              const Vector<string> &ns,
                              const Vector<string> &t,
                              WordAligMatrix waMatrix,
                              unsigned int numReps/*=1*/)
{
  unsigned int i;
	
  outS<<"# "<<numReps<<endl;
  for(i=0;i<t.size();++i)
  {
    if(i<t.size()-1) outS<<t[i]<<" ";
    else outS<<t[i]; 
  }
  outS<<endl;
  
  for(i=0;i<ns.size();++i)
  {
    if(i<ns.size()-1) outS<<ns[i]<<" ";
    else outS<<ns[i]<<endl; 	 
  }
  outS<<waMatrix;	
}
//-------------------------
void printAlignmentInGIZAFormat(FILE* outf,
                                const Vector<string> &ns,
                                const Vector<string> &t,
                                WordAligMatrix waMatrix,
                                const char *header)
{
  unsigned int i,j,n;
	
  fprintf(outf,"%s\n",header);
  for(i=0;i<t.size();++i)
  {
    if(i<t.size()-1) fprintf(outf,"%s ",t[i].c_str());
    else fprintf(outf,"%s",t[i].c_str()); 
  }
  fprintf(outf,"\n");
  
  for(i=0;i<ns.size();++i)
  {
    fprintf(outf,"%s ({ ",ns[i].c_str());
    for(j=0;j<waMatrix.get_J();++j) 
    {
      if(i==0)
      {
        if(!waMatrix.jAligned(j)) fprintf(outf,"%d ",j+1);
      }
      else
      {
        if(waMatrix.getValue(i-1,j)!=0)
        {
          for(n=0;n<waMatrix.getValue(i-1,j);++n) fprintf(outf,"%d ",j+1);
        }
      }
    }
    fprintf(outf,"}) ");
  }
  fprintf(outf,"\n");
}
//-------------------------
void printAlignmentInMyFormat(FILE* outf,
                              const Vector<string> &ns,
                              const Vector<string> &t,
                              WordAligMatrix waMatrix,
                              unsigned int numReps/*=1*/)
{
  unsigned int i;

  fprintf(outf,"# %d\n",numReps);
  for(i=0;i<t.size();++i)
  {
    if(i<t.size()-1) fprintf(outf,"%s ",t[i].c_str());
    else fprintf(outf,"%s",t[i].c_str());
  }
  fprintf(outf,"\n");
          
  for(i=0;i<ns.size();++i)
  {
    if(i<ns.size()-1) fprintf(outf,"%s ",ns[i].c_str());
    else fprintf(outf,"%s\n",ns[i].c_str());
  }
  waMatrix.print(outf);	  
}
