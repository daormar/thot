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
/* Module: WbaIncrPhraseModel                                       */
/*                                                                  */
/* Definitions file: WbaIncrPhraseModel.cc                          */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "WbaIncrPhraseModel.h"


//--------------- Function definitions

//-------------------------
void WbaIncrPhraseModel::printTTable(FILE* file)
{
  StlPhraseTable* ptPtr=0;

  ptPtr=dynamic_cast<StlPhraseTable*>(basePhraseTablePtr);

  if(ptPtr) // C++ RTTI
  {
    StlPhraseTable::TrgPhraseInfo::const_iterator phraseTIter;
      
    for(phraseTIter=ptPtr->beginTrg();phraseTIter!=ptPtr->endTrg();++phraseTIter)
    {
      StlPhraseTable::SrcTableNode srctn;
      StlPhraseTable::SrcTableNode::iterator srctnIter;
      ptPtr->getEntriesForTarget(phraseTIter->first,srctn);

      for(srctnIter=srctn.begin();srctnIter!=srctn.end();++srctnIter)
      {
        std::vector<WordIndex>::const_iterator vectorWordIndexIter;
        for(vectorWordIndexIter=srctnIter->first.begin();vectorWordIndexIter!=srctnIter->first.end();++vectorWordIndexIter)
          fprintf(file,"%s ",wordIndexToSrcString(*vectorWordIndexIter).c_str());
        fprintf(file,"|||"); 
        for(vectorWordIndexIter=phraseTIter->first.begin();vectorWordIndexIter!=phraseTIter->first.end();++vectorWordIndexIter)
          fprintf(file," %s",wordIndexToTrgString(*vectorWordIndexIter).c_str());
        fprintf(file," ||| %.8f %.8f\n",(float)srctnIter->second.first.get_c_s(),(float)srctnIter->second.second.get_c_st());
      }
    }
  }
}

//-------------------------
WbaIncrPhraseModel::~WbaIncrPhraseModel()
{
  delete basePhraseTablePtr;  
}

//-------------------------
