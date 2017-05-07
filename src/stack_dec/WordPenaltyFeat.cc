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
/* Module: WordPenaltyFeat                                          */
/*                                                                  */
/* Definitions file: WordPenaltyFeat.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "WordPenaltyFeat.h"

//--------------- WordPenaltyFeat class functions

template<>
typename WordPenaltyFeat<PhrScoreInfo>::HypScoreInfo
WordPenaltyFeat<PhrScoreInfo>::extensionScore(unsigned int srcSentLen,
                                              const HypScoreInfo& predHypScrInf,
                                              const PhrHypDataStr& predHypDataStr,
                                              const PhrHypDataStr& newHypDataStr,
                                              Score& unweightedScore)
{
  
}
