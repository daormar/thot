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
 
/*********************************************************************/
/*                                                                   */
/* Module: DoubleMatrix                                              */
/*                                                                   */
/* Definitions file: DoubleMatrix.cc                                 */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "DoubleMatrix.h"

//--------------- Global variables ------------------------------------

//--------------- Function declarations 

//--------------- Constants

//--------------- Classes ---------------------------------------------

//-------------------------
void DoubleMatrix::set(unsigned int i,
                       unsigned int j,
                       double d)
{
      // Grow in the dimension of i if necessary
  while(doubleVecVec.size()<=i)
  {
    std::vector<double> aj;
    doubleVecVec.push_back(aj);
  }

      // Grow in the dimension of i if necessary
  while(doubleVecVec[i].size()<=j)
  {
    doubleVecVec[i].push_back(INVALID_DM_VAL);
  }
      // Set value
  doubleVecVec[i][j]=d;
}

//-------------------------
unsigned int DoubleMatrix::i_size(void)
{
  return doubleVecVec.size();
}

//-------------------------
unsigned int DoubleMatrix::ij_size(unsigned int i)
{
  return doubleVecVec[i].size();
}

//-------------------------
void DoubleMatrix::clear(void)
{
  doubleVecVec.clear();
}
