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
/* Module: thot_test                                                */
/*                                                                  */
/* Definitions file: thot_test.cc                                   */
/*                                                                  */
/* Description: Launches unit tests using the cppunit library.      */
/*                                                                  */
/********************************************************************/

/**
 * @file thot_test.h
 *
 * @brief Launches unit tests using the cppunit library. See
 * http://cppunit.sourceforge.net/doc/cvs/cppunit_cookbook.html for a
 * quick tutorial.
 */

//--------------- Include files --------------------------------------

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "ctimer.h"
#include "options.h"
#include "ErrorDefs.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <set>

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------

struct thot_test_pars
{
  
};
  
//--------------- Function Declarations ------------------------------

void version(void);
int handleParameters(int argc,
                     char *argv[],
                     thot_test_pars& pars);
int takeParameters(int argc,
                   char *argv[],
                   thot_test_pars& ttp);
int checkParameters(const thot_test_pars& ttp);
void printUsage(void);
int launch_tests(const thot_test_pars& ttp);

//--------------- Global variables -----------------------------------


//--------------- Function Definitions -------------------------------

//---------------
int main(int argc, char *argv[])
{
      // Take and check parameters
  thot_test_pars ttp;
  if(handleParameters(argc,argv,ttp)==ERROR)
  {
    return ERROR;
  }
  else
  {
        // Launch tests
    int ret=launch_tests(ttp);
    if(ret==ERROR) return ERROR;
    else return OK;
  }
}

//---------------
int launch_tests(const thot_test_pars& ttp)
{
      // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

      // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( suite );

      // Change the default outputter to a compiler error format outputter
  // runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
  //                                                      std::cerr ) );
      // Run the tests.
  bool wasSucessful = runner.run();
  
  // Return error code 1 if the test failed
  return wasSucessful ? 0 : 1;
}

//---------------
int handleParameters(int argc,
                     char *argv[],
                     thot_test_pars& ttp)
{
  if(readOption(argc,argv,"--version")==OK)
  {
    version();
    return ERROR;
  }
  if(readOption(argc,argv,"--help")==OK)
  {
    printUsage();
    return ERROR;   
  }
  if(takeParameters(argc,argv,ttp)==ERROR)
  {
    return ERROR;
  }
  else
  {
    if(checkParameters(ttp)==OK)
    {
      return OK;
    }
    else
    {
      return ERROR;
    }
  }
}

//---------------
int checkParameters(const thot_test_pars& ttp)
{
  return OK;
}

//---------------
int takeParameters(int argc,
                   char *argv[],
                   thot_test_pars& ttp)
{
      // Check if a configuration file was provided
  std::string cfgFileName;
  int err=readSTLstring(argc,argv, "-c", &cfgFileName);
  if(!err)
  {
        // TO-BE-DONE
  }
  return OK;
}

//---------------

void printUsage(void)
{
  cerr << "thot_test              [--help] [--version]"<<endl<<endl;
  cerr << " --help                : Display this help and exit."<<endl;
  cerr << " --version             : Output version information and exit."<<endl;
}

//---------------
void version(void)
{
  cerr<<"thot_test is part of the thot package "<<endl;
  cerr<<"thot version "<<THOT_VERSION<<endl;
  cerr<<"thot is GNU software written by Daniel Ortiz"<<endl;
}
