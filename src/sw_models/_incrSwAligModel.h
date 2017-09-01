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
/* Module: _incrSwAligModel                                         */
/*                                                                  */
/* Prototype file: _incrSwAligModel.h                               */
/*                                                                  */
/* Description: Defines the _incrSwAligModel class.                 */
/*              _incrSwAligModel is a predecessor class for         */
/*              derivating single-word incremental statistical      */
/*              alignment models.                                   */
/*                                                                  */
/********************************************************************/

#ifndef __incrSwAligModel_h
#define __incrSwAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_swAligModel.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- _incrSwAligModel class

template<class PPINFO>
class _incrSwAligModel: public _swAligModel<PPINFO>
{
 public:

  typedef typename _swAligModel<PPINFO>::PpInfo PpInfo;

  virtual void set_expval_maxnsize(unsigned int _anji_maxnsize)=0;
      // Function to set a maximum size for the vector of expected
      // values anji (by default the size is not restricted)

  virtual void efficientBatchTrainingForRange(pair<unsigned int,unsigned int> sentPairRange,
                                              int verbosity=0);
  void efficientBatchTrainingForAllSents(int verbosity=0);
};

//--------------- _incrSwAligModel class method definitions

//-------------------------
template<class PPINFO>
void _incrSwAligModel<PPINFO>::efficientBatchTrainingForRange(pair<unsigned int,unsigned int> /*sentPairRange*/,
                                                              int/* verbosity=0*/)
{
  std::cerr<<"Warning: efficient batch training not implemented for this class.\n";
}

//-------------------------
template<class PPINFO>
void _incrSwAligModel<PPINFO>::efficientBatchTrainingForAllSents(int verbosity/*=0*/)
{
  efficientBatchTrainingForRange(make_pair(0,this->numSentPairs()-1),verbosity);
}

//-------------------------

#endif
