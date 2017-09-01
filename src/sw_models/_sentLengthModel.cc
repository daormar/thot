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
/* Module: _sentLengthModel                                          */
/*                                                                   */
/* Definitions file: _sentLengthModel.cc                             */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "_sentLengthModel.h"

//--------------- Global variables ------------------------------------

//--------------- Function declarations 

//--------------- Constants


//--------------- Classes ---------------------------------------------

//--------------- _sentLengthModel class method definitions

//-------------------------
void _sentLengthModel::linkVocabPtr(SingleWordVocab* _swVocabPtr)
{
  swVocabPtr=_swVocabPtr;
}

//-------------------------
void _sentLengthModel::linkSentPairInfo(BaseSentenceHandler* _sentenceHandlerPtr)
{
  sentenceHandlerPtr=_sentenceHandlerPtr;
}

//-------------------------
void _sentLengthModel::trainSentPairRange(pair<unsigned int,unsigned int> sentPairRange,
                                          int /*verbosity=0*/)
{
      // Iterate over the training samples
  for(unsigned int n=sentPairRange.first;n<=sentPairRange.second;++n)
  {
    std::vector<std::string> srcSentStrVec;
    std::vector<std::string> trgSentStrVec;
    Count c;
    nthSentPair(n,srcSentStrVec,trgSentStrVec,c);

    trainSentPair(srcSentStrVec,trgSentStrVec,c);
  }
}

//-------------------------
int _sentLengthModel::nthSentPair(unsigned int n,
                                  std::vector<std::string>& srcSentStr,
                                  std::vector<std::string>& trgSentStr,
                                  Count& c)
{
  return sentenceHandlerPtr->nthSentPair(n,srcSentStr,trgSentStr,c);
}

//-------------------------
