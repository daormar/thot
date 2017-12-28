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
 
#ifndef _TranslationMetadata_h
#define _TranslationMetadata_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseTranslationMetadata.h"

//--------------- Constants ------------------------------------------

#define XML_PHR_ANNOT_TAG_NAME "phr_pair_annot"
#define XML_SRC_SEGM_TAG_NAME "src_segm"
#define XML_TRG_SEGM_TAG_NAME "trg_segm"

//--------------- Typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

template<class SCORE_INFO>
class TranslationMetadata: public BaseTranslationMetadata<SCORE_INFO>
{
 public:

      // Constructor
  TranslationMetadata(void);

      // Services

      // Initialization and source sentence extraction
  void obtainTransConstraints(std::string rawSrcSent,
                              int verbosity=0);
  std::vector<std::string> getSrcSentVec(void)const;

      // Constraint-related functions
  std::vector<std::string> getTransForSrcPhr(std::pair<PositionIndex,PositionIndex> srcPhr)const;
  std::set<std::pair<PositionIndex,PositionIndex> > getConstrainedSrcPhrases(void)const;
  bool srcPhrAffectedByConstraint(std::pair<PositionIndex,PositionIndex> srcPhr)const;
  bool translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                       const std::vector<PositionIndex>& targetSegmentCuts,
                                       const std::vector<std::string>& targetWordVec)const;
      // This function verifies if a sentence translation satisfies
      // constraints. It is applied over complete translations or
      // partial ones built from left to right.
  bool phraseTranslationIsValid(const std::vector<std::string>& sourceWordVec,
                                const std::vector<std::string>& targetWordVec)const;

  void clear(void);
  
 private:

      // Data members
  std::set<std::string> xmlTagSet;
  std::vector<std::string> srcSentVec;
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> > srcPhrTransMap;

      // Auxiliary functions
  std::string tokenizeSrcSentence(std::string srcSent)const;
  std::string obtainStartTag(std::string tagName)const;
  std::string obtainEndTag(std::string tagName)const;
  bool xmlTag(std::string srcSent,
              unsigned int initialPos,
              unsigned int& endTagPos)const;
  bool constraintFound(std::vector<std::string> tokRawSrcSentVec,
                       unsigned int currPos,
                       std::vector<std::string>& srcPhrase,
                       std::vector<std::string>& trgPhrase,
                       unsigned int& finalPos)const;
  bool transViolatesSrcPhrConstraints(const SourceSegmentation& sourceSegmentation,
                                      const std::vector<PositionIndex>& targetSegmentCuts,
                                      const std::vector<std::string>& targetWordVec)const;
  bool transViolatesSrcPhrConstraint(std::pair<PositionIndex,PositionIndex> constrainedSrcSegm,
                                     std::vector<std::string> constrainedTrans,
                                     const SourceSegmentation& sourceSegmentation,
                                     const std::vector<PositionIndex>& targetSegmentCuts,
                                     const std::vector<std::string>& targetWordVec)const;
};

//--------------- TranslationMetadata class functions
//

//---------------------------------------
template<class SCORE_INFO>
TranslationMetadata<SCORE_INFO>::TranslationMetadata(void)
{
  std::string tag;

      // Insert xml tags
  tag=obtainStartTag(XML_PHR_ANNOT_TAG_NAME);
  xmlTagSet.insert(tag);
  tag=obtainEndTag(XML_PHR_ANNOT_TAG_NAME);
  xmlTagSet.insert(tag);

  tag=obtainStartTag(XML_SRC_SEGM_TAG_NAME);
  xmlTagSet.insert(tag);
  tag=obtainEndTag(XML_SRC_SEGM_TAG_NAME);
  xmlTagSet.insert(tag);

  tag=obtainStartTag(XML_TRG_SEGM_TAG_NAME);
  xmlTagSet.insert(tag);
  tag=obtainEndTag(XML_TRG_SEGM_TAG_NAME);
  xmlTagSet.insert(tag);
}

//---------------------------------------
template<class SCORE_INFO>
void TranslationMetadata<SCORE_INFO>::obtainTransConstraints(std::string rawSrcSent,int verbosity/*=0*/)
{
  if(rawSrcSent.empty())
  {
        // Initialize data members
    srcSentVec.clear();
    srcPhrTransMap.clear();
  }
  else
  {
        // Initialize data members
    srcSentVec.clear();
    srcPhrTransMap.clear();

        // Tokenize raw source sentence
    std::string tokRawSrcSent=tokenizeSrcSentence(rawSrcSent);
  
        // Convert raw source sentence into a string vector
    std::vector<std::string> tokRawSrcSentVec=StrProcUtils::stringToStringVector(tokRawSrcSent);

        // Scan string vector
    bool end=false;
    unsigned int i=0;
    std::vector<std::string> srcPhrase;
    std::vector<std::string> trgPhrase;
    unsigned int finalPos;
  
    while(!end)
    {
      if(constraintFound(tokRawSrcSentVec,i,srcPhrase,trgPhrase,finalPos))
      {
            // A constraint was found

            // Obtain start and end source position
            // 
            // NOTE: index of first source word is 1 (0 is used to refer
            // to the NULL word)
        unsigned int startSrcPos=srcSentVec.size()+1;
        unsigned int endSrcPos=startSrcPos+srcPhrase.size()-1;
      
            // Update source sentence vector
        for(unsigned int j=0;j<srcPhrase.size();++j)
          srcSentVec.push_back(srcPhrase[j]);

            // Update source to target constraint map
        srcPhrTransMap[std::make_pair(startSrcPos,endSrcPos)]=trgPhrase;

            // Set new value for i
        i=finalPos+1;
      }
      else
      {
            // Constraint not found, add source word to srcSentVec
        srcSentVec.push_back(tokRawSrcSentVec[i]);
      
            // Increase i
        i+=1;
      }
          // Check ending condition
      if(i>=tokRawSrcSentVec.size())
        end=true;
    }

        // Print verbose information if requested
    if(verbosity>0)
    {
      if(srcPhrTransMap.size()>0)
      {
            // Print source sentence without xml tags
        std::cerr<<"Source sentence without xml tags:";
        for(unsigned int i=0;i<srcSentVec.size();++i)
        {
          std::cerr<<" "<<srcSentVec[i];
        }
        std::cerr<<std::endl;
      
            // Print translations constraints
        std::cerr<<"Translation constraints:";
        std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> >::const_iterator const_iter;
        for(const_iter=srcPhrTransMap.begin();const_iter!=srcPhrTransMap.end();++const_iter)
        {
          std::cerr<<" "<<const_iter->first.first<<","<<const_iter->first.second<<" ->";
          for(unsigned int i=0;i<const_iter->second.size();++i)
          {
            std::cerr<<" "<<const_iter->second[i];
          } 
          std::cerr<<" ;";
        }
        std::cerr<<std::endl;
      }
    }
  }
}

//---------------------------------------
template<class SCORE_INFO>
std::vector<std::string> TranslationMetadata<SCORE_INFO>::getSrcSentVec(void)const
{
  return srcSentVec;
}

//---------------------------------------
template<class SCORE_INFO>
std::vector<std::string> TranslationMetadata<SCORE_INFO>::getTransForSrcPhr(std::pair<PositionIndex,PositionIndex> srcPhr)const
{
      // Find translation for source phrase if it exists
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> >::const_iterator const_iter;

  const_iter=srcPhrTransMap.find(srcPhr);
  if(const_iter!=srcPhrTransMap.end())
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
std::set<std::pair<PositionIndex,PositionIndex> > TranslationMetadata<SCORE_INFO>::getConstrainedSrcPhrases(void)const
{
      // Initialize variables
  std::set<std::pair<PositionIndex,PositionIndex> > result;
  
      // Iterate over constraints
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> >::const_iterator const_iter;
  for(const_iter=srcPhrTransMap.begin();const_iter!=srcPhrTransMap.end();++const_iter)
  {
    result.insert(const_iter->first);
  }

      // Return result
  return result;
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::srcPhrAffectedByConstraint(std::pair<PositionIndex,PositionIndex> srcPhr)const
{
      // Iterate over constraints
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> >::const_iterator const_iter;
  for(const_iter=srcPhrTransMap.begin();const_iter!=srcPhrTransMap.end();++const_iter)
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
std::string TranslationMetadata<SCORE_INFO>::tokenizeSrcSentence(std::string srcSent)const
{
  std::string result;
  
      // Scan string and build result
  unsigned int i=0;
  while(i<srcSent.size())
  {
    unsigned int endTagPos;
    if(xmlTag(srcSent,i,endTagPos))
    {
          // Add initial blank to result if necessary
      if(i>0 && srcSent[i-1]!=' ')
        result+=" ";
          
          // Add xml tag to result
      for(;i<=endTagPos;++i)
        result+=srcSent[i];
      
          // Add final blank to result if necessary
      if(srcSent.size()>endTagPos && srcSent[endTagPos+1]!=' ')
        result+=" ";
    }
    else
    {
          // No xml tag was identified
      result+=srcSent[i];
      i+=1;
    }
  }

      // Return result
  return result;
}

//---------------------------------------
template<class SCORE_INFO>
std::string TranslationMetadata<SCORE_INFO>::obtainStartTag(std::string tagName)const
{
  std::string tag="<";
  tag+=tagName;
  tag+=">";
  return tag;
}

//---------------------------------------
template<class SCORE_INFO>
std::string TranslationMetadata<SCORE_INFO>::obtainEndTag(std::string tagName)const
{
  std::string tag="</";
  tag+=tagName;
  tag+=">";
  return tag;
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::xmlTag(std::string srcSent,
                                             unsigned int initialPos,
                                             unsigned int& endTagPos)const
{
  if(srcSent[initialPos]=='<')
  {
    std::string auxStr="<";
    unsigned int i=initialPos+1;
    while(true)
    {
      if(i>=srcSent.size())
      {
        return false;
      }
      else
      {
            // Add next character
        auxStr+=srcSent[i];

            // Check if end of tag symbol was found
        if(srcSent[i]=='>')
        {
              // Check if found tag name is a valid one
          std::set<std::string>::const_iterator const_iter=xmlTagSet.find(auxStr);

          if(const_iter!=xmlTagSet.end())
          {
            endTagPos=i;
            return true;
          }
          else
          {
            return false;
          }
        }
            // Check if a blank symbol was found
        if(srcSent[i]==' ')
        {
          return false;
        }
        
        ++i;
      }
    }
  }
  else
  {
    return false;
  }
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::constraintFound(std::vector<std::string> tokRawSrcSentVec,
                                                      unsigned int currPos,
                                                      std::vector<std::string>& srcPhrase,
                                                      std::vector<std::string>& trgPhrase,
                                                      unsigned int& finalPos)const
{
      // Initialize variables
  unsigned int i=currPos;
  std::vector<std::string> auxSrcPhrase;
  std::vector<std::string> auxTrgPhrase;

      // Scan tokens
      
      // No tag has been observed
  if(i<tokRawSrcSentVec.size() && tokRawSrcSentVec[i]==obtainStartTag(XML_PHR_ANNOT_TAG_NAME))
  {
    i+=1;
  }
  else
  {
    return false;
  }

      // XML_PHR_ANNOT_TAG_NAME has been observed
  if(i<tokRawSrcSentVec.size() && tokRawSrcSentVec[i]==obtainStartTag(XML_SRC_SEGM_TAG_NAME))
  {
    i+=1;
        // Read source segment if it is provided
    while(i<tokRawSrcSentVec.size() && tokRawSrcSentVec[i]!=obtainEndTag(XML_SRC_SEGM_TAG_NAME))
    {
      auxSrcPhrase.push_back(tokRawSrcSentVec[i]);
      i+=1;
    }
        // Check if end tag was found
    if(i>=tokRawSrcSentVec.size())
      return false;

        // Check if source phrase has at least one word
    if(auxSrcPhrase.size()==0)
      return false;

        // Increase value of i
    i+=1;
  }
  else
  {
    return false;
  }

      // XML_SRC_SEGM_TAG_NAME has been observed
  if(i<tokRawSrcSentVec.size() && tokRawSrcSentVec[i]==obtainStartTag(XML_TRG_SEGM_TAG_NAME))
  {
    i+=1;
        // Read target segment if it is provided
    while(i<tokRawSrcSentVec.size() && tokRawSrcSentVec[i]!=obtainEndTag(XML_TRG_SEGM_TAG_NAME))
    {
      auxTrgPhrase.push_back(tokRawSrcSentVec[i]);
      i+=1;
    }
        // Check if end tag was found
    if(i>=tokRawSrcSentVec.size())
      return false;

        // Check if target phrase has at least one word
    if(auxTrgPhrase.size()==0)
      return false;

        // Increase value of i
    i+=1;
  }
  else
  {
    return false;
  }

        // XML_TRG_SEGM_TAG_NAME has been observed
  if(i<tokRawSrcSentVec.size() && tokRawSrcSentVec[i]==obtainEndTag(XML_PHR_ANNOT_TAG_NAME))
  {
        // Set values of output variables
    srcPhrase=auxSrcPhrase;
    trgPhrase=auxTrgPhrase;
    finalPos=i;
    
    return true;
  }
  else
  {
    return false;
  }
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::translationSatisfiesConstraints(const SourceSegmentation& sourceSegmentation,
                                                                      const std::vector<PositionIndex>& targetSegmentCuts,
                                                                      const std::vector<std::string>& targetWordVec)const
{
  if(transViolatesSrcPhrConstraints(sourceSegmentation,targetSegmentCuts,targetWordVec))
    return false;
  
  return true;
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::phraseTranslationIsValid(const std::vector<std::string>& /*sourceWordVec*/,
                                                               const std::vector<std::string>& /*targetWordVec*/)const
{
  return true;
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::transViolatesSrcPhrConstraints(const SourceSegmentation& sourceSegmentation,
                                                                     const std::vector<PositionIndex>& targetSegmentCuts,
                                                                     const std::vector<std::string>& targetWordVec)const
{
      // Iterate over all of the source phrase constraints
  std::map<std::pair<PositionIndex,PositionIndex>,std::vector<std::string> >::const_iterator iter;
  for(iter=srcPhrTransMap.begin();iter!=srcPhrTransMap.end();++iter)
  {
    if(transViolatesSrcPhrConstraint(iter->first,
                                     iter->second,
                                     sourceSegmentation,
                                     targetSegmentCuts,
                                     targetWordVec))
      return true;
  }

  return false;
}

//---------------------------------------
template<class SCORE_INFO>
bool TranslationMetadata<SCORE_INFO>::transViolatesSrcPhrConstraint(std::pair<PositionIndex,PositionIndex> constrainedSrcSegm,
                                                                    std::vector<std::string> constrainedTrans,
                                                                    const SourceSegmentation& sourceSegmentation,
                                                                    const std::vector<PositionIndex>& targetSegmentCuts,
                                                                    const std::vector<std::string>& targetWordVec)const
{
      // Look for source segments in translation that are related to
      // the constrained one
  std::pair<PositionIndex,PositionIndex> relatedSrcSegm;
  std::pair<PositionIndex,PositionIndex> relatedTrgSegm;
  bool foundSrcSegmRelatedToConstraint=false;
  for(unsigned int i=0;i<sourceSegmentation.size();++i)
  {
    if(constrainedSrcSegm.first>=sourceSegmentation[i].first && constrainedSrcSegm.first<=sourceSegmentation[i].second)
    {
      foundSrcSegmRelatedToConstraint=true;
      relatedSrcSegm=sourceSegmentation[i];
      if(i==0)
        relatedTrgSegm.first=1;
      else
        relatedTrgSegm.first=targetSegmentCuts[i-1]+1;
      relatedTrgSegm.second=targetSegmentCuts[i];
      break;
    }
  }
      // Check if no related source segment has been covered yet in
      // translation
  if(!foundSrcSegmRelatedToConstraint)
    return false;
      
      // Check if related source phrase is the same as that affected
      // by constraint
  if(constrainedSrcSegm==relatedSrcSegm)
  {
        // Check if translation is equal to the constrained one
    if(constrainedTrans.size()!=relatedTrgSegm.second-relatedTrgSegm.first+1)
      return true;
    for(unsigned int j=0;j<constrainedTrans.size();++j)
    {
      if(constrainedTrans[j]!=targetWordVec[relatedTrgSegm.first+j-1])
        return true;
    }
        // Translation is equal to the constrained one
    return false;
  }
  else
    return true;
}

//---------------------------------------
template<class SCORE_INFO>
void TranslationMetadata<SCORE_INFO>::clear(void)
{
  srcSentVec.clear();
  srcPhrTransMap.clear();
}

#endif
