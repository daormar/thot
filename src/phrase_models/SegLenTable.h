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
/* Module: SegLenTable                                              */
/*                                                                  */
/* Prototype file: SegLenTable.h                                    */
/*                                                                  */
/* Description: Defines the SegLenTable class, which stores a       */
/*              probability table for the segmentation length of    */
/*              a sentence pair.                                    */
/*                                                                  */
/********************************************************************/

#ifndef _SegLenTable_h
#define _SegLenTable_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "PhraseDefs.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "awkInputStream.h"
#include "Prob.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

	 
//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- SegLenTable class

class SegLenTable
{
  public:

    SegLenTable(void);
    Prob pk_tlen(unsigned int tlen,unsigned int k); // Returns p(k|tlen)
    void constantSegmLengthTable(void);
    void uniformSegmLengthTable(void);
    void clear(void);

    void incrCountOf_tlenk(unsigned int tlen,unsigned int k);
    void incrCountOf_tlen(unsigned int tlen);

    bool printSegmLengthTable(char *outputFileName);
    bool load_seglentable(const char *segmLengthTableFileName);
	void printSegmLengthTable(std::ostream &outS);
  
  private:
    
	double segmLengthCount[MAX_SENTENCE_LENGTH][MAX_SENTENCE_LENGTH];
    double ksegmLengthCountMargin[MAX_SENTENCE_LENGTH];
  
};

#endif
