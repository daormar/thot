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
/* Module: SrcSegmLenTable                                          */
/*                                                                  */
/* Prototype file: SrcSegmLenTable.h                                */
/*                                                                  */
/* Description: Defines the SrcSegmLenTable class, which stores a   */
/*              probability table for the length of a source        */
/*              phrase.                                             */
/*                                                                  */
/********************************************************************/

#ifndef _SrcSegmLenTable_h
#define _SrcSegmLenTable_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "awkInputStream.h"
#include "Prob.h"

using namespace std;

//--------------- Constants ------------------------------------------

#define SRCSEGMLEN_UNIFORM       1
#define SRCSEGMLEN_GEOM          2
#define SRCSEGMLEN_GEOM_HIT_PROB 0.9

//--------------- typedefs -------------------------------------------

	 
//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- SrcSegmLenTable class

class SrcSegmLenTable
{
  public:

        // Constructor
    SrcSegmLenTable(void);

        // Functions to access model probabilities
    LgProb srcSegmLenLgProb(unsigned int x_k,
                            unsigned int x_km1,
                            unsigned int srcLen);
        // obtains the log-probability for the length of a source
        // segment log(p(x_k|x_{k-1},srcLen))
    
        // load function
    bool load(const char *srcSegmLenFileName);

        // clear function
    void clear(void);

  private:

    int mode;
    
    LgProb srcSegmLenLgProbUniform(unsigned int x_k,
                                   unsigned int x_km1,
                                   unsigned int srcLen);

    LgProb srcSegmLenLgProbGeom(unsigned int x_k,
                                unsigned int x_km1,
                                unsigned int srcLen);   
};

#endif
