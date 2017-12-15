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
    // Nothing to do
}

//---------------------------------------
void JsonTranslationMetadata::obtainTransConstraints(std::string rawSrcSent,int verbosity/*=0*/)
{
    // TODO: How parsing errors should be handled

    // Initialize data structures
    srcSentVec.clear();
    srcPhrTransMap.clear();

    if(!rawSrcSent.empty())
    {
        // Parse input data in JSON format
        picojson::value json;
        std::string err = picojson::parse(json, rawSrcSent);

        if(!err.empty())  // Parsing JSON failed
        {
            std::cerr << "Processing constraints in JSON format failed: " << err << std::endl;
        }
        else if(!json.is<picojson::object>())  // Loaded data are not valid JSON
        {
            std::cerr << "Received data are not a valid JSON: " << rawSrcSent << std::endl;
        }
        else  // JSON parsed correctly
        {
            // Retrieve source sentence
            std::string srcSent = json.get("src_title").get("preprocessed").get<std::string>();
            // Convert raw source sentence into a string vector
            srcSentVec = StrProcUtils::stringToStringVector(srcSent);

            // Retrieve translations based on tags assigned by annotator in TEX
            picojson::array tags = json.get(JSON_TEX_SEGMENTATION).get<picojson::array>();
            std::vector<std::pair<std::string, std::string> > translations;

            for(picojson::array::iterator iter = tags.begin(); iter != tags.end(); iter++)
            {
                // Check if translation field exists
                if((*iter).get("translation").is<std::string>())
                {
                    std::string original = (*iter).get("original").get<std::string>();
                    std::string translation = (*iter).get("translation").get<std::string>();
                    // Check if original (source) and translation (target) phrases contains different charaters than whitespaces
                    if(!containsOnlyWhitespaces(original) && !containsOnlyWhitespaces(translation))
                    {
                        translations.push_back(std::make_pair(original, translation));
                    }
                }
            }

            // Encode translations based on occurences in source sentence
            addTranslations(translations);
        }

        if(verbosity > 0)
        {
            std::cerr << "Source sentence: " << StrProcUtils::stringVectorToString(srcSentVec) << std::endl;

            // Constraints
            std::cerr << "Translation constraints:";

            std::map<std::pair<PositionIndex, PositionIndex>, std::vector<std::string> >::const_iterator const_iter;

            for(const_iter = srcPhrTransMap.begin(); const_iter != srcPhrTransMap.end(); ++const_iter)
            {
                std::cerr << " " << const_iter->first.first << "," << const_iter->first.second << " -> ";
                std::cerr << StrProcUtils::stringVectorToString(const_iter->second) << ";";
            }
            std::cerr<<std::endl;

        }
    }
}

//---------------------------------------
std::vector<std::string> JsonTranslationMetadata::getSrcSentVec(void)const
{
    return srcSentVec;
}

//---------------------------------------
std::vector<std::string> JsonTranslationMetadata::getTransForSrcPhr(std::pair<PositionIndex, PositionIndex> srcPhr)const
{
    // Find translation for source phrase if it exists
    std::map<std::pair<PositionIndex, PositionIndex>, std::vector<std::string> >::const_iterator const_iter;

    const_iter = srcPhrTransMap.find(srcPhr);
    if(const_iter != srcPhrTransMap.end())
    {
        return const_iter->second;
    }
    else
    {
        std::vector<std::string> strVec;
        return strVec;
    }
}

//---------------------------------------
std::set<std::pair<PositionIndex,PositionIndex> > JsonTranslationMetadata::getConstrainedSrcPhrases(void)const
{
    // Initialize variables
    std::set<std::pair<PositionIndex,PositionIndex> > result;

    // Iterate over constraints
    std::map<std::pair<PositionIndex,PositionIndex>, std::vector<std::string> >::const_iterator const_iter;
    for(const_iter = srcPhrTransMap.begin(); const_iter != srcPhrTransMap.end(); ++const_iter)
    {
        result.insert(const_iter->first);
    }

    // Return result
    return result;
}

//---------------------------------------
bool JsonTranslationMetadata::srcPhrAffectedByConstraint(std::pair<PositionIndex, PositionIndex> srcPhr)const
{
    // Iterate over constraints
    std::map<std::pair<PositionIndex,PositionIndex>, std::vector<std::string> >::const_iterator const_iter;
    for(const_iter=srcPhrTransMap.begin(); const_iter != srcPhrTransMap.end(); ++const_iter)
    {
        // Look for overlappings between constraints and given source phrase
        if(srcPhr.first <= const_iter->first.second && srcPhr.second >= const_iter->first.first)
            return true;
    }

    // No constraints were found affecting source phrase
    return false;
}

//---------------------------------------
bool JsonTranslationMetadata::translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                                              const std::vector<PositionIndex>& targetSegmentCuts,
                                                              const std::vector<std::string>& targetWordVec)const
{
    return !transViolatesSrcPhrConstraints(sourceSegmentation,targetSegmentCuts,targetWordVec);
}

//---------------------------------------
bool JsonTranslationMetadata::phraseTranslationIsValid(const std::vector<std::string>& /*sourceWordVec*/,
                                                       const std::vector<std::string>& /*targetWordVec*/)const
{
    // TODO: It is good for now when JSON format has to be compatible with XML
    // Later it should be improved.
    return true;
}

//---------------------------------------
void JsonTranslationMetadata::clear(void)
{
    srcSentVec.clear();
    srcPhrTransMap.clear();
}

//-------- Private methods --------------

//---------------------------------------
bool JsonTranslationMetadata::containsOnlyWhitespaces(std::string phrase)
{
    for(unsigned int i = 0; i < phrase.size(); i++)
        if(phrase[i] != ' ' && phrase[i] != '\t') return false;

    return true;
}


//---------------------------------------
bool JsonTranslationMetadata::isPhraseInSentence(std::vector<std::string>& phrase, unsigned int startPosition)
{
    if(startPosition + phrase.size() > srcSentVec.size())
        return false;  // Sentence is too short to contain phrase

    for(unsigned int i = 0; i < phrase.size(); i++)
        if(srcSentVec[startPosition + i] != phrase[i])
            return false;  // Phrase does not match

    return true;
}

//---------------------------------------
void JsonTranslationMetadata::addTranslations(std::vector<std::pair<std::string, std::string> >& translations)
{
    unsigned int sIdx = 0;  // Source token index
    unsigned int tpIdx = 0;  // Translation pair index

    while(sIdx < srcSentVec.size() && tpIdx < translations.size())
    {
        std::pair<std::string, std::string> tp = translations[tpIdx];
        std::vector<std::string> srcPhraseVec = StrProcUtils::stringToStringVector(tp.first);
        std::vector<std::string> trgPhraseVec = StrProcUtils::stringToStringVector(tp.second);

        if(isPhraseInSentence(srcPhraseVec, sIdx)) {
            std::pair<PositionIndex, PositionIndex> tPos = std::make_pair(sIdx, sIdx + srcPhraseVec.size() - 1);
            srcPhrTransMap[tPos] = trgPhraseVec;
            // Look for the next translation pair
            tpIdx++;
            // Omit tokens which belongs to the found phrase
            sIdx += srcPhraseVec.size();
        } else {
            sIdx++;  // Go to the next token in sentence
        }
    }
}

//---------------------------------------
bool JsonTranslationMetadata::transViolatesSrcPhrConstraints(const SourceSegmentation& sourceSegmentation,
                                                             const std::vector<PositionIndex>& targetSegmentCuts,
                                                             const std::vector<std::string>& targetWordVec)const
{
    std::map<std::pair<PositionIndex, PositionIndex>,std::vector<std::string> >::const_iterator iter;
    // Iterate over all of the source phrase constraints
    for(iter = srcPhrTransMap.begin(); iter != srcPhrTransMap.end(); ++iter)
    {
        if(transViolatesSrcPhrConstraint(iter->first, iter->second, sourceSegmentation, targetSegmentCuts, targetWordVec))
            return true;
    }

    return false;
}

//---------------------------------------
bool JsonTranslationMetadata::transViolatesSrcPhrConstraint(std::pair<PositionIndex,PositionIndex> constrainedSrcSegm,
                                                            std::vector<std::string> constrainedTrans,
                                                            const SourceSegmentation& sourceSegmentation,
                                                            const std::vector<PositionIndex>& targetSegmentCuts,
                                                            const std::vector<std::string>& targetWordVec)const
{
    // Look for source segments in translation that are related to
    // the constrained one
    std::pair<PositionIndex,PositionIndex> relatedSrcSegm;
    std::pair<PositionIndex,PositionIndex> relatedTrgSegm;
    bool foundSrcSegmRelatedToConstraint = false;
    for(unsigned int i = 0; i < sourceSegmentation.size(); ++i)
    {
        if(constrainedSrcSegm.first >= sourceSegmentation[i].first && constrainedSrcSegm.first <= sourceSegmentation[i].second)
        {
            foundSrcSegmRelatedToConstraint = true;
            relatedSrcSegm = sourceSegmentation[i];
            relatedTrgSegm.first = (i == 0) ? 1 : targetSegmentCuts[i - 1] + 1;
            relatedTrgSegm.second = targetSegmentCuts[i];
            break;
        }
    }
    // Check if no related source segment has been covered yet in translation
    if(!foundSrcSegmRelatedToConstraint)
        return false;
    else if(constrainedSrcSegm != relatedSrcSegm)  // Constrained source phrase length is not respected
        return true;

    // Check if translation is equal to the constrained one
    if(constrainedTrans.size() != relatedTrgSegm.second - relatedTrgSegm.first + 1)
        return true;

    for(unsigned int j = 0; j < constrainedTrans.size(); ++j)
    {
        if(constrainedTrans[j] != targetWordVec[relatedTrgSegm.first + j - 1])
            return true;
    }

    // Translation is equal to the constrained one
    return false;
}
