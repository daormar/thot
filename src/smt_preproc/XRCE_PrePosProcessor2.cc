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
/* Module: XRCE_PrePosProcessor2                                    */
/*                                                                  */
/* Definitions file: XRCE_PrePosProcessor2.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "XRCE_PrePosProcessor2.h"


//--------------- XRCE_PrePosProcessor2 class functions
//

//---------------------------------------
XRCE_PrePosProcessor2::XRCE_PrePosProcessor2()
{
  
}

//---------------------------------------
bool XRCE_PrePosProcessor2::loadCapitInfo(const char* filename)
{
  std::string capOptFileName;
  bool err;

  clear();
  
  capOptFileName=filename;
  capOptFileName=capOptFileName+".cap";
  err=loadCapitOptions(capOptFileName.c_str());
  if(err==THOT_OK)
  {
    err=loadLangModelForSmtPreproc(filename);
    return err;
  }
  else
  {
    return ERROR;
  }
}

//---------------------------------------
std::string XRCE_PrePosProcessor2::preprocLine(std::string str,
                                               bool caseconv,
                                               bool keepPreprocInfo)
{
  if(keepPreprocInfo)
    lastPreprocStr="";
  else
    lastPreprocStr=XRCEtokLine(str.c_str(),false);
  return XRCEpprocLine(str.c_str(),caseconv,keepPreprocInfo);
      // when last argument of XRCEpprocLine is true its internal tables
      // are modified.
      // If caseconv==true, then the input string is lowercased.
}

//---------------------------------------
std::string XRCE_PrePosProcessor2::postprocLine(std::string str,
                                                bool caseconv)
{
  std::string pprocResult;

  if(caseconv)
    pprocResult=capitalize(str);
  else
    pprocResult=str;
      
  pprocResult=XRCEpostprocLine(pprocResult.c_str(),false);

  return pprocResult;
}

//---------------------------------------
bool XRCE_PrePosProcessor2::isCategory(std::string word)
{
  if(word.size()==0) return false;
  else
  {
    if(word[word.size()-1]=='>')
      return true;
    else
      return false;
  }
}

//---------------------------------------
std::string XRCE_PrePosProcessor2::capitalize(std::string str)
{
  Vector<std::string> strVec=StrProcUtils::stringToStringVector(str);
  Vector<std::string> lastPreprocStrVec=StrProcUtils::stringToStringVector(lastPreprocStr);
  Vector<std::string> resultVec;
  std::string best_word;
  std::map<std::string,Vector<std::string> >::iterator mapCapitIter;
  IncrJelMerNgramLM::LM_State lm_state;
  unsigned int i=0;
  
  lmodel.getStateForBeginOfSentence(lm_state);
  
      // Update lm_state with the prefix words
  if(lastPreprocStrVec.size()>1)
  {
    for(i=0;i<lastPreprocStrVec.size()-1;++i)
    {
      lmodel.getNgramLgProbGivenStateStr(lastPreprocStrVec[i],lm_state);
      resultVec.push_back(lastPreprocStrVec[i]);
    }
  }  
      // Capitalize the last prefix word
  if(lastPreprocStrVec.size()>=1)
  {
    best_word=strVec[i];
        // Obtain capitalization options
    mapCapitIter=capitMap.find(strVec[i]);
    if(mapCapitIter!=capitMap.end())
    {
      IncrJelMerNgramLM::LM_State lm_state_aux;
      LgProb best_lgp=-FLT_MAX;
      LgProb lgp;
          // Score capitalization options
      for(unsigned int j=0;j<mapCapitIter->second.size();++j)
      {
        std::string cap_opt=mapCapitIter->second[j];

            // A capitalization option is taken into account if it is
            // compatible with the prefix  
        if(strncmp(strVec[i].c_str(),cap_opt.c_str(),strVec[i].size())==0)
        {
          lm_state_aux=lm_state;
          lgp=lmodel.getNgramLgProbGivenStateStr(cap_opt,lm_state_aux);
          if(best_lgp<lgp)
          {
            best_lgp=lgp;
            best_word=mapCapitIter->second[j];
          }
        }
      }
    }
    lmodel.getNgramLgProbGivenStateStr(best_word,lm_state);
    resultVec.push_back(best_word);
    ++i;
  }

      // Capitalize the words after the last prefix word
  for(;i<strVec.size();++i)
  {
    best_word=strVec[i];

        // Obtain capitalization options
    mapCapitIter=capitMap.find(strVec[i]);
    if(mapCapitIter!=capitMap.end())
    {
      IncrJelMerNgramLM::LM_State lm_state_aux;
      LgProb best_lgp=-FLT_MAX;
      LgProb lgp;
          // Score capitalization options
      for(unsigned int j=0;j<mapCapitIter->second.size();++j)
      {
        std::string cap_opt=mapCapitIter->second[j];
        lm_state_aux=lm_state;
        lgp=lmodel.getNgramLgProbGivenStateStr(cap_opt,lm_state_aux);
        if(best_lgp<lgp)
        {
          best_lgp=lgp;
          best_word=mapCapitIter->second[j];
        }
      }
    }
    resultVec.push_back(best_word);
    if(strVec[i]!=END_OF_PREF_MARK)
      lmodel.getNgramLgProbGivenStateStr(best_word,lm_state);
  }
  return StrProcUtils::stringVectorToString(resultVec);
}

//---------------------------------------
bool XRCE_PrePosProcessor2::loadCapitOptions(const char* filename)
{
  awkInputStream capitInfoStream;

      // Open file
  if(capitInfoStream.open(filename)==ERROR)
  {
    cerr<<"Error while loading file with capitalization options: "<<filename<<endl;
    return ERROR;
  }
  else
  {
    unsigned int lineNo=0;
    Vector<std::string> strVec;
    std::string canonicStr;
    
    cerr<<"Reading capitalization information from file: "<<filename<<endl;
    
    while(capitInfoStream.getln())
    {      
      ++lineNo;
      canonicStr=capitInfoStream.dollar(1);
      if(capitInfoStream.NF>2)
      {
        strVec.clear();
        for(unsigned int i=3;i<=capitInfoStream.NF;++i)
        {
          strVec.push_back(capitInfoStream.dollar(i));
        }
        capitMap.insert(make_pair(canonicStr,strVec));
      }
      else
      {
        cerr<<"Anomalous entry on line "<<lineNo<<endl;
      }
    }
    return THOT_OK;
  }
}

//---------------------------------------
bool XRCE_PrePosProcessor2::loadLangModelForSmtPreproc(const char* prefixFileName)
{
  languageModelFileName=prefixFileName;
  languageModelFileName=languageModelFileName+".lm";
  if(lmodel.load(languageModelFileName.c_str())==ERROR)
    return ERROR;
  else return THOT_OK;
}

//---------------------------------------
void XRCE_PrePosProcessor2::clear(void)
{
  capitMap.clear();
  lmodel.clear();
}

//---------------------------------------
XRCE_PrePosProcessor2::~XRCE_PrePosProcessor2()
{
  
}
