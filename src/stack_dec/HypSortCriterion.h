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
 * @file HypSortCriterion.h
 * 
 * @brief Declares the HypSortCriterion template class.
 */


#ifndef _HypSortCriterion_h
#define _HypSortCriterion_h


//--------------- HypSortCriterion class

/**
 * @brief Sort criterion for sorting hypotheses in a stack.
 *
 */

template<class HYPOTHESIS>
class HypSortCriterion
{
 public:
  bool operator() (const HYPOTHESIS &h1,const HYPOTHESIS &h2)const
    {
      return h1.getScore() > h2.getScore();
    }
};

#endif
