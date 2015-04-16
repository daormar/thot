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
/* Module: BaseEditDist                                             */
/*                                                                  */
/* Prototypes file: BaseEditDist.h                                  */
/*                                                                  */
/* Description: Defines the BaseEditDist class, an abstract class   */
/*              for derivating clases for calculating edit distance.*/
/*                                                                  */
/********************************************************************/

#ifndef _BaseEditDist_h
#define _BaseEditDist_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <Score.h>

//--------------- Constants ------------------------------------------

#define NONE_OP             -1
#define HIT_OP               0
#define HIT_OP_STR           "HIT"
#define INS_OP               1
#define INS_OP_STR           "INS"
#define DEL_OP               2
#define DEL_OP_STR           "DEL"
#define SUBST_OP             3
#define SUBST_OP_STR         "SUBST"
#define PREF_DEL_OP          4
#define DONT_USE_PREF_DEL_OP 0
#define USE_PREF_DEL_OP      1

//--------------- Type definitions -----------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseEditDist class declaration

template<class OBJ>
class BaseEditDist
{
 public:

  virtual Score calculateEditDist(const OBJ& x,
                                  const OBJ& y,
                                  int verbose=0)=0;

  virtual void setErrorModel(Score _hitCost,
                             Score _insCost,
                             Score _substCost,
                             Score _delCost)=0; 
      // Sets the cost of each operation (insertions, deletions and
      // substitutions)

      // destructor
  virtual ~BaseEditDist();
};

//--------------- EditDist class function definitions

//---------------------------------------
template<class OBJ>
BaseEditDist<OBJ>::~BaseEditDist()
{
}

#endif
