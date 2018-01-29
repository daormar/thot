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
 * @file SmtStack.h
 *
 * @brief The SmtStack class implements a stack to be used in stack
 * decoding.
 */

#ifndef _SmtStack_h
#define _SmtStack_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_smtStack.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- SmtStack template class

/**
 * @brief Simple statistical machine translation stack class.
 */
template<class HYPOTHESIS> 
class SmtStack: public _smtStack<HYPOTHESIS>
{
 public:

  typedef typename _smtStack<HYPOTHESIS>::Container Container;

      // iterator
  class iterator;
  friend class iterator;
  class iterator
  {
   protected:
    SmtStack<HYPOTHESIS>* smtstackPtr;
    typename Container::iterator msetIter;
   public:
    iterator(void){smtstackPtr=NULL;}
    iterator(SmtStack<HYPOTHESIS>* smtstack,
             typename Container::iterator iter):smtstackPtr(smtstack)
      {
        msetIter=iter;
      }  
    bool operator++(void); //prefix
    bool operator++(int);  //postfix
    int operator==(const iterator& right); 
    int operator!=(const iterator& right); 
    typename Container::iterator& operator->(void);
    HYPOTHESIS operator*(void)const;

        // SmtStack<HYPOTHESIS>::remove function declared as friend
    friend void SmtStack<HYPOTHESIS>::remove(SmtStack<HYPOTHESIS>::iterator iter);
  };
 
      // iterator-related functions
  iterator begin(void);
  iterator end(void);

      // basic functionality
  typename SmtStack<HYPOTHESIS>::iterator pushIter(const HYPOTHESIS& hyp);
      // push hyp into the stack. pushIter returns end() if the hyp was
      // not finally pushed into the stack
  bool push(const HYPOTHESIS& hyp);
  HYPOTHESIS pop(void);
  void remove(SmtStack<HYPOTHESIS>::iterator iter);
  void removeLast(void);
  
 protected:

};

//--------------- SmtStack template class function definitions


//---------------------------------------
template<class HYPOTHESIS> 
typename SmtStack<HYPOTHESIS>::iterator SmtStack<HYPOTHESIS>::pushIter(const HYPOTHESIS& hyp)
{
  if(this->maxStackSize==0) return end();
  else
  {
    HYPOTHESIS hyplast;

    while(this->container.size()>this->maxStackSize) removeLast();

    if(!this->empty())
    {
      hyplast=this->last();
      if(this->container.size()==this->maxStackSize &&
         (double)hyplast.getScore()>=(double)hyp.getScore())
      {
            // stack has reached its maximum size but the score of hyp is
            // worse than the score of the last hypothesis
#      ifdef THOT_STATS
        ++this->discardedPushOpsDueToSize;
#      endif

        return end();
      }
      else
      {
            // hyp is inserted and then the stack is pruned
        typename Container::iterator msetIt;
        
        msetIt=this->container.insert(hyp);
        typename SmtStack<HYPOTHESIS>::iterator ret(this,msetIt);
        if(this->container.size()>this->maxStackSize)
        {
          removeLast();
#        ifdef THOT_STATS
          ++this->discardedPushOpsDueToSize;
#        endif
        }
        return ret;
      }
    }
    else
    {
          // hyp is inserted, the stack does not need to be pruned
      typename Container::iterator msetIt;
    
      msetIt=this->container.insert(hyp);
      typename SmtStack<HYPOTHESIS>::iterator ret(this,msetIt);
      if(this->container.size()>this->maxStackSize) removeLast();
      return ret;
    }
  }
}

//---------------------------------------
template<class HYPOTHESIS> 
bool SmtStack<HYPOTHESIS>::push(const HYPOTHESIS& hyp)
{
  iterator smtsIter;

  smtsIter=pushIter(hyp);
  if(smtsIter==end()) return false;
  else return true;
}

//---------------------------------------
template<class HYPOTHESIS> 
HYPOTHESIS SmtStack<HYPOTHESIS>::pop(void)
{
  typename Container::iterator pos;
  HYPOTHESIS hyp;
  
  pos=this->container.begin();
  hyp=*pos;
  this->container.erase(pos);
  return hyp;
}

//---------------------------------------
template<class HYPOTHESIS> 
void SmtStack<HYPOTHESIS>::remove(SmtStack<HYPOTHESIS>::iterator iter)
{
  if(iter.smtstackPtr==this)
  {
    this->container.erase(iter.msetIter);
  }
}

//---------------------------------------
template<class HYPOTHESIS> 
void SmtStack<HYPOTHESIS>::removeLast(void)
{
  typename Container::iterator pos;

  if(!this->container.empty())
  {
    pos=this->container.end();
    --pos;
    this->container.erase(pos--);
  }  
}

//--------------------------
template<class HYPOTHESIS>
typename SmtStack<HYPOTHESIS>::iterator SmtStack<HYPOTHESIS>::begin(void)
{
 typename SmtStack<HYPOTHESIS>::iterator iter(this,this->container.begin());
	
 return iter;
}
//--------------------------
template<class HYPOTHESIS>
typename SmtStack<HYPOTHESIS>::iterator SmtStack<HYPOTHESIS>::end(void)
{
 typename SmtStack<HYPOTHESIS>::iterator iter(this,this->container.end());
	
 return iter;
}

// Iterator function definitions
//--------------------------
template<class HYPOTHESIS>
bool SmtStack<HYPOTHESIS>::iterator::operator++(void) //prefix
{
 if(smtstackPtr!=NULL)
 {
  ++msetIter;
  if(msetIter==smtstackPtr->container.end()) return false;
  else return true;	 
 }
 else return false;
}
//--------------------------
template<class HYPOTHESIS>
bool SmtStack<HYPOTHESIS>::iterator::operator++(int)  //postfix
{
 return operator++();
}
//--------------------------
template<class HYPOTHESIS>
int SmtStack<HYPOTHESIS>::iterator::operator==(const iterator& right)
{
 return (smtstackPtr==right.smtstackPtr && msetIter==right.msetIter);	
}
//--------------------------
template<class HYPOTHESIS>
int SmtStack<HYPOTHESIS>::iterator::operator!=(const iterator& right)
{
 return !((*this)==right);	
}
//--------------------------
template<class HYPOTHESIS>
typename SmtStack<HYPOTHESIS>::Container::iterator&
SmtStack<HYPOTHESIS>::iterator::operator->(void)
{
  return msetIter;
}
//--------------------------
template<class HYPOTHESIS>
HYPOTHESIS SmtStack<HYPOTHESIS>::iterator::operator*(void)const
{
   return *msetIter;
}

#endif
