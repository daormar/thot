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
/* Module: ThotImtFactory                                           */
/*                                                                  */
/* Definitions file: ThotImtFactory.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotImtFactory.h"

//--------------- ThotImtFactory class functions

//--------------------------
int ThotImtFactory::init(int argc,
                         char *argv[],
                         Context *context /*= 0*/)
{
        // Take parameters
  std::string cfgFileName;
  int ret=takeInitPars(argc,argv,pars);
  if(ret==EXIT_FAILURE) return EXIT_FAILURE;

      // Check parameters
  ret=checkPars(pars);
  if(ret==EXIT_FAILURE) return EXIT_FAILURE;
  
      // Init ThotDecoder instance
  int verbose=true;
  ret=thotDecoder.initUsingCfgFile(pars.c_str,tdup,verbose);
  if(ret==THOT_OK) return EXIT_SUCCESS;
  else return EXIT_FAILURE;
}

//--------------------------
IInteractiveMtEngine* ThotImtFactory::createInstance(const std::string &specialization_id /*= ""*/)
{
      // Obtain id for user name
  int id=userNameToUserIdMap.idForUserName(specialization_id);

      // Set user parameters in thotDecoder
  int verbose=true;
  if(thotDecoder.user_id_new(id))
    thotDecoder.initUserPars(id,tdup,verbose);

      // Return engine instance
  return new ThotImtEngine(&userNameToUserIdMap,tdup,&thotDecoder,id,pars.v_given);
}

//--------------------------
void ThotImtFactory::deleteInstance(IInteractiveMtEngine* instancePtr)
{
  delete instancePtr;
}

//--------------------------
int ThotImtFactory::takeInitPars(int argc,
                                char *argv[],
                                ThotImtFactoryInitPars& pars)
{
  int i=1;
  unsigned int matched;
  std::vector<std::string> argv_stl=argv2argv_stl(argc,argv);
  
  while(i<argc)
  {
    matched=0;

        // -c parameter
    if(argv_stl[i]=="-c" && !matched)
    {
      pars.c_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -c parameter."<<std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        pars.c_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // --v parameter
    if(argv_stl[i]=="-v" && !matched)
    {
      pars.v_given=true;
      ++matched;
    }

        // Check if current parameter is not valid
    if(matched==0)
    {
      std::cerr<<"Error: parameter "<<argv_stl[i]<<" not valid."<<std::endl;
      return EXIT_FAILURE;
    }
    ++i;
  }
  return EXIT_SUCCESS;
}

//--------------------------
int ThotImtFactory::checkPars(const ThotImtFactoryInitPars& pars)
{
  if(!pars.c_given) return EXIT_FAILURE;
  else return EXIT_SUCCESS;
}

//---------------- plugins
// This macro creates functions that allow to create instances of
// ThotFactory
// NOTE: ThotImtFactory provides instances of the IInteractiveMtEngine
// class
EXPORT_CASMACAT_PLUGIN_NAME(IInteractiveMtEngine,ThotImtFactory,thot_imt_plugin);
