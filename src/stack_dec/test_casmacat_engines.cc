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
/* Module: test.cc                                                  */
/*                                                                  */
/* Definitions file: test.cc                                        */
/*                                                                  */
/* Description: tests the casmacat engines.                         */
/*                                                                  */   
/********************************************************************/


//--------------- Include files --------------------------------------

#include "ThotMtFactory.h"
#include "ThotImtFactory.h"
#include <StrProcUtils.h>
#include <iostream>
#include <iomanip>

using namespace std;

//--------------- Constants ------------------------------------------



//--------------- Function Declarations ------------------------------

int casmacat_mt_engine(void);
int casmacat_imt_engine(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------



//--------------- Function Definitions -------------------------------


//--------------- main function
int main()
{
  int ret;

  ret=casmacat_mt_engine();

  ret=casmacat_imt_engine();

  return ret;
}

//-------------------------
int casmacat_mt_engine(void)
{
  // casmacat mt engine test
  cerr<<"Casmacat mt-engine test"<<endl;

      // Create instance of mt engine
  std::string username="dom";
  int argc_mt=4;
  char* argv_mt[4];
  argv_mt[0]="";
  argv_mt[1]="-c";
  argv_mt[2]="./aux_dirs/cfg_files/casmacat_adapt_wg_test.cfg";
//  argv_mt[2]="./aux_dirs/cfg_files/casmacat_xerox_enes_adapt_wg.cfg";
  argv_mt[3]="--config";

  int ret;  
  ThotMtFactory thotMtFactory;
  ret=thotMtFactory.init(argc_mt,argv_mt);
  if(ret==EXIT_FAILURE)
  {
    cerr<<"Error during initialization of casmacat mt engine"<<endl;
    return EXIT_FAILURE;
  }
  IMtEngine* mt_engine_ptr=thotMtFactory.createInstance(username);

      // Obtain translation of a test sentence
  vector<string> srcVec=StrProcUtils::charItemsToVector("printer");
  vector<string> trgVec;

      // Tokenize source sentence
      //tokenize(source, srcVec);
  cout<<"Source sentence: ";
  copy(srcVec.begin(), srcVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;
      
      // Translate sentence
  cout<< "* Translating sentence..."<<endl;
  mt_engine_ptr->translate(srcVec,trgVec);

      // Print translation
  cout<< "Translation: ";
  copy(trgVec.begin(), trgVec.end(), ostream_iterator<string>(cout, " "));
  cerr<<endl;

      // Extend models
  cout<< "* Extending models..."<<endl;
  mt_engine_ptr->update(srcVec,trgVec);

      // Delete mt engine instance
  thotMtFactory.deleteInstance(mt_engine_ptr);

  return EXIT_SUCCESS;
}

//-------------------------
int casmacat_imt_engine(void)
{
  // casmacat imt engine test
  cerr<<"Casmacat imt-engine test"<<endl;

      // Create instance of imt engine
  std::string username="dom";
  int argc_mt=4;
  char* argv_mt[4];
  argv_mt[0]="";
  argv_mt[1]="-c";
  argv_mt[2]="./aux_dirs/cfg_files/casmacat_adapt_wg_test.cfg";
//  argv_mt[2]="./aux_dirs/cfg_files/casmacat_xerox_enes_adapt_wg.cfg";
  argv_mt[3]="-v";
  
  ThotImtFactory thotImtFactory;
  int ret=thotImtFactory.init(argc_mt,argv_mt);
  if(ret==EXIT_FAILURE)
  {
    cerr<<"Error during initialization of casmacat imt engine"<<endl;
    return EXIT_FAILURE;
  }
  IInteractiveMtEngine* imt_engine_ptr=thotImtFactory.createInstance(username);

      // Create source sentence
  vector<string> srcVec=StrProcUtils::charItemsToVector("all rights reserved .");
  cout<<"Source sentence: ";
  copy(srcVec.begin(), srcVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;

      // Create prefix
  vector<string> prefixVec=StrProcUtils::charItemsToVector("reservados");
  cout<<"target prefix: ";
  copy(prefixVec.begin(), prefixVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;

      // Create new imt session
  IInteractiveMtSession * imtSessionPtr=imt_engine_ptr->newSession(srcVec);  
      
      // Generate suffix
  cout<< "* Obtaining target translation..."<<endl;
  vector<string> suffixVec;
  bool last_token_is_partial=false;
  vector<string> targetVec;
  imtSessionPtr->setPrefix(prefixVec,suffixVec,last_token_is_partial,targetVec);

      // Print target translation
  cout<< "User prefix: ";
  copy(prefixVec.begin(),prefixVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;
  cout<< "Target translation: ";
  copy(targetVec.begin(),targetVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;

      // Reject operation at the beginning of a word
  cout<< "* Testing reject operation at the beginning of a word..."<<endl;
  vector<string> suffixRejVec=StrProcUtils::charItemsToVector("los derechos reservados .");
  last_token_is_partial=false;
  imtSessionPtr->rejectSuffix(prefixVec,suffixRejVec,last_token_is_partial,targetVec);

      // Print target translation
  cout<< "User prefix: ";
  copy(prefixVec.begin(),prefixVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;
  cout<< "Target translation after rejection: ";
  copy(targetVec.begin(),targetVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;

        // Reject operation in the middle of a word
  cout<< "* Testing reject operation in the middle of a word..."<<endl;
  prefixVec.push_back("l");
  suffixRejVec=StrProcUtils::charItemsToVector("os derechos reservados .");
  last_token_is_partial=true;
  imtSessionPtr->rejectSuffix(prefixVec,suffixRejVec,last_token_is_partial,targetVec);

      // Print target translation
  cout<< "User prefix: ";
  copy(prefixVec.begin(),prefixVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;
  cout<< "Target translation after rejection: ";
  copy(targetVec.begin(),targetVec.end(), ostream_iterator<string>(cout, " "));
  cout<<endl;

      // Delete imt session
  imt_engine_ptr->deleteSession(imtSessionPtr);

      // Delete imt engine instance
  thotImtFactory.deleteInstance(imt_engine_ptr);

  return EXIT_SUCCESS;
}

//-------------------------
