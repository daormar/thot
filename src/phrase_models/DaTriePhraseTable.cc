/*
thot package for statistical machine translation
 
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
/* Module: DaTriePhraseTable                                        */
/*                                                                  */
/* Definitions file: DaTriePhraseTable.cc                           */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "DaTriePhraseTable.h"

//--------------- Function definitions

//-------------------------
DaTriePhraseTable::DaTriePhraseTable(void)
{
}
//-------------------------
bool DaTriePhraseTable::getNbestForSrc(const Vector<WordIndex>& s,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt)
{
      // TO-BE-DONE (LOW PRIORITY)
}
//-------------------------
bool DaTriePhraseTable::getNbestForTrg(const Vector<WordIndex>& t,
                                       NbestTableNode<PhraseTransTableNodeData>& nbt,
                                       int N)
{
      // TO-BE-DONE (MEDIUM PRIORITY)
}

//-------------------------
void DaTriePhraseTable::addTableEntry(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      PhrasePairInfo inf) 
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::addSrcInfo(const Vector<WordIndex>& s,
                                   Count s_inf)
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::addSrcTrgInfo(const Vector<WordIndex>& s,
                                      const Vector<WordIndex>& t,
                                      Count st_inf)
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::incrCountsOfEntry(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t,
                                          Count c) 
{
      // TO-BE-DONE (MEDIUM PRIORITY)
}

//-------------------------
PhrasePairInfo DaTriePhraseTable::infSrcTrg(const Vector<WordIndex>& s,
                                            const Vector<WordIndex>& t,
                                            bool& found) 
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
Count DaTriePhraseTable::getSrcInfo(const Vector<WordIndex>& s,
                                    bool &found)
{
      // TO-BE-DONE (HIGH PRIORITY)
}

//-------------------------
Count DaTriePhraseTable::getSrcTrgInfo(const Vector<WordIndex>& s,
                                       const Vector<WordIndex>& t,
                                       bool &found)
{
      // TO-BE-DONE (HIGH PRIORITY)
}

//-------------------------
Prob DaTriePhraseTable::pTrgGivenSrc(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
{  
      // TO-BE-DONE (HIGH PRIORITY)
      // p(t|s)=count(s,t)/count(s)
}

//-------------------------
LgProb DaTriePhraseTable::logpTrgGivenSrc(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t)
{
  return log((double)pTrgGivenSrc(s,t));
}

//-------------------------
Prob DaTriePhraseTable::pSrcGivenTrg(const Vector<WordIndex>& s,
                                     const Vector<WordIndex>& t)
{
      // TO-BE-DONE (HIGH PRIORITY)
      // p(s|t)=count(s,t)/count(t)
}

//-------------------------
LgProb DaTriePhraseTable::logpSrcGivenTrg(const Vector<WordIndex>& s,
                                          const Vector<WordIndex>& t)
{
  return log((double)pSrcGivenTrg(s,t));
}

//-------------------------
bool DaTriePhraseTable::getEntriesForTarget(const Vector<WordIndex>& t,
                                            DaTriePhraseTable::SrcTableNode& srctn) 
{
      // TO-BE-DONE (MEDIUM PRIORITY)
}

//-------------------------
bool DaTriePhraseTable::getEntriesForSource(const Vector<WordIndex>& s,
                                            DaTriePhraseTable::TrgTableNode& trgtn) 
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
Count DaTriePhraseTable::cSrcTrg(const Vector<WordIndex>& s,
                                 const Vector<WordIndex>& t)
{
  bool found;
  return getSrcTrgInfo(s,t,found).get_c_st();
}

//-------------------------
Count DaTriePhraseTable::cSrc(const Vector<WordIndex>& s)
{
  bool found;
  return getSrcInfo(s,found).get_c_s();
}

//-------------------------
Count DaTriePhraseTable::cTrg(const Vector<WordIndex>& t)
{
      // TO-BE-DONE (HIGH PRIORITY)
}

//-------------------------
size_t DaTriePhraseTable::size(void)
{
      // TO-BE-DONE (LOW PRIORITY)
}

//-------------------------
void DaTriePhraseTable::clear(void)
{
      // TO-BE-DONE (HIGH PRIORITY)
}

//-------------------------
DaTriePhraseTable::~DaTriePhraseTable(void)
{

}

//-------------------------
DaTriePhraseTable::const_iterator DaTriePhraseTable::begin(void)const
{
  // DaTriePhraseTable::const_iterator iter(this,phraseDict.begin());
  // return iter;
}

//-------------------------
DaTriePhraseTable::const_iterator DaTriePhraseTable::end(void)const
{
  // DaTriePhraseTable::const_iterator iter(this,phraseDict.end());
  // return iter;
}

// const_iterator function definitions
//--------------------------
bool DaTriePhraseTable::const_iterator::operator++(void) //prefix
{
  // if(ptPtr!=NULL)
  // {
  //   ++pdIter;
  //   if(pdIter==ptPtr->phraseDict.end()) return false;
  //   else
  //   {
  //     return true;
  //   }
  // }
  // else return false;
}

//--------------------------
bool DaTriePhraseTable::const_iterator::operator++(int)  //postfix
{
  return operator++();
}

//--------------------------
int DaTriePhraseTable::const_iterator::operator==(const const_iterator& right)
{
  // return (ptPtr==right.ptPtr && pdIter==right.pdIter);	
}

//--------------------------
int DaTriePhraseTable::const_iterator::operator!=(const const_iterator& right)
{
  return !((*this)==right);
}

//--------------------------
// const PhraseDict::const_iterator&
// DaTriePhraseTable::const_iterator::operator->(void)const
// {
//   return pdIter;
// }

//-------------------------
