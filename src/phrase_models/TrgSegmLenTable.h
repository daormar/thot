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
/* Module: TrgSegmLenTable                                          */
/*                                                                  */
/* Prototype file: TrgSegmLenTable.h                                */
/*                                                                  */
/* Description: Defines the SrcSegmLenTable class, which stores a   */
/*              probability table for the length of a target        */
/*              phrase.                                             */
/*                                                                  */
/********************************************************************/

#ifndef _TrgSegmLenTable_h
#define _TrgSegmLenTable_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "AwkInputStream.h"
#include "Prob.h"
#include <MathFuncs.h>

//--------------- Constants ------------------------------------------

#define TRGSEGMLEN_UNIFORM       1
#define TRGSEGMLEN_POISSON       2
#define TRGSEGMLEN_GEOM          3
#define TRGSEGMLEN_GEOM_HIT_PROB 0.9

//--------------- typedefs -------------------------------------------

	 
//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- TrgSegmLenTable class

class TrgSegmLenTable
{
  public:

        // Constructor
    TrgSegmLenTable(void);

        // Functions to access model probabilities
    LgProb trgSegmLenLgProb(unsigned int k,
                            const SentSegmentation& trgSegm,
                            unsigned int trgLen,
                            unsigned int lastSrcSegmLen);
        // obtains the log-probability for the length of a target
        // segment log(p(z_k|y_k,x_k-x_{k-1},trgLen))

        // load function
    bool load(const char *trgSegmLenFileName);

        // clear function
    void clear(void);

  private:

    int mode;

    double avgSrcSegmLen;
    double avgTrgSegmLen;

    LgProb trgSegmLenLgProbUniform(unsigned int k,
                                   const SentSegmentation& trgSegm,
                                   unsigned int trgLen,
                                   unsigned int lastSrcSegmLen);
    LgProb trgSegmLenLgProbPoisson(unsigned int k,
                                   const SentSegmentation& trgSegm,
                                   unsigned int trgLen,
                                   unsigned int lastSrcSegmLen);
    LgProb trgSegmLenLgProbGeom(unsigned int k,
                                const SentSegmentation& trgSegm,
                                unsigned int trgLen,
                                unsigned int lastSrcSegmLen);
    
    bool readAvgSegmLen(const char *avgSegmLenFileName);
};

#endif
