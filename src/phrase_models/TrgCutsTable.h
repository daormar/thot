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
/* Module: TrgCutsTable                                             */
/*                                                                  */
/* Prototype file: TrgCutsTable.h                                   */
/*                                                                  */
/* Description: Defines the TrgCutsTable class, which stores a      */
/*              probability table for the target phrase cuts.       */
/*                                                                  */
/********************************************************************/

#ifndef _TrgCutsTable_h
#define _TrgCutsTable_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "awkInputStream.h"
#include "Prob.h"
#include "PhraseDefs.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

//--------------- Constants ------------------------------------------

#define JUMP_ONE_POS_PAR        0.001
#define STOP_JUMPS_PAR          0.999

//--------------- typedefs -------------------------------------------

	 
//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- TrgCutsTable class

class TrgCutsTable
{
  public:

        // Constructor
    TrgCutsTable(void);

        // Functions to access model probabilities
    LgProb trgCutsLgProb(int offset);

        // load function
    bool load(const char *srcSegmLenFileName);

  private:

    float jumpOnePar;
    float stopJumps;
};

#endif
