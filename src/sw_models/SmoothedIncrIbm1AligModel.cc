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
/* Module: SmoothedIncrIbm1AligModel                                */
/*                                                                  */
/* Definitions file: SmoothedIncrIbm1AligModel.cc                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SmoothedIncrIbm1AligModel.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- SmoothedIncrIbm1AligModel class function definitions

//-------------------------   
Prob SmoothedIncrIbm1AligModel::pts(WordIndex s,
                                    WordIndex t)
{
  double uns_pts=unsmoothed_pts(s,t);
  if(uns_pts<=IBM1_PROB_THRESHOLD) return IBM1_PROB_THRESHOLD;
  else return uns_pts;
}
