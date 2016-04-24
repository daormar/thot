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
/* Module: BaseWordPenaltyModel                                     */
/*                                                                  */
/* Prototype file: BaseWordPenaltyModel.h                           */
/*                                                                  */
/* Description: BaseWordPenaltyModel is a base class to implement   */
/*              word penalty models.                                */
/*                                                                  */
/********************************************************************/

#ifndef _BaseWordPenaltyModel
#define _BaseWordPenaltyModel

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include "Prob.h"
#include "myVector.h"
#include <map>
#include <utility>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- BaseWordPenaltyModel class

class BaseWordPenaltyModel
{
 public:
  
      // returns log(wp(tl=len(strVec)))
  virtual LgProb wordPenaltyScoreStr(Vector<std::string> strVec){return wordPenaltyScore(strVec.size());};

      // returns log(wp(tl=tlen))
  virtual LgProb wordPenaltyScore(unsigned int tlen)=0;

      // returns log(wp(tl>=len(strVec)))
  virtual LgProb sumWordPenaltyScoreStr(Vector<std::string> strVec) {return sumWordPenaltyScore(strVec.size());};

      // returns log(wp(tl>=tlen))
  virtual LgProb sumWordPenaltyScore(unsigned int tlen)=0;

      // clear function
  virtual void clear(void);

        // Destructor
  virtual ~BaseWordPenaltyModel(){};
};

#endif
