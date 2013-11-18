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
/* Module: NbSearchStack                                            */
/*                                                                  */
/* Definitions file: NbSearchStack.cc                               */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "NbSearchStack.h"

//--------------- NbSearchStack class function definitions

//---------------------------------------
NbSearchStack::NbSearchStack(void)
{
  maxStackSize=UNLIMITED_MAX_NBSTACK_SIZE;
}

//---------------------------------------
void NbSearchStack::push(Score scr,
                         NbSearchHyp nbsHyp)
{
  container.insert(make_pair(scr,nbsHyp));
  if(maxStackSize!=UNLIMITED_MAX_NBSTACK_SIZE)
  {
    while(container.size()>(unsigned int) maxStackSize)
    {
      removeLast();
    }
  }
}

//---------------------------------------
void NbSearchStack::removeLast(void)
{
  Container::iterator pos;

  if(!this->container.empty())
  {
    pos=this->container.end();
    --pos;
    this->container.erase(pos--);
  }  
}

//---------------------------------------
pair<Score,NbSearchHyp> NbSearchStack::top(void)
{
  return *container.begin();
}

//---------------------------------------
void NbSearchStack::pop(void)
{
  Container::iterator pos;
  pos=this->container.begin();
  this->container.erase(pos);
}

//---------------------------------------
void NbSearchStack::setMaxStackSize(int maxStackSize)
{
  maxStackSize=maxStackSize;
}

//---------------------------------------
bool NbSearchStack::empty(void)
{
  return container.size()==0;
}

//---------------------------------------
size_t NbSearchStack::size(void)
{
  return container.size();
}

//---------------------------------------
void NbSearchStack::clear(void)
{
  container.clear();
}

//---------------------------------------
NbSearchStack::~NbSearchStack()
{
}
