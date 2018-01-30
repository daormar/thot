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
 * @file SmoothedIncrIbm1AligModel.h
 * 
 * @brief Defines the SmoothedIncrIbm1AligModel class.
 * SmoothedIncrIbm1AligModel class inherits from the IncrIbm1AligModel
 * class and incorporates a simple smoothing technique.
 * 
 */

#ifndef _SmoothedIncrIbm1AligModel_h
#define _SmoothedIncrIbm1AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "IncrIbm1AligModel.h"

//--------------- Constants ------------------------------------------

#define IBM1_PROB_THRESHOLD   1e-8

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- SmoothedIncrIbm1AligModel class

class SmoothedIncrIbm1AligModel: public IncrIbm1AligModel
{
  public:

   typedef IncrIbm1AligModel::PpInfo PpInfo;
   typedef IncrIbm1AligModel::SrcTableNode SrcTableNode;
  
   // lexical model functions
   Prob pts(WordIndex s,WordIndex t);
       // returns p(t|s)
};

#endif
