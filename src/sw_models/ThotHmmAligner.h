/*
thot package for statistical machine translation
Copyright (C) 2013 Germ\'an Sanchis, Daniel Ortiz-Mart\'inez
 
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

#ifndef _ThotHmmAligner_h
#define _ThotHmmAligner_h

//--------------- Include files --------------------------------------

#include <iostream>
#include <iomanip>
#include "IncrHmmAligModel.h"
#include <casmacat/IAlignmentEngine.h>
#include <casmacat/IPluginFactory.h>
#include <casmacat/utils.h>

using namespace std;
using namespace casmacat;

//--------------- Classes --------------------------------------------

class ThotHmmAligner: public IAlignmentEngine, Loggable {
  IncrHmmAligModel aligModel;
  Logger *_logger;
public:
  ThotHmmAligner(): _logger(0) { }

  virtual ~ThotHmmAligner() {
    LOG(INFO) << "ThotHmmAligner is now free!" << endl;
//     delete &aligModel;
  }

  virtual void align(const vector<string> &source,
                     const vector<string> &target,
                     vector< vector<float> > &alignments); 

  virtual void update(const std::vector<std::string> &source,
                      const std::vector<std::string> &target) {}

  virtual void setLogger(Logger *logger) {
    _logger = logger;
    LOG(INFO) << "IBMAligner is joining the logger!" << endl;
  }

  int init(char* filesPrefix);
};

#endif
