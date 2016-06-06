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
/* Module: CachedHmmAligLgProbVit.h                                 */
/*                                                                  */
/* Prototype file: CachedHmmAligLgProbVit                           */
/*                                                                  */
/* Description: Manages an IBM translation table.                   */
/*                                                                  */
/********************************************************************/

#ifndef _CachedHmmAligLgProbVit
#define _CachedHmmAligLgProbVit

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "Prob.h"
#include "SwDefs.h"

//--------------- Constants ------------------------------------------

#define CACHED_HMM_ALIG_LGPROB_VIT_INVALID_VAL 99

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- CachedHmmAligLgProbVit class

class CachedHmmAligLgProbVit
{
 public:
  bool isDefined(PositionIndex prev_i,
                 PositionIndex slen,
                 PositionIndex i);
  void set(PositionIndex prev_i,
           PositionIndex slen,
           PositionIndex i,
           LgProb lp);
  LgProb get(PositionIndex prev_i,
             PositionIndex slen,
             PositionIndex i);
  
 private:
  Vector<Vector<Vector<LgProb> > > cachedLgProbs;
  
};

#endif
