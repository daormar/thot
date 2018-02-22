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

/**
 * @file JsonTranslationMetadata.h 
 * 
 * @brief Class to handle translation metadata in JSON format.
 */

#ifndef _JsonTranslationMetadata_h
#define _JsonTranslationMetadata_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationMetadata.h"
#include "picojson.h"

//--------------- Constants ------------------------------------------

#define JSON_TEX_SEGMENTATION "tex_segmentation"
#define JSON_SRCTITLE "src_title"

//--------------- Typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

template<class SCORE_INFO>
class JsonTranslationMetadata: public BaseTranslationMetadata<SCORE_INFO>
{
 public:

      // Constructor
  JsonTranslationMetadata(void);

      // Services

      // Initialization and source sentence extraction
  void obtainTransConstraints(std::string rawSrcSent,
                              int verbosity=0);
  std::vector<std::string> getSrcSentVec(void)const;

      // Constraint-related functions
  std::vector<std::string> getTransForSrcPhr(std::pair<PositionIndex, PositionIndex> srcPhr)const;
  std::set<std::pair<PositionIndex,PositionIndex> > getConstrainedSrcPhrases(void)const;
  bool srcPhrAffectedByConstraint(std::pair<PositionIndex, PositionIndex> srcPhr)const;
  bool translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                       const std::vector<PositionIndex>& targetSegmentCuts,
                                       const std::vector<std::string>& targetWordVec)const;
      // This function verifies if a sentence translation satisfies
      // constraints. It is applied over complete translations or
      // partial ones built from left to right.
  bool phraseTranslationIsValid(const std::vector<std::string>& sourceWordVec,
                                const std::vector<std::string>& targetWordVec)const;

  void clear(void);

      // Destructor
  ~JsonTranslationMetadata(){};

 private:

      // Data members
  std::vector<std::string> srcSentVec;
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> > srcPhrTransMap;

      // Check if string is empty or contains only whitespace characters
  bool containsOnlyWhitespaces(std::string phrase);
      // Check if phrase is in source sentence at given position
  bool isPhraseInSentence(std::vector<std::string>& phrase, unsigned int startPosition);
      // Map translation on their positions in source sentence
  void addTranslations(std::vector<std::pair<std::string, std::string> >& translations);
      // Check if translation violates constraints
  bool transViolatesSrcPhrConstraints(const SourceSegmentation& sourceSegmentation,
                                      const std::vector<PositionIndex>& targetSegmentCuts,
                                      const std::vector<std::string>& targetWordVec)const;
  bool transViolatesSrcPhrConstraint(std::pair<PositionIndex,PositionIndex> constrainedSrcSegm,
                                     std::vector<std::string> constrainedTrans,
                                     const SourceSegmentation& sourceSegmentation,
                                     const std::vector<PositionIndex>& targetSegmentCuts,
                                     const std::vector<std::string>& targetWordVec)const;
};

//--------------- JsonTranslationMetadata class functions
//

//---------------------------------------
template<class SCORE_INFO>
JsonTranslationMetadata<SCORE_INFO>::JsonTranslationMetadata(void)
{
    // Nothing to do
}

//---------------------------------------
template<class SCORE_INFO>
void JsonTranslationMetadata<SCORE_INFO>::obtainTransConstraints(std::string rawSrcSent,int verbosity/*=0*/)
{
    // Clear data structures
    clear();

    if(!rawSrcSent.empty())
    {
        // Parse input data in JSON format
        picojson::value json;
        std::string err = picojson::parse(json, rawSrcSent);

        if(!err.empty())  // Parsing JSON failed (raw source sentence is
                          // set as the source sentence)
        {
            std::cerr << "Processing constraints in JSON format failed: " << err << std::endl;
            srcSentVec=StrProcUtils::stringToStringVector(rawSrcSent);
        }
        else if(!json.is<picojson::object>())  // Loaded data are not
                                               // valid JSON (raw source
                                               // sentence is set as the
                                               // source sentence)
        {
            std::cerr << "Received data are not a valid JSON: " << rawSrcSent << std::endl;
            srcSentVec=StrProcUtils::stringToStringVector(rawSrcSent);
        }
        else  // JSON parsed correctly
        {
              // Retrieve source sentence
          srcSentVec.clear();
          if(json.get(JSON_SRCTITLE).is<picojson::object>())
          {
            if(json.get(JSON_SRCTITLE).get("preprocessed").is<std::string>())
            {
              std::string srcSent = json.get(JSON_SRCTITLE).get("preprocessed").get<std::string>();
                    // Convert raw source sentence into a string vector
              srcSentVec = StrProcUtils::stringToStringVector(srcSent);
            }
          }

              // Retrieve translations based on tags assigned by annotator in TEX
          if(json.get(JSON_TEX_SEGMENTATION).is<picojson::array>())
          {
                // Retrieve translations
            picojson::array tags = json.get(JSON_TEX_SEGMENTATION).get<picojson::array>();
            std::vector<std::pair<std::string, std::string> > translations;
            for(picojson::array::iterator iter = tags.begin(); iter != tags.end(); iter++)
            {
              bool success=true;
              
                  // Check if translation field exists
              std::string translation;
              if((*iter).get("translation").is<std::string>())
                translation = (*iter).get("translation").get<std::string>();
              else
                success=false;

              std::string original;
              if((*iter).get("original").is<std::string>())
                original = (*iter).get("original").get<std::string>();
              else
                success=false;
              
              if(success)
              {
                    // Check if original (source) and translation
                    // (target) phrases contains different characters
                    // than whitespaces
                if(!containsOnlyWhitespaces(original) && !containsOnlyWhitespaces(translation))
                  translations.push_back(std::make_pair(original, translation));
              }
              else
                std::cerr<<"Error while parsing segmentation constraint"<<std::endl;
            }
                // Encode translations based on occurences in source sentence
            addTranslations(translations);
          }
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
template<class SCORE_INFO>
std::vector<std::string> JsonTranslationMetadata<SCORE_INFO>::getSrcSentVec(void)const
{
    return srcSentVec;
}

//---------------------------------------
template<class SCORE_INFO>
std::vector<std::string> JsonTranslationMetadata<SCORE_INFO>::getTransForSrcPhr(std::pair<PositionIndex, PositionIndex> srcPhr)const
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
template<class SCORE_INFO>
std::set<std::pair<PositionIndex,PositionIndex> > JsonTranslationMetadata<SCORE_INFO>::getConstrainedSrcPhrases(void)const
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
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::srcPhrAffectedByConstraint(std::pair<PositionIndex, PositionIndex> srcPhr)const
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
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                                                          const std::vector<PositionIndex>& targetSegmentCuts,
                                                                          const std::vector<std::string>& targetWordVec)const
{
    return !transViolatesSrcPhrConstraints(sourceSegmentation,targetSegmentCuts,targetWordVec);
}

//---------------------------------------
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::phraseTranslationIsValid(const std::vector<std::string>& /*sourceWordVec*/,
                                                                   const std::vector<std::string>& /*targetWordVec*/)const
{
    // TODO: It is good for now when JSON format has to be compatible with XML
    // Later it should be improved.
    return true;
}

//---------------------------------------
template<class SCORE_INFO>
void JsonTranslationMetadata<SCORE_INFO>::clear(void)
{
    srcSentVec.clear();
    srcPhrTransMap.clear();
}

//-------- Private methods --------------

//---------------------------------------
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::containsOnlyWhitespaces(std::string phrase)
{
    for(unsigned int i = 0; i < phrase.size(); i++)
        if(phrase[i] != ' ' && phrase[i] != '\t') return false;

    return true;
}


//---------------------------------------
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::isPhraseInSentence(std::vector<std::string>& phrase, unsigned int startPosition)
{
  if(phrase.empty())
    return true;

  if(startPosition + phrase.size() - 1 > srcSentVec.size())
    return false;  // Sentence is too short to contain phrase
  
  for(unsigned int i = 0; i < phrase.size(); i++)
    if(srcSentVec[startPosition + i] != phrase[i])
      return false;  // Phrase does not match
  
  return true;
}

//---------------------------------------
template<class SCORE_INFO>
void JsonTranslationMetadata<SCORE_INFO>::addTranslations(std::vector<std::pair<std::string, std::string> >& translations)
{
  if(!translations.empty())
  {
    unsigned int sIdx = 0;  // Source token index
    unsigned int tpIdx = 0;  // Translation pair index

    while(sIdx < srcSentVec.size() && tpIdx < translations.size())
    {
        std::pair<std::string, std::string> tp = translations[tpIdx];
        std::vector<std::string> srcPhraseVec = StrProcUtils::stringToStringVector(tp.first);
        std::vector<std::string> trgPhraseVec = StrProcUtils::stringToStringVector(tp.second);

        if(isPhraseInSentence(srcPhraseVec, sIdx))
        {
            std::pair<PositionIndex, PositionIndex> tPos = std::make_pair(sIdx+1, sIdx + srcPhraseVec.size());
            srcPhrTransMap[tPos] = trgPhraseVec;
            // Look for the next translation pair
            tpIdx++;
            // Omit tokens which belongs to the found phrase
            sIdx += srcPhraseVec.size();
        }
        else
        {
            sIdx++;  // Go to the next token in sentence
        }
    }
  }
}

//---------------------------------------
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::transViolatesSrcPhrConstraints(const SourceSegmentation& sourceSegmentation,
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
template<class SCORE_INFO>
bool JsonTranslationMetadata<SCORE_INFO>::transViolatesSrcPhrConstraint(std::pair<PositionIndex,PositionIndex> constrainedSrcSegm,
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

#endif
