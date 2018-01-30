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

/**
 * @file AlignmentContainer.cc
 * 
 * @brief Definitions file for AlignmentContainer.h
 */

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
 std::vector<std::string> ns,t;
 std::vector<unsigned int> t_id,s_id;
 AlignmentExtractor alignmentExtractor;
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 AligInfo alInfo;
 std::vector<AligInfo> alInfoVec;
	
 aligCont.clear();
 tVocab.clear(); sVocab.clear();
 tVocabInv.clear(); sVocabInv.clear();	
 numAlignments=0;
	
 if(alignmentExtractor.open(_GizaAligFileName)==THOT_ERROR)
 {
   return THOT_ERROR;
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
  std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
  unsigned int i;	
	 
  if(alExt.open(_GizaAligFileName)==THOT_ERROR)
  {
    return THOT_ERROR;
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
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==THOT_ERROR)
 {
   return THOT_ERROR;
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
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==THOT_ERROR)
 {
   return THOT_ERROR;
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
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==THOT_ERROR)
 {
   return THOT_ERROR;
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
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==THOT_ERROR)
 {
   return THOT_ERROR;
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
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::iterator alIter;
 unsigned int i;	
	 
 if(alExt.open(_GizaAligFileName)==THOT_ERROR)
 {
   return THOT_ERROR;
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
std::vector<unsigned int> AlignmentContainer::vecString2VecUnsigInt(std::vector<std::string> vStr,
                                                               std::map<std::string,unsigned int> & vocab,
                                                               std::vector<std::string> & vocabInv)const
{
 unsigned int i;
 std::vector<unsigned int> vInt;
 std::map<std::string,unsigned int>::iterator vocabIter;
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
std::vector<std::string> AlignmentContainer::vecUnsigInt2VecString(std::vector<unsigned int> vInt,
                                                         const std::vector<std::string> & vocabInv)const
{
 unsigned int i;
 std::vector<std::string> vStr;
	
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
bool AlignmentContainer::printNoCompact(std::ostream &outS)
{
  std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
  std::vector<AligInfo>::const_iterator eAlVectorIter;
  AlignmentExtractor alignmentExtractor;
  std::vector<std::string> ns,t,s_cont,t_cont;
  std::vector<unsigned int> t_ui;
  char cad[128];
  unsigned int numSent=0;
	
  if(alignmentExtractor.open(GizaAligFileName)==THOT_ERROR)
  {
    return THOT_ERROR;
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
  std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
  std::vector<AligInfo>::const_iterator eAlVectorIter;
  AlignmentExtractor alignmentExtractor;
  std::vector<std::string> ns,t,s_cont,t_cont;
  std::vector<unsigned int> t_ui;
  char cad[128];
  unsigned int numSent=0;
	
  if(alignmentExtractor.open(GizaAligFileName)==THOT_ERROR)
  {
    return THOT_ERROR;
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
std::ostream& operator << (std::ostream &outS,
                           const AlignmentContainer &ac)
{
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
 std::vector<AligInfo>::const_iterator eAlVectorIter;
 std::vector<std::string> s,t;	
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
 std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion>::const_iterator acIter;
 std::vector<AligInfo>::const_iterator eAlVectorIter;
 std::vector<std::string> s,t;	
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
