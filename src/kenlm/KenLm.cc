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
/* Module: KenLm                                                    */
/*                                                                  */
/* Definitions file: KenLm.cc                                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "KenLm.h"

//--------------- Global variables -----------------------------------


//--------------- KenLm class function definitions

//-------------------------
KenLm::KenLm(void)
{
}

//-------------------------
LgProb KenLm::getNgramLgProb(WordIndex w,
                             const Vector<WordIndex>& vu)
{
}

//-------------------------
LgProb KenLm::getNgramLgProbStr(string s,const Vector<string>& rq)
{
}
//-------------------------
LgProb KenLm::getLgProbEnd(const Vector<WordIndex>& vu)
{
}

//-------------------------
LgProb KenLm::getLgProbEndStr(const Vector<string>& rq)
{
}

//-------------------------
bool KenLm::getStateForWordSeq(const Vector<WordIndex>& wordSeq,
                               Vector<WordIndex>& state)
{
}

//-------------------------
void KenLm::getStateForBeginOfSentence(Vector<WordIndex> &state)
{
}

//-------------------------
LgProb KenLm::getNgramLgProbGivenState(WordIndex w,
                                       Vector<WordIndex> &state)
{
}

//-------------------------
LgProb KenLm::getNgramLgProbGivenStateStr(std::string s,
                                          Vector<WordIndex> &state)
{
}

//-------------------------
LgProb KenLm::getLgProbEndGivenState(Vector<WordIndex> &state)
{
}

//-------------------------
bool KenLm::existSymbol(string s)const
{
}

//-------------------------
WordIndex KenLm::addSymbol(string s)
{
}

//-------------------------
unsigned int KenLm::getVocabSize(void)
{
}

//-------------------------
WordIndex KenLm::stringToWordIndex(string s)const
{
}

//-------------------------
string KenLm::wordIndexToString(WordIndex w)const
{
}

//-------------------------
WordIndex KenLm::getBosId(bool &found)const
{
}

//-------------------------
WordIndex KenLm::getEosId(bool &found)const
{
}

//-------------------------
bool KenLm::loadVocab(const char *fileName)
{
}

//-------------------------
bool KenLm::printVocab(const char *fileName)
{
}

//-------------------------
void KenLm::clearVocab(void)
{
}

//-------------------------
bool KenLm::load(const char *fileName)
{
}

//-------------------------
bool KenLm::print(const char *fileName)
{
}

//-------------------------
unsigned int KenLm::getNgramOrder(void)
{
}

//-------------------------
void KenLm::setNgramOrder(int _ngramOrder)
{

}

//-------------------------
size_t KenLm::size(void)
{
}

//-------------------------
void KenLm::clear(void)
{
}
