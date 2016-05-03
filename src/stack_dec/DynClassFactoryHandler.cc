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
/* Module:  DynClassFactoryHandler                                  */
/*                                                                  */
/* Definitions file: DynClassFactoryHandler.cc                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "DynClassFactoryHandler.h"

//--------------- DynClassFactoryHandler struct functions

//--------------------------
DynClassFactoryHandler::DynClassFactoryHandler()
{
}

//--------------------------
int DynClassFactoryHandler::init_smt(std::string fileName)
{
      // Release data structure
  release_smt();

      // Initialize smt dynamic classes...
  
  if(dynClassFileHandler.load(fileName)==ERROR)
  {
    cerr<<"Error while loading ini file"<<endl;
    return ERROR;
  }
      // Define variables to obtain base class infomation
  std::string baseClassName;
  std::string soFileName;
  std::string initPars;

      ///////////// Obtain info for BasePhraseModel class
  baseClassName="BasePhraseModel";
  if(dynClassFileHandler.getInfoForBaseClass(baseClassName,soFileName,initPars)==ERROR)
  {
    cerr<<"Error: ini file does not contain information about "<<baseClassName<<" class"<<endl;
    cerr<<"Please check content of master.ini file or execute \"thot_handle_ini_files -r\" to reset it"<<endl;
    return ERROR;
  }   
      // Load class derived from BasePhraseModel dynamically
  if(!basePhraseModelDynClassLoader.open_module(soFileName))
  {
    cerr<<"Error: so file ("<<soFileName<<") could not be opened"<<endl;
    return ERROR;
  }     
      // Store init parameters for BasePhraseModel
  basePhraseModelInitPars=initPars;
  
  return OK;
}

//--------------------------
void DynClassFactoryHandler::release_smt(void)
{
      // Close modules
  basePhraseModelDynClassLoader.close_module();
}
