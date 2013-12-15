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
/* Module: WordAligMatrix                                           */
/*                                                                  */
/* Prototype file: WordAligMatrix                                   */
/*                                                                  */
/* Description: Defines the WordAligMatrix class for store a        */
/*              word-level alignment matrix.                        */
/*                                                                  */
/********************************************************************/

#ifndef _wordAligMatrix_h
#define _wordAligMatrix_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "myVector.h"
#include <map>
#include "PositionIndex.h"

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------

class WordAligMatrix;
	
//--------------- function declarations ------------------------------

ostream& operator << (ostream &outS,const WordAligMatrix &waMatrix);

//--------------- Classes --------------------------------------------

//--------------- WordAligMatrix class
class WordAligMatrix
{
 public:

  // Constructors
  WordAligMatrix(void);
  WordAligMatrix(unsigned int I_dims,unsigned int J_dims);
  WordAligMatrix(const WordAligMatrix &waMatrix);

  // Basic operations
  unsigned int get_I(void)const;
  unsigned int get_J(void)const;
  unsigned int getValue(unsigned int i,unsigned int j)const;
  void init(unsigned int I_dims,unsigned int J_dims);
  void putAligVec(Vector<PositionIndex> aligVec);
      // Put alignment vector into word matrix.
      // aligVec[j]=0 denotes that the j'th word is not aligned.
      // j is in the range [0,J-1], i is in the range [1,I]
  bool getAligVec(Vector<PositionIndex>& aligVec);
  void reset(void);
  void set(void);
  void clear(void);
  void set(unsigned int i,unsigned int j);
      // Set position i,j to 1. The first word has index 0
  void setValue(unsigned int i,unsigned int j,unsigned int val);
  void transpose(void);
  WordAligMatrix& operator= (const WordAligMatrix &waMatrix);
  bool operator== (const WordAligMatrix &waMatrix);
  WordAligMatrix& flip(void); //flips every bit of the matrix

  // Operations between word alignment matrices
  WordAligMatrix& operator&= (const WordAligMatrix &waMatrix);
      //Bitwise AND of two WordAligMatrix
  WordAligMatrix& operator|= (const WordAligMatrix &waMatrix);
      //Bitwise incl OR of two WordAligMatrix
  WordAligMatrix& operator^= (const WordAligMatrix &waMatrix);
      //Bitwise excl OR of two WordAligMatrix
  WordAligMatrix& operator+= (const WordAligMatrix &waMatrix);
      //Sum of two WordAligMatrix
  WordAligMatrix& operator-= (const WordAligMatrix &waMatrix);
      //Sustract waMatrix from *this
  WordAligMatrix& symmetr1 (const WordAligMatrix &waMatrix);
      //Combine two WordAligMatrix in the Och way (1999)
  WordAligMatrix& symmetr2  (const WordAligMatrix &waMatrix);
      //Combine two WordAligMatrix in the Och way (2002, Master thesis)
  WordAligMatrix& growDiag  (const WordAligMatrix &waMatrix);
      //Combine two WordAligMatrix using grow-diag

  // Predicates
  bool jAligned(unsigned int j)const;
  bool iAligned(unsigned int i)const;
  bool ijInNeighbourhood(unsigned int i,unsigned int j);
  bool ijHasHorizNeighbours(unsigned int i,unsigned int j);
  bool ijHasVertNeighbours(unsigned int i,unsigned int j);

  // Printing functions
  friend ostream& operator << (ostream &outS,const WordAligMatrix &waMatrix);
  void print(FILE* f);
  void wordAligAsVectors(Vector<pair<unsigned int,unsigned int> >& sourceSegm,Vector<unsigned int>& targetCuts);

  // Destructor
  ~WordAligMatrix();
    
  private:

      // Data members
   unsigned int I;
   unsigned int J;
   unsigned int** matrix;
};
#endif
