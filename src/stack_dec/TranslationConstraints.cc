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
/* Module: TranslationConstraints                                   */
/*                                                                  */
/* Definitions file: TranslationConstraints.cc                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "TranslationConstraints.h"

//--------------- TranslationConstraints class functions
//

//---------------------------------------
TranslationConstraints::TranslationConstraints(void)
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
void TranslationConstraints::obtainTransConstraints(std::string rawSrcSent,int verbosity/*=0*/)
{
      // Initialize data members
  srcSentVec.clear();
  srcPhrTransMap.clear();
  
      // Tokenize raw source sentence
  std::string tokRawSrcSent=tokenizeSrcSentence(rawSrcSent);
  
      // Convert raw source sentence into a string vector
  Vector<std::string> tokRawSrcSentVec=StrProcUtils::stringToStringVector(tokRawSrcSent);

      // Scan string vector
  bool end=false;
  unsigned int i=0;
  Vector<std::string> srcPhrase;
  Vector<std::string> trgPhrase;
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
      srcPhrTransMap[make_pair(startSrcPos,endSrcPos)]=trgPhrase;

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
      cerr<<"Source sentence without xml tags:";
      for(unsigned int i=0;i<srcSentVec.size();++i)
      {
        cerr<<" "<<srcSentVec[i];
      }
      cerr<<endl;
      
          // Print translations constraints
      cerr<<"Translation constraints:";
      std::map<pair<PositionIndex,PositionIndex>,Vector<std::string> >::const_iterator const_iter;
      for(const_iter=srcPhrTransMap.begin();const_iter!=srcPhrTransMap.end();++const_iter)
      {
        cerr<<" "<<const_iter->first.first<<","<<const_iter->first.second<<" ->";
        for(unsigned int i=0;i<const_iter->second.size();++i)
        {
          cerr<<" "<<const_iter->second[i];
        } 
        cerr<<" ;";
      }
      cerr<<endl;
    }
  }
}

//---------------------------------------
Vector<std::string> TranslationConstraints::getSrcSentVec(void)const
{
  return srcSentVec;
}

//---------------------------------------
Vector<std::string> TranslationConstraints::getTransForSrcPhr(pair<PositionIndex,PositionIndex> srcPhr)const
{
      // Find translation for source phrase if it exists
  std::map<pair<PositionIndex,PositionIndex>,Vector<std::string> >::const_iterator const_iter;

  const_iter=srcPhrTransMap.find(srcPhr);
  if(const_iter!=srcPhrTransMap.end())
  {
    return const_iter->second;
  }
  else
  {
    Vector<std::string> strVec;
    return strVec;
  }
}

//---------------------------------------
std::set<pair<PositionIndex,PositionIndex> > TranslationConstraints::getConstrainedSrcPhrases(void)const
{
      // Initialize variables
  std::set<pair<PositionIndex,PositionIndex> > result;
  
      // Iterate over constraints
  std::map<pair<PositionIndex,PositionIndex>,Vector<std::string> >::const_iterator const_iter;
  for(const_iter=srcPhrTransMap.begin();const_iter!=srcPhrTransMap.end();++const_iter)
  {
    result.insert(const_iter->first);
  }

      // Return result
  return result;
}

//---------------------------------------
bool TranslationConstraints::srcPhrAffectedByConstraint(pair<PositionIndex,PositionIndex> srcPhr)const
{
      // Iterate over constraints
  std::map<pair<PositionIndex,PositionIndex>,Vector<std::string> >::const_iterator const_iter;
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
std::string TranslationConstraints::tokenizeSrcSentence(std::string srcSent)const
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
std::string TranslationConstraints::obtainStartTag(std::string tagName)const
{
  std::string tag="<";
  tag+=tagName;
  tag+=">";
  return tag;
}

//---------------------------------------
std::string TranslationConstraints::obtainEndTag(std::string tagName)const
{
  std::string tag="</";
  tag+=tagName;
  tag+=">";
  return tag;
}

//---------------------------------------
bool TranslationConstraints::xmlTag(std::string srcSent,
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
bool TranslationConstraints::constraintFound(Vector<std::string> tokRawSrcSentVec,
                                             unsigned int currPos,
                                             Vector<std::string>& srcPhrase,
                                             Vector<std::string>& trgPhrase,
                                             unsigned int& finalPos)const
{
      // Initialize variables
  unsigned int i=currPos;
  Vector<std::string> auxSrcPhrase;
  Vector<std::string> auxTrgPhrase;

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
void TranslationConstraints::clear(void)
{
  xmlTagSet.clear();
  srcSentVec.clear();
  srcPhrTransMap.clear();
}
