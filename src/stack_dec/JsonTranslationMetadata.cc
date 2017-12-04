/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz

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
/* Module: JsonTranslationMetadata                                  */
/*                                                                  */
/* Definitions file: JsonTranslationMetadata.cc                     */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "JsonTranslationMetadata.h"

//--------------- JsonTranslationMetadata class functions
//

//---------------------------------------
JsonTranslationMetadata::JsonTranslationMetadata(void)
{
    // TODO
}

//---------------------------------------
void JsonTranslationMetadata::obtainTransConstraints(std::string rawSrcSent,int verbosity/*=0*/)
{
    // TODO
}

//---------------------------------------
std::vector<std::string> JsonTranslationMetadata::getSrcSentVec(void)const
{
    // TODO
}

//---------------------------------------
std::vector<std::string> JsonTranslationMetadata::getTransForSrcPhr(std::pair<PositionIndex,PositionIndex> srcPhr)const
{
    // TODO
}

//---------------------------------------
std::set<std::pair<PositionIndex,PositionIndex> > JsonTranslationMetadata::getConstrainedSrcPhrases(void)const
{
    // TODO
}

//---------------------------------------
bool JsonTranslationMetadata::srcPhrAffectedByConstraint(std::pair<PositionIndex,PositionIndex> srcPhr)const
{
    // TODO
}

//---------------------------------------
bool JsonTranslationMetadata::translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                                          const std::vector<PositionIndex>& targetSegmentCuts,
                                                          const std::vector<std::string>& targetWordVec)const
{
    // TODO
}

//---------------------------------------
bool JsonTranslationMetadata::phraseTranslationIsValid(const std::vector<std::string>& /*sourceWordVec*/,
                                                   const std::vector<std::string>& /*targetWordVec*/)const
{
    // TODO
}

//---------------------------------------
void JsonTranslationMetadata::clear(void)
{
    // TODO
}
