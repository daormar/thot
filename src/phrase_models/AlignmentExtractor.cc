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
/* Module: AlignmentExtractor                                       */
/*                                                                  */
/* Definitions file: AlignmentExtractor.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "AlignmentExtractor.h"

//--------------- AlignmentExtractor class method definitions

//-------------------------
AlignmentExtractor::AlignmentExtractor(void)
{
}

//-------------------------
bool AlignmentExtractor::open(const char *str,
                              unsigned int _fileFormat/*=GIZA_ALIG_FILE_FORMAT*/)
{
  FILE *stream;
  int ret;
  
  stream=fopen(str,"r");
  if(stream==NULL)
  {
    cerr<<"Error while opening file with alignments: "<<str<<endl;
    return ERROR;
  }

  ret=open_stream(stream,_fileFormat);

  return ret;
}

//-------------------------
bool AlignmentExtractor::open_stream(FILE *stream,
                                     unsigned int _fileFormat/*=GIZA_ALIG_FILE_FORMAT*/)
{
  fileFormat=_fileFormat;

  return awkInpStrm.open_stream(stream);
}

//-------------------------
void AlignmentExtractor::close(void)
{
 awkInpStrm.close();	
}

//-------------------------
bool AlignmentExtractor::rewind(void)
{
 return awkInpStrm.rewind();	
}

//-------------------------
Vector<string>& AlignmentExtractor::get_ns(void)
{
  return ns;
}

//-------------------------
Vector<string>& AlignmentExtractor::get_t(void)
{
  return t;
}

//-------------------------
WordAligMatrix& AlignmentExtractor::get_wamatrix(void)
{
  return wordAligMatrix;
}

//-------------------------
float AlignmentExtractor::get_numReps(void)
{
  return numReps;
}
//-------------------------
bool AlignmentExtractor::getNextAlignment(void)
{
 if(fileFormat==GIZA_ALIG_FILE_FORMAT) return getNextAlignInGIZAFormat();
 if(fileFormat==ALIG_OP_FILE_FORMAT) return getNextAlignInAlignOpFormat();
 return false;
}

//-------------------------
bool AlignmentExtractor::getNextAlignInGIZAFormat(void)
{
 unsigned int i,srcPos,trgPos,slen;
 
 ns.clear();
 t.clear();

     // Each alignment entry has three lines. The first line 
     // must start with the '#' symbol.
 if(awkInpStrm.getline())
 {
   if(awkInpStrm.NF>=1 && (strcmp("#",awkInpStrm.dollar(1).c_str())==0 || strcmp("<ALMOHADILLA>",awkInpStrm.dollar(1).c_str())==0))
   {
     if(awkInpStrm.NF>2) numReps=1;		   
     else
     {
       if(awkInpStrm.NF==1) numReps=1;
       else numReps=atof(awkInpStrm.dollar(2).c_str());
     }
        
     awkInpStrm.getline();
     for(i=1;i<=awkInpStrm.NF;++i)
     {
       t.push_back(awkInpStrm.dollar(i));
     }
		
     awkInpStrm.getline();
     i=1; slen=0;
     while(i<=awkInpStrm.NF)
     {
       if(strcmp("({",awkInpStrm.dollar(i).c_str())==0) ++slen;
       ++i;	 
     }
     i=1; srcPos=0;

     if(slen==0)
     {
       cerr<<"Error: GIZA alignment file corrupted!\n";
       cerr<<"Alignment extraction process aborted!\n";
       return false;
     }
     
     wordAligMatrix.init(slen-1,t.size());
     while(i<=awkInpStrm.NF)
     {
       std::string ew;
       bool opBraceFound;

       opBraceFound=false;
       ew=awkInpStrm.dollar(i);
       ++i;
       if(strcmp("({",awkInpStrm.dollar(i).c_str())==0) opBraceFound=true;
       while(i<=awkInpStrm.NF && strcmp("({",awkInpStrm.dollar(i).c_str())!=0)
       {
         ++i;
       }
       ++i;	
       while(i<=awkInpStrm.NF && strcmp("})",awkInpStrm.dollar(i).c_str())!=0)	
       {
         trgPos=atoi(awkInpStrm.dollar(i).c_str());
         
         if(trgPos-1>=t.size())
         {
           return 1;
         }
         else
         {
           if(srcPos>0 && (srcPos-1)<wordAligMatrix.get_I() && (trgPos-1)<wordAligMatrix.get_J())
           {
             unsigned int val=wordAligMatrix.getValue(srcPos-1,trgPos-1)+1;
             wordAligMatrix.setValue(srcPos-1,trgPos-1,val);
           }
         }
         ++i;	  
       }
       if(opBraceFound) ns.push_back(ew);
       else cerr<<"alig_op: Anomalous entry! (perhaps a problem with file codification?)\n";
       ++srcPos;	  
       ++i;	  
     }
     return true;
   }
   else return false;
 }
 else return false;
}

//-------------------------
bool AlignmentExtractor::getNextAlignInAlignOpFormat(void)
{
 unsigned int i,col,row;
	
 t.clear();
 ns.clear();
 
 wordAligMatrix.clear();
	
 if(awkInpStrm.getline())
 {
   if(awkInpStrm.NF==2 && strcmp("#",awkInpStrm.dollar(1).c_str())==0)
   {
     numReps=atof(awkInpStrm.dollar(2).c_str());	   
     awkInpStrm.getline();
     for(i=1;i<=awkInpStrm.NF;++i)
     {
       t.push_back(awkInpStrm.dollar(i));
     }
		
     awkInpStrm.getline();
     for(i=1;i<=awkInpStrm.NF;++i)
     {
       ns.push_back(awkInpStrm.dollar(i));
     }
		   
     wordAligMatrix.init(ns.size()-1,t.size());
     for(row=ns.size()-1;row>=1;--row)
     {
       awkInpStrm.getline();
       
       if(awkInpStrm.NF!=t.size()) return 0;
       else
       {
         for(col=1;col<=t.size();++col )
         {
           wordAligMatrix.setValue(row-1,
                                   col-1,
                                   atoi(awkInpStrm.dollar(col).c_str()));
         }
       }
     }
     return true;
   }
   else return false;
 }
 return false;
}

//-------------------------
void AlignmentExtractor::transposeAlig(void)
{
 Vector<string> aux;
 unsigned int i;
 std::string nullw;
 
 aux=t;
 t.clear();
 for(i=1;i<ns.size();++i)
 {
  t.push_back(ns[i]);
 }
 if(ns.size()>0) nullw=ns[0];
 ns.clear();
 ns.push_back(nullw);
 for(i=0;i<aux.size();++i)
 {
  ns.push_back(aux[i]);
 }
 wordAligMatrix.transpose();
}

//-------------------------
bool AlignmentExtractor::join(const char *GizaAligFileName,
                              const char *outFileName,
                              bool transpose,
                              bool verbose)
{
 AlignmentExtractor alExt;
 unsigned int numSent=0;
 
#ifdef _GLIBCXX_USE_LFS
 ofstream outF;
 outF.open(outFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#else
 FILE *outF;
 outF=fopen(outFileName,"w");
 if(outF==NULL)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#endif
 
 if(alExt.open(GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment() && getNextAlignment())
   {
     ++numSent;
     if(verbose) cerr<<"Operating sentence pair # "<<numSent<<endl;
     if(transpose) alExt.transposeAlig();
     if(t==alExt.t && ns==alExt.ns)
     {
       wordAligMatrix|=alExt.wordAligMatrix;	
     }
     else
     {
       cerr<<"Warning: sentences to operate are not equal!!!"<<" (Sent. pair:"<<numSent<<")"<<endl;
     }

     printAlignmentInGIZAFormat(outF);
   }
   alExt.close();
 }
 rewind();

 return OK;
}
//-------------------------
bool AlignmentExtractor::intersect(const char *GizaAligFileName,
                                   const char *outFileName,
                                   bool transpose,
                                   bool verbose)
{
 AlignmentExtractor alExt;
 unsigned int numSent=0;

#ifdef _GLIBCXX_USE_LFS
 ofstream outF;
 outF.open(outFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#else
 FILE *outF;
 outF=fopen(outFileName,"w");
 if(outF==NULL)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#endif
    
 if(alExt.open(GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment() && getNextAlignment())
   {
     ++numSent;
     if(verbose) cerr<<"Operating sentence pair # "<<numSent<<endl;

     if(transpose) alExt.transposeAlig();
     if(t==alExt.t && ns==alExt.ns)
     {
       wordAligMatrix&=alExt.wordAligMatrix;	
     }
     else
     {
       cerr<<"Warning: sentences to operate are not equal!!!"<<" (Sent. pair:"<<numSent<<")"<<endl;
     }

     printAlignmentInGIZAFormat(outF);
   }
   alExt.close(); 
 }
 rewind();

 return OK;
}
//-------------------------
bool AlignmentExtractor::sum(const char *GizaAligFileName,
                             const char *outFileName,
                             bool transpose,
                             bool verbose)
{
 AlignmentExtractor alExt;
 unsigned int numSent=0;

#ifdef _GLIBCXX_USE_LFS
 ofstream outF;
 outF.open(outFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#else
 FILE *outF;
 outF=fopen(outFileName,"w");
 if(outF==NULL)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#endif
 
 if(alExt.open(GizaAligFileName)==ERROR)
 {   
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment() && getNextAlignment())
   {
     ++numSent;
     if(verbose) cerr<<"Operating sentence pair # "<<numSent<<endl;

     if(transpose) alExt.transposeAlig();
     if(t==alExt.t && ns==alExt.ns)
     {
       wordAligMatrix+=alExt.wordAligMatrix;
     }
     else
     {
       cerr<<"Warning: sentences to operate are not equal!!!"<<" (Sent. pair:"<<numSent<<")"<<endl;
     }

     printAlignmentInGIZAFormat(outF);
   }
   alExt.close();
 }
 rewind();

 return OK;
}
//-------------------------
bool AlignmentExtractor::symmetr1(const char *GizaAligFileName,
                                  const char *outFileName,
                                  bool transpose,
                                  bool verbose)
{
 AlignmentExtractor alExt;
 unsigned int numSent=0;

#ifdef _GLIBCXX_USE_LFS
 ofstream outF;
 outF.open(outFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#else
 FILE *outF;
 outF=fopen(outFileName,"w");
 if(outF==NULL)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#endif
 
 if(alExt.open(GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment() && getNextAlignment())
   {
     ++numSent;
     if(verbose) cerr<<"Operating sentence pair # "<<numSent<<endl;

     if(transpose) alExt.transposeAlig();
     if(t==alExt.t && ns==alExt.ns)
     {
       wordAligMatrix.symmetr1(alExt.wordAligMatrix);	
     }
     else
     {
       cerr<<"Warning: sentences to operate are not equal!!!"<<" (Sent. pair:"<<numSent<<")"<<endl;
     }
     printAlignmentInGIZAFormat(outF);
   }
   alExt.close();
 }
 rewind();

 return OK;
}
//-------------------------
bool AlignmentExtractor::symmetr2(const char *GizaAligFileName,
                                  const char *outFileName,
                                  bool transpose,
                                  bool verbose)
{
 AlignmentExtractor alExt;
 unsigned int numSent=0;

#ifdef _GLIBCXX_USE_LFS
 ofstream outF;
 outF.open(outFileName,ios::out);
 if(!outF)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#else
 FILE *outF;
 outF=fopen(outFileName,"w");
 if(outF==NULL)
 {
   cerr<<"Error while opening output file."<<endl;
   return 1;
 }
#endif
 
 if(alExt.open(GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment() && getNextAlignment())
   {
     ++numSent;
     if(verbose) cerr<<"Operating sentence pair # "<<numSent<<endl;

     if(transpose) alExt.transposeAlig();
     if(t==alExt.t && ns==alExt.ns)
     {
       wordAligMatrix.symmetr2(alExt.wordAligMatrix);	
     }
     else
     {
       cerr<<"Warning: sentences to operate are not equal!!!"<<" (Sent. pair:"<<numSent<<")"<<endl;
     }

     printAlignmentInGIZAFormat(outF);
   }
   alExt.close();
 }
 rewind();

 return OK;
}
//-------------------------
void AlignmentExtractor::printAlignmentInGIZAFormat(ostream &outS)
{
  unsigned int i,j,n;
  string header;

  outS<<"# "<<numReps<<endl;
  for(i=0;i<t.size();++i)
  {
    if(i<t.size()-1) outS<<t[i]<<" ";
    else outS<<t[i]<<endl; 
  }
    
  for(i=0;i<ns.size();++i)
  {
    outS<<ns[i]<<" ({ ";
    for(j=0;j<wordAligMatrix.get_J();++j) 
    {
      if(i==0){if(!wordAligMatrix.jAligned(j)) outS<<j+1<<" ";}
      else
      {
        if(wordAligMatrix.getValue(i-1,j)!=0)
        {
          for(n=0;n<wordAligMatrix.getValue(i-1,j);++n) outS<<j+1<<" ";
        }
      }
    }
    outS<<"}) ";	  
  }
  outS<<endl; 
}
//-------------------------
void AlignmentExtractor::printAlignmentInGIZAFormat(FILE *file)
{
  unsigned int i,j,n;
  string header;

  fprintf(file,"# %f\n",numReps);

  for(i=0;i<t.size();++i)
  {
    if(i<t.size()-1) fprintf(file,"%s ",t[i].c_str());
    else fprintf(file,"%s\n",t[i].c_str());
  }
    
  for(i=0;i<ns.size();++i)
  {
    fprintf(file,"%s ({ ",ns[i].c_str());
    for(j=0;j<wordAligMatrix.get_J();++j) 
    {
      if(i==0)
      {
        if(!wordAligMatrix.jAligned(j)) fprintf(file,"%d ",j+1);
      }
      else
      {
        if(wordAligMatrix.getValue(i-1,j)!=0)
        {
          for(n=0;n<wordAligMatrix.getValue(i-1,j);++n) fprintf(file,"%d ",j+1);
        }
      }
    }
    fprintf(file,"}) ");
  }
  fprintf(file,"\n");
}

//-------------------------
AlignmentExtractor::~AlignmentExtractor()
{
}

//-------------------------
ostream& operator << (ostream &outS,AlignmentExtractor &ae)
{
 char cad[128];
 
 ae.rewind();
 while(ae.getNextAlignment())
 {
   sprintf(cad,"# %f",ae.get_numReps());	 
   printAlignmentInGIZAFormat(outS,ae.get_ns(),ae.get_t(),ae.get_wamatrix(),cad);	
 }
 ae.rewind();
   
 return outS;	
}
