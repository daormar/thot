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
/* Module: DynClassFileHandler                                      */
/*                                                                  */
/* Definitions file: DynClassFileHandler.cc                         */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "DynClassFileHandler.h"

//--------------- DynClassFileHandler class function definitions

//---------------------------------------
DynClassFileHandler::DynClassFileHandler(void)
{
}

//---------------------------------------
bool DynClassFileHandler::load(std::string _fileName,int verbose/*=1*/)
{
  fileName=_fileName;
  
  AwkInputStream awk;
  
  if(awk.open(fileName.c_str())==THOT_ERROR)
  {
    if(verbose)
      std::cerr<<"Error while opening file with dynamic class information: "<<fileName<<"\n";
    return THOT_ERROR;
  }
  else
  {
    if(verbose)
      std::cerr<<"Reading dynamic class information file: "<<fileName<<"\n";

        // Clear data structures
    dynClassInfoMap.clear();
    
        // Read file entries
    while(awk.getln())
    {
      if(awk.NF>=4)
      {
            // Check if line is a comment
        std::string firstField=awk.dollar(1);
        bool isComment=false;
        if(!firstField.empty())
        {
          if(firstField=="#")
            isComment=true;
          else
          {
            if(firstField[0]=='#')
              isComment=true;
          }
        }
        
        if(!isComment)
        {
              // Extract entry information
          std::vector<std::string> strVec;
          std::string baseClassName=awk.dollar(1);
          std::string soFileName=StrProcUtils::expandLibDirIfFound(awk.dollar(3));
          std::string initPars;
          for(unsigned int i=5;i<=awk.NF-1;++i)
          {
            initPars+=awk.dollar(i);
            if(i<awk.NF)
              initPars+=" ";
          }
          DynClassInfo dynClassInfo(soFileName,initPars);
          dynClassInfoMap[baseClassName]=dynClassInfo;
          if(verbose)
            std::cerr<<"Found entry for class "<<baseClassName<<", so file: "<<soFileName<<", init parameters: "<<initPars<<std::endl;
        }
      }
    }
    return THOT_OK;
  }
}

//---------------------------------------
int DynClassFileHandler::getInfoForBaseClass(std::string baseClassName,
                                             std::string& soFileName,
                                             std::string& initPars)
{
  DynClassInfoMap::const_iterator dciIter=dynClassInfoMap.find(baseClassName);
  if(dciIter!=dynClassInfoMap.end())
  {
    soFileName=dciIter->second.first;
    initPars=dciIter->second.second;
     
    return THOT_OK;
  }
  else
  {
    return THOT_ERROR;
  }
}

//---------------------------------------
void DynClassFileHandler::clear(void)
{
  fileName.clear();
  dynClassInfoMap.clear();
}

//---------------------------------------
DynClassFileHandler::~DynClassFileHandler()
{
}
