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
/* Module: IncrHmmP0AligModel                                       */
/*                                                                  */
/* Prototype file: IncrHmmP0AligModel.h                             */
/*                                                                  */
/* Description: Defines the IncrHmmP0AligModel class.               */
/*              IncrHmmP0AligModel class allows to generate and     */
/*              access to the data of a Hmm statistical             */
/*              alignment model with fixed p0 probability.          */
/*                                                                  */
/********************************************************************/

#ifndef _IncrHmmP0AligModel_h
#define _IncrHmmP0AligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "IncrHmmAligModel.h"

//--------------- Constants ------------------------------------------

#define DEFAULT_HMM_P0 0.1

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrHmmP0AligModel class

class IncrHmmP0AligModel: public IncrHmmAligModel
{
  public:

      // Constructor
   IncrHmmP0AligModel();

      // Set hmm p0 value
   void set_hmm_p0(Prob _hmm_p0);
   
      // load function
   bool load(const char* prefFileName);
   
      // print function
   bool print(const char* prefFileName);

      // clear() function
   void clear(void);

  protected:

   Prob hmm_p0;

   bool loadHmmP0(const char *hmmP0FileName);
   bool printHmmP0(const char *hmmP0FileName);

   Vector<WordIndex> extendWithNullWordAlig(const Vector<WordIndex>& srcWordIndexVec);
   double unsmoothed_logaProb(PositionIndex prev_i,
                              PositionIndex slen,
                              PositionIndex i);
};

#endif
