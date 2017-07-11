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
/* Module: AlignmentContainer                                       */
/*                                                                  */
/* Definitions file: AlignmentContainer.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "AlignmentContainer.h"
#include "PhraseDefs.h"

//--------------- AlignmentContainer class method definitions

//-------------------------
AlignmentContainer::AlignmentContainer(void)
{
 numAlignments=0;	
 GizaAligFileName[0]=0;	
}

//-------------------------
bool AlignmentContainer::extractAlignmentsFromGIZAFile(const char *_GizaAligFileName,
                                                       bool transpose/*=0*/)
{
 unsigned int j;
 Vector<string> ns,t;
 Vector<unsigned int> t_id,s_id;
 AlignmentExtractor alignmentExtractor;
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 AligInfo alInfo;
 Vector<AligInfo> alInfoVec;
	
 aligCont.clear();
 tVocab.clear(); sVocab.clear();
 tVocabInv.clear(); sVocabInv.clear();	
 numAlignments=0;
	
 if(alignmentExtractor.open(_GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   sprintf(GizaAligFileName,"%s",_GizaAligFileName);
	   
   while(alignmentExtractor.getNextAlignment())
   {
     if(transpose) alignmentExtractor.transposeAlig();
	 t=alignmentExtractor.get_t();
	 ns=alignmentExtractor.get_ns();
		
	 t_id=vecString2VecUnsigInt(t,tVocab,tVocabInv);
	 s_id=vecString2VecUnsigInt(ns,sVocab,sVocabInv);
		
	 alInfo.count_s_t_=1;	
	 alInfo.s=s_id;
	 alInfo.wordAligMatrix=alignmentExtractor.get_wamatrix();
	 alIter=aligCont.find(t_id);
	 if(alIter==aligCont.end())
     {
       alInfoVec.clear();
       alInfoVec.push_back(alInfo);	  
       aligCont[t_id]=alInfoVec;
     }
	 else
     {
       for(j=0;j<alIter->second.size();++j)
       {
         if(alIter->second[j].s==s_id)	 
         {
           ++alIter->second[j].count_s_t_;
           break;
         }
       }
       if(j==alIter->second.size()) alIter->second.push_back(alInfo);			
     }
#    ifdef THOT_STATS
	 ++numAlignments;
#    endif	
   }
 }
 	
 return THOT_OK;	
}

//-------------------------
bool AlignmentContainer::join(const char *_GizaAligFileName,bool transpose/*=0*/)
{
  AlignmentExtractor alExt;
  map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
  unsigned int i;	
	 
  if(alExt.open(_GizaAligFileName)==ERROR)
  {
    return ERROR;
  }
  else
  {
    while(alExt.getNextAlignment())
    {  
      if(transpose) alExt.transposeAlig();
      alIter=aligCont.find(vecString2VecUnsigInt(alExt.get_t(),tVocab,tVocabInv));
	
      if(alIter!=aligCont.end())
	  {
        for(i=0;i<alIter->second.size();++i)
        {
          if(vecString2VecUnsigInt(alExt.get_ns(),sVocab,sVocabInv)==alIter->second[i].s)
          {
            alIter->second[i].wordAligMatrix|=alExt.get_wamatrix();	
          }			
        }
	  }
    }
    alExt.close();
    return THOT_OK;
  }
}

//-------------------------
bool AlignmentContainer::intersect(const char *_GizaAligFileName,
                                   bool transpose/*=0*/)
{
 AlignmentExtractor alExt;
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment())
   {  
     if(transpose) alExt.transposeAlig();
     alIter=aligCont.find(vecString2VecUnsigInt(alExt.get_t(),tVocab,tVocabInv));
	
     if(alIter!=aligCont.end())
     {
	   for(i=0;i<alIter->second.size();++i)
	   {
		if(vecString2VecUnsigInt(alExt.get_ns(),sVocab,sVocabInv)==alIter->second[i].s)
		{
          alIter->second[i].wordAligMatrix&=alExt.get_wamatrix();	
		}			
	   }
     }
   }
   alExt.close();
   return THOT_OK;
 }
}
//-------------------------
bool AlignmentContainer::sum(const char *_GizaAligFileName,bool transpose/*=0*/)
{
 AlignmentExtractor alExt;
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment())
   {  
	if(transpose) alExt.transposeAlig();
	alIter=aligCont.find(vecString2VecUnsigInt(alExt.get_t(),tVocab,tVocabInv));
	
	if(alIter!=aligCont.end())
    {
      for(i=0;i<alIter->second.size();++i)
      {
		if(vecString2VecUnsigInt(alExt.get_ns(),sVocab,sVocabInv)==alIter->second[i].s)
		{
          alIter->second[i].wordAligMatrix+=alExt.get_wamatrix();	
		}			
      }
    }
   }
   alExt.close();
   return THOT_OK;
 }
}

//-------------------------
bool AlignmentContainer::symmetr1(const char *_GizaAligFileName,
                                  bool transpose/*=0*/)
{
 AlignmentExtractor alExt;
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment())
   {  
     if(transpose) alExt.transposeAlig();
     alIter=aligCont.find(vecString2VecUnsigInt(alExt.get_t(),tVocab,tVocabInv));
	
     if(alIter!=aligCont.end())
     {		  
	   for(i=0;i<alIter->second.size();++i)
	   {
         if(vecString2VecUnsigInt(alExt.get_ns(),sVocab,sVocabInv)==alIter->second[i].s)
         {
           alIter->second[i].wordAligMatrix.symmetr1(alExt.get_wamatrix());	
         }			
	   }
     }
   }
   alExt.close();
   return THOT_OK;
 }
}

//-------------------------
bool AlignmentContainer::symmetr2(const char *_GizaAligFileName,
                                  bool transpose/*=0*/)
{
 AlignmentExtractor alExt;
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment())
   {  
     if(transpose) alExt.transposeAlig();
     alIter=aligCont.find(vecString2VecUnsigInt(alExt.get_t(),tVocab,tVocabInv));
	
     if(alIter!=aligCont.end())
     {		  
	   for(i=0;i<alIter->second.size();++i)
	   {
         if(vecString2VecUnsigInt(alExt.get_ns(),sVocab,sVocabInv)==alIter->second[i].s)
         {
           alIter->second[i].wordAligMatrix.symmetr2(alExt.get_wamatrix());	
         }			
	   }
     }
   }
   alExt.close();
   return THOT_OK;
 }
}

//-------------------------
bool AlignmentContainer::growDiagFinal(const char *_GizaAligFileName,
                                       bool transpose/*=0*/)
{
 AlignmentExtractor alExt;
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==ERROR)
 {
   return ERROR;
 }
 else
 {
   while(alExt.getNextAlignment())
   {  
     if(transpose) alExt.transposeAlig();
     alIter=aligCont.find(vecString2VecUnsigInt(alExt.get_t(),tVocab,tVocabInv));
	
     if(alIter!=aligCont.end())
     {		  
	   for(i=0;i<alIter->second.size();++i)
	   {
         if(vecString2VecUnsigInt(alExt.get_ns(),sVocab,sVocabInv)==alIter->second[i].s)
         {
           alIter->second[i].wordAligMatrix.growDiagFinal(alExt.get_wamatrix());	
         }			
	   }
     }
   }
   alExt.close();
   return THOT_OK;
 }
}

//-------------------------
Vector<unsigned int> AlignmentContainer::vecString2VecUnsigInt(Vector<string> vStr,
                                                               map<string,unsigned int> & vocab,
                                                               Vector<string> & vocabInv)const
{
 unsigned int i;
 Vector<unsigned int> vInt;
 map<string,unsigned int>::iterator vocabIter;
 unsigned int wIndex;
	
 for(i=0;i<vStr.size();++i)
 {
   vocabIter=vocab.find(vStr[i]);
	 
   if(vocabIter!=vocab.end())	 
     vInt.push_back(vocabIter->second);
   else
   {
     wIndex=vocab.size();
     vocab[vStr[i]]=wIndex;
     vocabInv.push_back(vStr[i]);
     vInt.push_back(wIndex);	  
   }
 }
 return vInt;
}

//-------------------------
Vector<string> AlignmentContainer::vecUnsigInt2VecString(Vector<unsigned int> vInt,
                                                         const Vector<string> & vocabInv)const
{
 unsigned int i;
 Vector<string> vStr;
	
 for(i=0;i<vInt.size();++i)
 {
   if(vInt[i]<vocabInv.size()) vStr.push_back(vocabInv[vInt[i]]);	 
 }
	
 return vStr;
}

//-------------------------
void AlignmentContainer::clear(void)
{
 aligCont.clear();
 tVocab.clear();
 sVocab.clear();
 tVocabInv.clear();
 sVocabInv.clear();	
}

//-------------------------
bool AlignmentContainer::printNoCompact(ostream &outS)
{
  map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
  Vector<AligInfo>::const_iterator eAlVectorIter;
  AlignmentExtractor alignmentExtractor;
  Vector<string> ns,t,s_cont,t_cont;
  Vector<unsigned int> t_ui;
  char cad[128];
  unsigned int numSent=0;
	
  if(alignmentExtractor.open(GizaAligFileName)==ERROR)
  {
    return ERROR;
  }
  else
  {	   
    while(alignmentExtractor.getNextAlignment())
	{
      t=alignmentExtractor.get_t();
      ns=alignmentExtractor.get_ns();
      t_ui=vecString2VecUnsigInt(t,tVocab,tVocabInv);	
      acIter=aligCont.find(t_ui);
      if(acIter!=aligCont.end())
      {
        for(eAlVectorIter=acIter->second.begin();eAlVectorIter!=acIter->second.end();++eAlVectorIter)
        {
		  s_cont=vecUnsigInt2VecString(eAlVectorIter->s,sVocabInv);	 
		  if(ns==s_cont)
		  {
            ++numSent; 
            t_cont=vecUnsigInt2VecString(acIter->first,tVocabInv);
            sprintf(cad,"# Sentence %d",numSent);	  
            printAlignmentInGIZAFormat(outS,ns,t,eAlVectorIter->wordAligMatrix,cad);  
		  }
        }
      }
	}
	return THOT_OK;
  } 
}
//-------------------------
bool AlignmentContainer::printNoCompact(FILE *file)
{
  map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
  Vector<AligInfo>::const_iterator eAlVectorIter;
  AlignmentExtractor alignmentExtractor;
  Vector<string> ns,t,s_cont,t_cont;
  Vector<unsigned int> t_ui;
  char cad[128];
  unsigned int numSent=0;
	
  if(alignmentExtractor.open(GizaAligFileName)==ERROR)
  {
    return ERROR;
  }
  else
  {	   
    while(alignmentExtractor.getNextAlignment())
	{
      t=alignmentExtractor.get_t();
      ns=alignmentExtractor.get_ns();
      t_ui=vecString2VecUnsigInt(t,tVocab,tVocabInv);	
      acIter=aligCont.find(t_ui);
      if(acIter!=aligCont.end())
      {
        for(eAlVectorIter=acIter->second.begin();eAlVectorIter!=acIter->second.end();++eAlVectorIter)
        {
		  s_cont=vecUnsigInt2VecString(eAlVectorIter->s,sVocabInv);	 
		  if(ns==s_cont)
		  {
            ++numSent; 
            t_cont=vecUnsigInt2VecString(acIter->first,tVocabInv);
            sprintf(cad,"# Sentence %d",numSent);	  
            printAlignmentInGIZAFormat(file,ns,t,eAlVectorIter->wordAligMatrix,cad);  
		  }
        }
      }
	}
	return THOT_OK;
  } 
}

//-------------------------
ostream& operator << (ostream &outS,
                      const AlignmentContainer &ac)
{
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
 Vector<AligInfo>::const_iterator eAlVectorIter;
 Vector<string> s,t;	
 char cad[128];
	
 for(acIter=ac.aligCont.begin();acIter!=ac.aligCont.end();++acIter)
 {
   for(eAlVectorIter=acIter->second.begin();eAlVectorIter!=acIter->second.end();++eAlVectorIter)
   {
     t=ac.vecUnsigInt2VecString(acIter->first,ac.tVocabInv);
     s=ac.vecUnsigInt2VecString(eAlVectorIter->s,ac.sVocabInv);
     sprintf(cad,"# %d",eAlVectorIter->count_s_t_);	 
     printAlignmentInGIZAFormat(outS,s,t,eAlVectorIter->wordAligMatrix,cad);
   } 
 }
 return outS;	
}
//-------------------------
void AlignmentContainer::printCompact(FILE *file)
{
 map<Vector<unsigned int>,Vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
 Vector<AligInfo>::const_iterator eAlVectorIter;
 Vector<string> s,t;	
 char cad[128];
	
 for(acIter=this->aligCont.begin();acIter!=this->aligCont.end();++acIter)
 {
   for(eAlVectorIter=acIter->second.begin();eAlVectorIter!=acIter->second.end();++eAlVectorIter)
   {
     t=this->vecUnsigInt2VecString(acIter->first,this->tVocabInv);
     s=this->vecUnsigInt2VecString(eAlVectorIter->s,this->sVocabInv);
     sprintf(cad,"# %d",eAlVectorIter->count_s_t_);	 
     printAlignmentInGIZAFormat(file,s,t,eAlVectorIter->wordAligMatrix,cad);
   } 
 }
}
