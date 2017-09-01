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
/* Module: NbestTableNode                                           */
/*                                                                  */
/* Prototype file: NbestTableNode                                   */
/*                                                                  */
/* Description: node for NbestTransTable template class.            */
/*                                                                  */
/********************************************************************/

#ifndef _NbestTableNode
#define _NbestTableNode

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StatModelDefs.h"
#include <utility>
#include <map>
#include <algorithm>
#include <vector>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- NbestTableNode class
template<class NODEDATA>
class NbestTableNode
{
 public:
  
  void insert(Score s,NODEDATA v);
  void insert(pair<Score,NODEDATA > scoreDataPair);
  NODEDATA getBestElem(void);
  Score getScoreOfBestElem(void);
  void removeLastElement(void);
  void pruneGivenThreshold(Score threshold);
  void stableSort(void);
  unsigned int size(void);
  void clear(void);
 
  // iterator
  class iterator;
  friend class iterator;
  class iterator
  {
   protected:
	   NbestTableNode<NODEDATA>* nttnodePtr;
	   typename multimap<Score,NODEDATA,greaterScore >::iterator mmapIter;
   public:
	   iterator(void){nttnodePtr=NULL;}
	   iterator(NbestTableNode<NODEDATA>* nttnode,
	            typename multimap<Score,NODEDATA,greaterScore >::iterator iter):nttnodePtr(nttnode)
       {
         mmapIter=iter;
       }  
	   bool operator++(void); //prefix
       bool operator++(int);  //postfix
	   int operator==(const iterator& right); 
	   int operator!=(const iterator& right); 
	   typename multimap<Score,NODEDATA,greaterScore >::iterator&
         operator->(void);
  };
 
  // NbestTableNode iterator-related functions
  iterator begin(void);
  iterator end(void);
   
 protected:
  multimap<Score,NODEDATA,greaterScore > tableNodeMultiMap;
};

//--------------- Template function definitions

//--------------------------
template<class NODEDATA>
void NbestTableNode<NODEDATA>::insert(Score s,NODEDATA v)
{
 pair<Score,NODEDATA > scoreDataPair;
	
 scoreDataPair.first=s;
 scoreDataPair.second=v;	
 tableNodeMultiMap.insert(scoreDataPair);	
}
//--------------------------
template<class NODEDATA>
void NbestTableNode<NODEDATA>::insert(pair<Score,NODEDATA > scoreDataPair)
{
 tableNodeMultiMap.insert(scoreDataPair);	
}
//--------------------------
template<class NODEDATA>
NODEDATA NbestTableNode<NODEDATA>::getBestElem(void)
{
 typename multimap<Score,NODEDATA,greaterScore >::iterator tableNodeIter;
 NODEDATA n;	

 tableNodeIter=tableNodeMultiMap.begin();
 if(tableNodeIter!=tableNodeMultiMap.end()) return tableNodeIter->second;
 else
 {
   return n;		
 }
}
//--------------------------
template<class NODEDATA>
Score NbestTableNode<NODEDATA>::getScoreOfBestElem(void)
{
 typename multimap<Score,NODEDATA,greaterScore >::iterator tableNodeIter;
 	
 tableNodeIter=tableNodeMultiMap.begin();
 if(tableNodeIter!=tableNodeMultiMap.end()) return tableNodeIter->first;
 else return 0;		
}
//--------------------------
template<class NODEDATA>
void NbestTableNode<NODEDATA>::removeLastElement(void)
{
 typename multimap<Score,NODEDATA,greaterScore >::iterator tableNodeIter;
	
 tableNodeIter=tableNodeMultiMap.end();
 --tableNodeIter;
 tableNodeMultiMap.erase(tableNodeIter--);	
}

//--------------------------
template<class NODEDATA>
void NbestTableNode<NODEDATA>::pruneGivenThreshold(Score threshold)
{
 typename multimap<Score,NODEDATA,greaterScore >::iterator tableNodeIter;

 if(tableNodeMultiMap.size()>0)
 {
   tableNodeIter=tableNodeMultiMap.end();
   --tableNodeIter;
   while(tableNodeMultiMap.size()>0)
   {
     if(tableNodeIter->first < threshold)
       tableNodeMultiMap.erase(tableNodeIter--);
     else break;
   }
 }
}

//--------------------------
template<class NODEDATA>
void NbestTableNode<NODEDATA>::stableSort(void)
{
 typename multimap<Score,NODEDATA,greaterScore >::iterator tableNodeIter;
 std::vector<pair<Score,NODEDATA> > vp;
 pair<Score,NODEDATA> p;
 unsigned int i;
 
 for(tableNodeIter=tableNodeMultiMap.begin();tableNodeIter!=tableNodeMultiMap.end();++tableNodeIter)
 {
   p.first=tableNodeIter->first;
   p.second=tableNodeIter->second;
   vp.push_back(p);
 }

 if(vp.size()>0)
 {
   tableNodeMultiMap.clear();
   sort(vp.begin(),vp.end());
   for(i=0;i<vp.size();++i)
   {
     tableNodeMultiMap.insert(vp[i]);
   }
 }
}

//--------------------------
template<class NODEDATA>
unsigned int NbestTableNode<NODEDATA>::size(void)
{
 return tableNodeMultiMap.size();	
}
//--------------------------
template<class NODEDATA>
void NbestTableNode<NODEDATA>::clear(void)
{
 tableNodeMultiMap.clear();	
}
//--------------------------
template<class NODEDATA>
typename NbestTableNode<NODEDATA>::iterator NbestTableNode<NODEDATA>::begin(void)
{
 typename NbestTableNode<NODEDATA>::iterator iter(this,tableNodeMultiMap.begin());
	
 return iter;
}
//--------------------------
template<class NODEDATA>
typename NbestTableNode<NODEDATA>::iterator NbestTableNode<NODEDATA>::end(void)
{
 typename NbestTableNode<NODEDATA>::iterator iter(this,tableNodeMultiMap.end());
	
 return iter;
}

// Iterator function definitions
//--------------------------
template<class NODEDATA>
bool NbestTableNode<NODEDATA>::iterator::operator++(void) //prefix
{
 if(nttnodePtr!=NULL)
 {
  ++mmapIter;
  if(mmapIter==nttnodePtr->tableNodeMultiMap.end()) return false;
  else return true;	 
 }
 else return false;
}
//--------------------------
template<class NODEDATA>
bool NbestTableNode<NODEDATA>::iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class NODEDATA>
int NbestTableNode<NODEDATA>::iterator::operator==(const iterator& right)
{
 return (nttnodePtr==right.nttnodePtr && mmapIter==right.mmapIter);	
}
//--------------------------
template<class NODEDATA>
int NbestTableNode<NODEDATA>::iterator::operator!=(const iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class NODEDATA>
typename multimap<Score,NODEDATA,greaterScore >::iterator&
NbestTableNode<NODEDATA>::iterator::operator->(void)
{
  return mmapIter;
}

#endif
