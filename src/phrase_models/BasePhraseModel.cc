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
/* Module: BasePhraseModel                                          */
/*                                                                  */
/* Definitions file: BasePhraseModel.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "BasePhraseModel.h"

//--------------- Function definitions


//-------------------------
Prob BasePhraseModel::strPt_s_(const Vector<string>& s,
                               const Vector<string>& t)
{
  return exp((double)strLogpt_s_(s,t));
}

//-------------------------
LgProb BasePhraseModel::strLogpt_s_(const Vector<string>& s,
                                    const Vector<string>& t)
{
  Vector<WordIndex> wIndex_s,wIndex_t;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return logpt_s_(wIndex_s,wIndex_t);
}

//-------------------------
Prob BasePhraseModel::pt_s_(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t)
{
  return exp((double)logpt_s_(s,t));
}

//-------------------------
Prob BasePhraseModel::strPs_t_(const Vector<string>& s,
                               const Vector<string>& t)
{
  return exp((double)strLogps_t_(s,t));
}

//-------------------------
LgProb BasePhraseModel::strLogps_t_(const Vector<string>& s,
                                    const Vector<string>& t)
{
  Vector<WordIndex> wIndex_s,wIndex_t;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return logps_t_(wIndex_s,wIndex_t);
}

//-------------------------
Prob BasePhraseModel::ps_t_(const Vector<WordIndex>& s,
                            const Vector<WordIndex>& t)
{
  return exp((double)logps_t_(s,t));
}
    
//-------------------------
bool BasePhraseModel::strGetTransFor_s_(const Vector<string>& s,
                                        TrgTableNode& trgtn)
{
  Vector<WordIndex> wIndex_s;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  return getTransFor_s_(wIndex_s,trgtn);  
}

//-------------------------
bool BasePhraseModel::strGetTransFor_t_(const Vector<string>& t,
                                        SrcTableNode& srctn)
{
  Vector<WordIndex> wIndex_t;

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return getTransFor_t_(wIndex_t,srctn);
}

//-------------------------
bool BasePhraseModel::strGetNbestTransFor_s_(const Vector<string>& s,
                                             NbestTableNode<PhraseTransTableNodeData>& nbt)
{
  Vector<WordIndex> wIndex_s;

  for(unsigned int i=0;i<s.size();++i)
    wIndex_s.push_back(stringToSrcWordIndex(s[i]));

  return getNbestTransFor_s_(wIndex_s,nbt);
}

//-------------------------
bool BasePhraseModel::strGetNbestTransFor_t_(const Vector<string>& t,
                                             NbestTableNode<PhraseTransTableNodeData>& nbt,
                                             int N/*=-1*/)
{
  Vector<WordIndex> wIndex_t;

  for(unsigned int i=0;i<t.size();++i)
    wIndex_t.push_back(stringToTrgWordIndex(t[i]));

  return getNbestTransFor_t_(wIndex_t,nbt,N);
}

//-------------------------
int BasePhraseModel::trainSentPair(const Vector<std::string>& /*srcSentStrVec*/,
                                   const Vector<std::string>& /*trgSentStrVec*/,
                                   Count /*c*/,
                                   int /*verbose*/)
{
  cerr<<"Warning: Phrase-based model training of a sentence pair was requested, but such functionality is not provided!"<<endl;
  return ERROR;
}

//-------------------------
int BasePhraseModel::trainBilPhrases(const Vector<Vector<std::string> >& /*srcPhrVec*/,
                                     const Vector<Vector<std::string> >& /*trgPhrVec*/,
                                     Count /*c*/,
                                     Count /*lowerBound*/,
                                     int /*verbose*/)
{
  cerr<<"Warning: Phrase-based model training of bilingual phrases was requested, but such functionality is not provided!"<<endl;
  return ERROR;
}

//---------------
std::string BasePhraseModel::absolutizeModelFileName(std::string descFileName,
                                                     std::string modelFileName)
{
  if(modelFileName.empty())
    return modelFileName;
  else
  {
        // Check if path is already absolute
    if(modelFileName[0]=='/')
    {
          // Path is absolute
      return modelFileName;
    }
    else
    {
          // Path is not absolute
      if(descFileName.empty())
      {
        return modelFileName;
      }
      else
      {
            // Absolutize model file name using directory name contained
            // in descriptor file path
        return extractDirName(descFileName)+modelFileName;
      }
    }
  }
}

//---------------
std::string BasePhraseModel::extractDirName(std::string filePath)
{
  if(filePath.empty())
  {
    std::string dirName;
    return dirName;
  }
  else
  {
        // Provided file path is not empty
    int last_slash_pos=-1;

        // Find last position of slash symbol
    for(unsigned int i=0;i<filePath.size();++i)
      if(filePath[i]=='/')
        last_slash_pos=i;

        // Check if any slash symbols were found
    if(last_slash_pos==-1)
    {
      std::string dirName;
      return dirName;
    }
    else
    {
          // The last slash symbol was found at "last_slash_pos"
      return filePath.substr(0,last_slash_pos+1);
    }
  }
}

//---------------
bool BasePhraseModel::fileIsDescriptor(std::string fileName,
                                       std::string& mainFileName)
{
  awkInputStream awk;
  if(awk.open(fileName.c_str())==ERROR)
    return false;
  else
  {
    if(awk.getln())
    {
      if(awk.NF>=3 && awk.dollar(1)=="thot" && awk.dollar(2)=="tm" && awk.dollar(3)=="descriptor")
      {
            // Process descriptor (main file will be read)
        while(awk.getln())
        {
          if(awk.NF>=3 && awk.dollar(3)=="main")
          {
                // File is a descriptor and main file was found
            mainFileName=awk.dollar(2);
            awk.close();
            return true;
          }
          else
          {
                // Check if old descriptor file is being processed
            if(awk.NF>=3 && awk.dollar(2)=="main")
            {
                  // File is an old descriptor and main file was found
              mainFileName=awk.dollar(1);
              awk.close();
              return true;
            }
          }
        }
            // File is not a descriptor since it does not incorporate a
            // main language model
        return false;
      }
      else
      {
            // File is not a descriptor
        awk.close();
        return false;
      }
    }
    else
    {
          // File is empty
      awk.close();
      return false;
    }
  }
}

//-------------------------
