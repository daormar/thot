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
#define JSON_SELLER_TERMS "seller_term_dict"
#define JSON_APPROVED_TERMS "approved_term_dict"
#define JSON_FORBIDDEN_TERMS "forbidden_terms"
#define JSON_BRANDS "brands"
#define JSON_UNTRANSLATABLE_TERMS "untranslatable_terms"

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
      // Data structures
  typedef struct {
    std::string dstText;
    std::string srcLang;
    std::string dstLang;
    unsigned int freq;
  } SellerTerm;

  typedef struct {
    std::string dstText;
    int category;
    unsigned int freq;
    float changeProb;
  } ApprovedTerm;

  typedef struct {
    std::string dstText;
    int category;
    std::string source;
  } Brand;

      // Data members
  std::vector<std::string> srcSentVec;
  int productCategory;
  std::multimap<std::string, SellerTerm> sellerTerms;
  std::multimap<std::string, ApprovedTerm> approvedTerms;
  std::multimap<std::string, Brand> brandTerms;
  std::vector<std::string> untranslatableTerms;

  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> > srcPhrTransMap;  // TODO: Remove and replace with specialized data members for different types of the terms

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

            // Read item category
            productCategory = (int) json.get("category").get<double>();

            // Retrieve seller terms
            if(json.get(JSON_SELLER_TERMS).is<picojson::array>())
            {
                picojson::array seller = json.get(JSON_SELLER_TERMS).get<picojson::array>();
                for(picojson::array::iterator iter = seller.begin(); iter != seller.end(); iter++)
                {
                    SellerTerm term;
                    term.dstText = (*iter).get("dst_text").get<std::string>();
                    term.srcLang = (*iter).get("src_lang").get<std::string>();
                    term.dstLang = (*iter).get("dst_lang").get<std::string>();
                    term.freq = (float) (*iter).get("frequency").get<double>();

                    std::string srcText = (*iter).get("src_text").get<std::string>();

                    sellerTerms.insert(std::make_pair(srcText, term));
                }
            }

            // Retrieve approved terms
            if(json.get(JSON_APPROVED_TERMS).is<picojson::array>())
            {
                picojson::array approved = json.get(JSON_APPROVED_TERMS).get<picojson::array>();
                for(picojson::array::iterator iter = approved.begin(); iter != approved.end(); iter++)
                {
                    ApprovedTerm term;
                    term.dstText = (*iter).get("dst_text").get<std::string>();
                    term.category = (int) (*iter).get("category").get<double>();
                    term.freq = (float) (*iter).get("frequency").get<double>();
                    term.changeProb = (float) (*iter).get("change_probability").get<double>();

                    std::string srcText = (*iter).get("src_text").get<std::string>();

                    approvedTerms.insert(std::make_pair(srcText, term));
                }
            }

            // Retrieve brands
            if(json.get(JSON_BRANDS).is<picojson::array>())
            {
                picojson::array brands = json.get(JSON_BRANDS).get<picojson::array>();
                for(picojson::array::iterator iter = brands.begin(); iter != brands.end(); iter++)
                {
                    Brand brand;
                    brand.dstText = (*iter).get("dst_text").get<std::string>();
                    if((*iter).get("category").is<double>())
                    {
                        brand.category = (int) (*iter).get("category").get<double>();
                    }
                    else
                    {
                        brand.category = -1;  // Empty category values
                    }
                    brand.source = (*iter).get("source").get<std::string>();

                    std::string srcText = (*iter).get("src_text").get<std::string>();
                    brandTerms.insert(std::make_pair(srcText, brand));
                }
            }

            // Retrieve untranslatable terms
            if(json.get(JSON_UNTRANSLATABLE_TERMS).is<picojson::array>())
            {
                picojson::array untranslatable = json.get(JSON_UNTRANSLATABLE_TERMS).get<picojson::array>();
                for(picojson::array::iterator iter = untranslatable.begin(); iter != untranslatable.end(); iter++)
                {
                    std::string srcText = (*iter).get("src_text").get<std::string>();
                    untranslatableTerms.push_back(srcText);
                }
            }

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
    if(startPosition + phrase.size() - 1 > srcSentVec.size())
        return false;  // Sentence is too short to contain phrase

    for(unsigned int i = 0; i < phrase.size(); i++)
        if(srcSentVec[startPosition + i - 1] != phrase[i])
            return false;  // Phrase does not match

    return true;
}

//---------------------------------------
template<class SCORE_INFO>
void JsonTranslationMetadata<SCORE_INFO>::addTranslations(std::vector<std::pair<std::string, std::string> >& translations)
{
    unsigned int sIdx = 1;  // Source token index
    unsigned int tpIdx = 0;  // Translation pair index

    while(sIdx <= srcSentVec.size() && tpIdx < translations.size())
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
