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
/* Module: BpSet                                                    */
/*                                                                  */
/* Prototype file: BpSet.h                                          */
/*                                                                  */
/* Description: Defines the BpSet class, which stores a             */
/*              set of indices with a count corresponding to        */
/*              consistent bilingual phrase pairs.                  */
/*                                                                  */
/********************************************************************/

#ifndef _BpSet_h
#define _BpSet_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <myVector.h>
#include <PositionIndex.h>
#include "BpSetInfo.h"
#include <MathFuncs.h>

using namespace std;

//--------------- Constants ------------------------------------------
typedef Vector<BpSetInfo> BpSetInfoCont;

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- BpSet class

class BpSet
{
  public:

        // Constructor
    BpSet(void);

        // Basic functions
    void incrPair(PositionIndex x1,
                  PositionIndex x2,
                  PositionIndex y1,
                  PositionIndex y2,
                  float lc=0);
    void retrieveTrgPhrasesGivenSrc(PositionIndex x1,
                                    PositionIndex x2,
                                    BpSetInfoCont& trgPhrases)const;
    PositionIndex getx1Max(void)const;
    PositionIndex getx2Max(PositionIndex x1)const;
    void obtainUnion(const BpSet& b);
    
        // clear() function
    void clear(void);
    
  private:
    
    Vector<Vector<Vector<BpSetInfo> > > consPairs;
};

#endif
