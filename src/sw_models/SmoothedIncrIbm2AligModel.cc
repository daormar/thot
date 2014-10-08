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
/* Module: SmoothedIncrIbm2AligModel                                */
/*                                                                  */
/* Definitions file: SmoothedIncrIbm2AligModel.cc                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SmoothedIncrIbm2AligModel.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- SmoothedIncrIbm2AligModel class function definitions

//-------------------------   
Prob SmoothedIncrIbm2AligModel::pts(WordIndex s,
                                    WordIndex t)
{
  double uns_pts=unsmoothed_pts(s,t);
  if(uns_pts<=IBM2_PROB_THRESHOLD) return IBM2_PROB_THRESHOLD;
  else return uns_pts;
}

//-------------------------
Prob SmoothedIncrIbm2AligModel::aProb(PositionIndex j,
                                      PositionIndex slen,
                                      PositionIndex tlen,
                                      PositionIndex i)
{
  double uns_ap=unsmoothed_aProb(j,slen,tlen,i);
  if(uns_ap==0)
  {
        // if (j,slen,tlen) has never been seen, distribute probability
        // uniformly
    return (double) 1.0/(slen+1);
  }
  else
  {
        // if event has been seen, return unsmoothed probability
    return uns_ap;
  }
}
