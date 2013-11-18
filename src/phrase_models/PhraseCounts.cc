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
/* Module: PhraseCounts                                             */
/*                                                                  */
/* Definitions file: PhraseCounts                                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhraseCounts.h"

//--------------- Function definitions

//--------------- insert function specialization for the Trie class 
template<> TrieBid<WordIndex,Count>* TrieBid<WordIndex,Count>::insert(const Vector<WordIndex>& keySeq,
                                                                      const Count& d)
{
  unsigned int i;
  TrieBid<WordIndex,Count> *t,*newt;
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
          newt=new TrieBid<WordIndex,Count>;
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
        t->data=0;
        t->children=NULL;
      }
      else
      { // t is initialized, add new key to 'next' list
        newt=new TrieBid<WordIndex,Count>;
        t->next=newt;
        newt->father=t->father;
        t=newt;
        t->key=k;
        t->data=0;
        t->children=NULL;
      }
      if(i<keySeq.size()-1) // end of seq. not reached?
      {
        newt=new TrieBid<WordIndex,Count>;
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
PhraseCounts::PhraseCounts(void)
{
  numberOfPhrasesStored=0;
}
//-------------------------
PhraseCountState PhraseCounts::addPhrase(const Vector<WordIndex>& phrase,
                                         Count count)
{
  PhraseCountState pState;
  
  pState=pcTable.getState(phrase);
 
 if(pState!=NULL)
 {
   if((float)pState->getData()==0) 
   {
     ++numberOfPhrasesStored;
   }		
   pState->getData()=count;
 }
 else
 {
   pState=pcTable.insert(phrase,count);
   ++numberOfPhrasesStored;
 }
 return pState;
}
//-------------------------
PhraseCountState PhraseCounts::incrCountOfPhrase(const Vector<WordIndex>& phrase,
                                                 Count count)
{
  PhraseCountState pState;
  
  pState=pcTable.getState(phrase);
 
  if(pState!=NULL)
  {
    if((float)pState->getData()==0) 
    {
      ++numberOfPhrasesStored;
    }				  
    pState->getData()=pState->getData()+count;
  }
  else
  {
    pState=pcTable.insert(phrase,count);
    ++numberOfPhrasesStored;
  }
  return pState;
}
//-------------------------
Count PhraseCounts::getCount(const Vector<WordIndex>& phrase,
                             bool& found)
{
  Count* countPtr;

  countPtr=pcTable.find(phrase);

  if(countPtr!=NULL)
  {
    found=true;
    return *countPtr;
  }
  else
  {
    found=false;
    return 0;
  }
}
//-------------------------
PhraseCountState PhraseCounts::getState(const Vector<WordIndex>& phrase)
{
  return pcTable.getState(phrase);
}

//-------------------------
void PhraseCounts::getPhraseGivenState(PhraseCountState pState,
                                       Vector<WordIndex>& phrase)
{
  if(pState!=NULL)
  {
    pState->getKeySeq(phrase);
    phrase=invertVectorElements(phrase);
  }
}
//-------------------------
Count PhraseCounts::getCountGivenState(PhraseCountState pState)
{
  return pState->getData();  
}

//-------------------------
unsigned int PhraseCounts::size(void)
{
 return numberOfPhrasesStored;	
}
//-------------------------
unsigned int PhraseCounts::trieSize(void)
{
 return pcTable.size();	
}
//-------------------------
void PhraseCounts::clear(void)
{
 pcTable.clear();
 numberOfPhrasesStored=0;	
}
//-------------------------
Vector<WordIndex> PhraseCounts::invertVectorElements(const Vector<WordIndex>& v)
{Vector<WordIndex> inv;
 int i;

 inv.clear();
 for(i=v.size()-1;i>=0;--i)
 {inv.push_back(v[i]);
 }
 return inv; 
}

//-------------------------
