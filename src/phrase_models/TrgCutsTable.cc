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
/* Definitions file: TrgCutsTable.cc                                */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "TrgCutsTable.h"

//--------------- Global variables -----------------------------------


//--------------- TrgCutsTable class function definitions

//-------------------------
TrgCutsTable::TrgCutsTable(void)
{
  jumpOnePar=JUMP_ONE_POS_PAR;
  stopJumps=STOP_JUMPS_PAR;
}

//-------------------------
LgProb TrgCutsTable::trgCutsLgProb(int offset)
{
//   float result=0;
//   offset=abs(offset);
//   for(int i=0;i<offset;++i)
//   {
//     result+=log(JUMP_ONE_POS_PAR);
//   }
//   result+=log(1-JUMP_ONE_POS_PAR);
//   return result;

  return ((float)abs(offset)*log(jumpOnePar))+log(stopJumps);
}

//-------------------------
bool TrgCutsTable::load(const char *trgCutsTableFileName)
{
 awkInputStream awk;
	
 cerr<<"Loading model for target sentence cuts from file "<<trgCutsTableFileName<<endl;
 if(awk.open(trgCutsTableFileName)==ERROR)
 {
   jumpOnePar=JUMP_ONE_POS_PAR;
   stopJumps=STOP_JUMPS_PAR;
   cerr<<"Warning: file with model for target sentence cuts does not exist, assuming default parameters, jumpOnePar="<<jumpOnePar<<" ; stopJumps="<<stopJumps<<".\n";
   return ERROR;
 }
 else
 {
   if(awk.getline())
   {
     jumpOnePar=atof(awk.dollar(1).c_str());
     stopJumps=atof(awk.dollar(2).c_str());
     cerr<<"Target sentence cuts parameters: jumpOnePar="<<jumpOnePar<<" ; stopJumps="<<stopJumps<<".\n";
   }
 }
 return OK;	
}
