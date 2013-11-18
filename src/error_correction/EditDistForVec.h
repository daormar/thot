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
/* Module: EditDistForVec                                           */
/*                                                                  */
/* Prototypes file: EditDistForVec.h                                */
/*                                                                  */
/* Description: Defines the EditDistForVec class that calculates    */
/*              the edit distance between Vectors of objects.       */
/*                                                                  */
/********************************************************************/

#ifndef _EditDistForVec_h
#define _EditDistForVec_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_editDist.h"

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------


//--------------- Classes --------------------------------------------


//--------------- EditDistForVec class declaration

template<class OBJ>
class EditDistForVec: public _editDist<Vector<OBJ> >
{
 public:
	 
  EditDistForVec(void);
  			
  virtual ~EditDistForVec(void);

 protected:
      // Basic function to calculate edit distance
  Score processMatrixCell(const Vector<OBJ>& x,
                           const Vector<OBJ>& y,
                           const DistMatrix& dm,
                           int i,
                           int j,
                           int& pred_i,
                           int& pred_j,
                           int& op_id);
};

//--------------- Template function definitions

//---------------
template<class OBJ>
EditDistForVec<OBJ>::EditDistForVec(void):_editDist<Vector<OBJ> >()
{
}

//---------------------------------------
template<class OBJ>
Score EditDistForVec<OBJ>::processMatrixCell(const Vector<OBJ>& x,
                                             const Vector<OBJ>& y,
                                             const DistMatrix& dm,
                                             int i,
                                             int j,
                                             int& pred_i,
                                             int& pred_j,
                                             int& op_id)
{
  if(i==0 && j==0)
  {
    pred_i=0;
    pred_j=0;
    op_id=NONE_OP;
    return 0;
  }
  else
  {
    if(i==0)
    {
      pred_i=0;
      pred_j=j-1;
      op_id=INS_OP;
      return dm[0][j-1] + this->insCost;
    }
    else
    {
      if(j==0)
      {
        pred_i=i-1;
        pred_j=0;
        op_id=DEL_OP;
        return dm[i-1][0] + this->delCost;
      }
      else
      {
        Score min;
        pred_i=i-1;
        pred_j=j-1;
        
        if(x[i-1]==y[j-1])
        {
          min = dm[i-1][j-1] + this->hitCost;
          op_id=HIT_OP;
        }
        else
        {
          min = dm[i-1][j-1] + this->substCost;
          op_id=SUBST_OP;       
        }
        if (dm[i-1][j]+this->delCost < min)
        {
          min = dm[i-1][j]+this->delCost;
          pred_i=i-1;
          pred_j=j;
          op_id=DEL_OP;
        }
        if (dm[i][j-1]+this->insCost < min)
        {
          min = dm[i][j-1]+this->insCost;
          pred_i=i;
          pred_j=j-1;
          op_id=INS_OP;
        }
        return min;
      }
    }
  }
}

//---------------
template<class OBJ>
EditDistForVec<OBJ>::~EditDistForVec(void)
{
}

#endif
