/*
thot package for statistical machine translation
Copyright (C) 2013 Vicent Alabau, Jes\'us Gonz\'alez-Rubio, Daniel Ortiz-Mart\'inez
 
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

#ifndef _ThotIbmMaxConfid_h
#define _ThotIbmMaxConfid_h

//--------------- Include files --------------------------------------

#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <string>
#include "SmoothedIncrIbm1AligModel.h"
#include <cctype>
#include <algorithm>  

//#include <casmacat/config.h>
#include <casmacat/IConfidenceEngine.h>
#include <casmacat/IPluginFactory.h>
#include <casmacat/utils.h>

using namespace std;
using namespace casmacat;

//--------------- Classes --------------------------------------------

class ThotIbmMaxConfid: public IConfidenceEngine, Loggable {
  Logger *_logger;
  SmoothedIncrIbm1AligModel _ibm;
public:
  ThotIbmMaxConfid(): _logger(0) { }
  
  // do not forget to free all allocated resources
  // otherwise define the destructor with an empty body
  virtual ~ThotIbmMaxConfid() { LOG(INFO) << "ThotIbmMaxConfid am freed." << endl; };
	
  virtual float getWordConfidences(const vector<string> &source,
                                   const vector<string> &target,
                                   const vector<bool> &validated,
                                   vector<float> &confidences);

  virtual float getSentenceConfidence(const vector<string> &source,
                                      const vector<string> &target,
                                      const vector<bool> &validated);
	
  virtual void update(const std::vector<std::string> &source,
                      const std::vector<std::string> &target);

  virtual void setLogger(Logger *logger);
	
  // cargamos los modelos ibm a partir del prefijo de los ficheros
  int init(string filesPrefix)
  {
    
    LOG(INFO) << "Initializing confidencer..." << endl;
    _ibm.load(filesPrefix.c_str());
    
    return EXIT_SUCCESS;
  }

};

#endif
