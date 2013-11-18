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
/* Module: PhraseCountsLog                                          */
/*                                                                  */
/* Definitions file: PhraseCountsLog                                */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhraseCountsLog.h"

//--------------- Function definitions

//--------------- insert function specialization for the Trie class 
template<> TrieBid<WordIndex,LogCount>* TrieBid<WordIndex,LogCount>::insert(const Vector<WordIndex>& keySeq,
                                                                            const LogCount& d)
{
  unsigned int i;
  TrieBid<WordIndex,LogCount> *t,*newt;
  WordIndex k;

  if(keySeq.size()==0) return NULL;
  
  t=this;
 
  for(i=0;i<keySeq.size();++i) // for each position of the sequence...
  { 
    k=keySeq[i];
    
    while(t->next!=NULL && t->key!=k) // search the key 'k'
    {
      t=t->next; 
    }
    if(t!=t->children && t->key==k) // key 'k' was found?
    {
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        if(t->children!=NULL) t=t->children; // node t has children
        else
        { // node t has not any children
          newt=new TrieBid<WordIndex,LogCount>;
          newt->father=t;
          t->children=newt;
          t=newt;
        }
      }
      else t->data=d;
    }
    else
    { // 'k' was not found
      if(t->children==t) // t is uninitialized?
      {
        t->key=k;
        t->data=SMALL_LG_NUM;
        t->children=NULL;
      }
      else
      { // t is initialized, add new key to 'next' list
        newt=new TrieBid<WordIndex,LogCount>;
        t->next=newt;
        newt->father=t->father;
        t=newt;
        t->key=k;
        t->data=SMALL_LG_NUM;
        t->children=NULL;
      }
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        newt=new TrieBid<WordIndex,LogCount>;
        newt->father=t;
        t->children=newt;
        t=newt;
      }
      else
      { // end of seq. reached
        t->data=d;
      }
    }
  }
  return t;
}

//-------------------------
PhraseCountsLog::PhraseCountsLog(void)
{
  numberOfPhrasesStored=0;
}
//-------------------------
PhraseCountLogState PhraseCountsLog::addPhrase(const Vector<WordIndex>& phrase,
                                               LogCount logCount)
{
  PhraseCountLogState plState;
  
  plState=plcTable.getState(phrase);
 
 if(plState!=NULL)
 {
   if((float)plState->getData()==SMALL_LG_NUM) 
   {
     ++numberOfPhrasesStored;
   }		
   plState->getData()=logCount;
 }
 else
 {
   plState=plcTable.insert(phrase,logCount);
   ++numberOfPhrasesStored;
 }
 return plState;
}
//-------------------------
PhraseCountLogState PhraseCountsLog::incrLogCountOfPhrase(const Vector<WordIndex>& phrase,
                                                          LogCount logCount)
{
  PhraseCountLogState plState;
  
  plState=plcTable.getState(phrase);
 
  if(plState!=NULL)
  {
    if((float)plState->getData()==SMALL_LG_NUM) 
    {
      ++numberOfPhrasesStored;
    }				  
    plState->getData()=MathFuncs::lns_sumlog(plState->getData(),logCount);
  }
  else
  {
    plState=plcTable.insert(phrase,logCount);
    ++numberOfPhrasesStored;
  }
  return plState;
}
//-------------------------
LogCount PhraseCountsLog::getLogCount(const Vector<WordIndex>& phrase,
                                      bool& found)
{
  LogCount* lcountPtr;

  lcountPtr=plcTable.find(phrase);

  if(lcountPtr!=NULL)
  {
    found=true;
    return *lcountPtr;
  }
  else
  {
    found=false;
    return SMALL_LG_NUM;
  }
}
//-------------------------
PhraseCountLogState PhraseCountsLog::getState(const Vector<WordIndex>& phrase)
{
  return plcTable.getState(phrase);
}

//-------------------------
void PhraseCountsLog::getPhraseGivenState(PhraseCountLogState plState,
                                          Vector<WordIndex>& phrase)
{
  if(plState!=NULL)
  {
    plState->getKeySeq(phrase);
    phrase=invertVectorElements(phrase);
  }
}
//-------------------------
LogCount PhraseCountsLog::getLogCountGivenState(PhraseCountLogState plState)
{
  return plState->getData();  
}

//-------------------------
unsigned int PhraseCountsLog::size(void)
{
 return numberOfPhrasesStored;	
}
//-------------------------
unsigned int PhraseCountsLog::trieSize(void)
{
 return plcTable.size();	
}
//-------------------------
void PhraseCountsLog::clear(void)
{
 plcTable.clear();
 numberOfPhrasesStored=0;	
}
//-------------------------
Vector<WordIndex> PhraseCountsLog::invertVectorElements(const Vector<WordIndex>& v)
{Vector<WordIndex> inv;
 int i;

 inv.clear();
 for(i=v.size()-1;i>=0;--i)
 {inv.push_back(v[i]);
 }
 return inv; 
}

//-------------------------
