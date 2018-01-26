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
 * @file NbestTransTable.h
 * 
 * @brief Stores and manages a n-best translation table.
 */

#ifndef _NbestTransTable
#define _NbestTransTable

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StatModelDefs.h"
#include "NbestTableNode.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- NbestTransTable class
template<class KEY,class NODEDATA>
class NbestTransTable
{
 public: 
 
    NbestTableNode<NODEDATA>* insertEntry(const KEY& k,
                                          const NbestTableNode<NODEDATA>& ttNode);
    NbestTableNode<NODEDATA>* getTranslationsForKey(const KEY& k);
 
    bool loadFromFile(char *tTableFileName);

    size_t size(void)const;
    void clear(void);

              // iterator
    class iterator;
    friend class iterator;
    class iterator
    {
      protected:
       NbestTransTable<KEY,NODEDATA>* ntPtr;
       typename std::map<KEY,NbestTableNode<NODEDATA> >::iterator tIter;
      public:
       iterator(void){ntPtr=NULL;}
       iterator(NbestTransTable<KEY,NODEDATA>* _ntPtr,
                typename std::map<KEY,NbestTableNode<NODEDATA> >::iterator _tIter)
       {
         ntPtr=_ntPtr;
         tIter=_tIter;
       }
       bool operator++(void); //prefix
       bool operator++(int);  //postfix
       int operator==(const iterator& right); 
       int operator!=(const iterator& right); 
       typename std::map<KEY,NbestTableNode<NODEDATA> >::iterator& operator->(void);
    };

    iterator begin(void);
    iterator end(void);

 protected:	 
    std::map<KEY,NbestTableNode<NODEDATA> > tTableMap; 
};

//--------------- Template function definitions

//-------------------------
template<class KEY,class NODEDATA>
NbestTableNode<NODEDATA>*
NbestTransTable<KEY,NODEDATA>::insertEntry(const KEY& key,
                                           const NbestTableNode<NODEDATA>& ttNode)
{
  std::pair<typename std::map<KEY,NbestTableNode<NODEDATA> >::iterator,bool> mapInsReturn;
 	
      // Insert translation table node into inverse translation table
  mapInsReturn=tTableMap.insert(std::make_pair(key,ttNode));
   
  return &mapInsReturn.first->second;
}
//-------------------------
template<class KEY,class NODEDATA>
NbestTableNode<NODEDATA>* NbestTransTable<KEY,NODEDATA>::getTranslationsForKey(const KEY& key)
{
  typename std::map<KEY,NbestTableNode<NODEDATA> >::iterator transTableIterator;
	
  transTableIterator=tTableMap.find(key);
  if(transTableIterator!=tTableMap.end())
  {// inverse translation present in the cache translation table
    return &transTableIterator->second;
  }
  else return NULL;	
}


//-------------------------
template<class KEY,class NODEDATA>
size_t NbestTransTable<KEY,NODEDATA>::size(void)const
{
  return tTableMap.size();
}

//-------------------------
template<class KEY,class NODEDATA>
void NbestTransTable<KEY,NODEDATA>::clear(void)
{
  tTableMap.clear();	
}

//-------------------------
template<class KEY,class NODEDATA>
typename NbestTransTable<KEY,NODEDATA>::iterator NbestTransTable<KEY,NODEDATA>::begin(void)
{
  typename NbestTransTable<KEY,NODEDATA>::iterator iter(this,tTableMap.begin());
  return iter;
}
//-------------------------
template<class KEY,class NODEDATA>
typename NbestTransTable<KEY,NODEDATA>::iterator NbestTransTable<KEY,NODEDATA>::end(void)
{
  typename NbestTransTable<KEY,NODEDATA>::iterator iter(this,tTableMap.end());
  return iter;		
}

  // Iterator function definitions
//--------------------------
template<class KEY,class NODEDATA>
bool NbestTransTable<KEY,NODEDATA>::iterator::operator++(void) //prefix
{
  if(ntPtr!=NULL)
  {
    ++tIter;
    if(tIter==ntPtr->tTableMap.end()) return false;
    else
    {
      return true;
    }
  }
  else return false;
}
//--------------------------
template<class KEY,class NODEDATA>
bool NbestTransTable<KEY,NODEDATA>::iterator::operator++(int)  //postfix
{
  return operator++();
}
//--------------------------
template<class KEY,class NODEDATA>
int NbestTransTable<KEY,NODEDATA>::iterator::operator==(const iterator& right)
{
  return (ntPtr==right.ntPtr && tIter==right.tIter);	
}
//--------------------------
template<class KEY,class NODEDATA>
int NbestTransTable<KEY,NODEDATA>::iterator::operator!=(const iterator& right)
{
  return !((*this)==right);
}
//--------------------------
template<class KEY,class NODEDATA>
typename std::map<KEY,NbestTableNode<NODEDATA> >::iterator&
NbestTransTable<KEY,NODEDATA>::iterator::operator->(void)
{
  return tIter;
}

#endif
