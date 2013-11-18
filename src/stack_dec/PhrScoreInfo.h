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
 
#ifndef _PhrScoreInfo_h
#define _PhrScoreInfo_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "StackDecLmTypes.h"
#include "Prob.h"

//--------------- Classes --------------------------------------------

class PhrScoreInfo
{
  public:

   LgProb score;
  
       // Language model info
   LM_State lmHist;

   LgProb getScore(void)const;
   void addHeuristic(LgProb h);
   void sustractHeuristic(LgProb h);
};

#endif
