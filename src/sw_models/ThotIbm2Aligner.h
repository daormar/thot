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

#ifndef _ThotIbm2Aligner_h
#define _ThotIbm2Aligner_h

//--------------- Include files --------------------------------------

#include <iostream>
#include <iomanip>
#include "SmoothedIncrIbm2AligModel.h"
#include <casmacat/IAlignmentEngine.h>
#include <casmacat/IPluginFactory.h>
#include <casmacat/utils.h>

using namespace casmacat;

//--------------- Classes --------------------------------------------

class ThotIbm2Aligner: public IAlignmentEngine, Loggable {
  SmoothedIncrIbm2AligModel aligModel;
  Logger *_logger;
public:
  ThotIbm2Aligner(): _logger(0) { }

  virtual ~ThotIbm2Aligner() {
    LOG(INFO) << "ThotIbm2Aligner is now free!" << std::endl;
//     delete &aligModel;
  }

  virtual void align(const vector<string> &source,
                     const vector<string> &target,
                     vector< vector<float> > &alignments); 

  virtual void update(const std::vector<std::string> &source,
                      const std::vector<std::string> &target) {}

  virtual void setLogger(Logger *logger) {
    _logger = logger;
    LOG(INFO) << "IBMAligner is joining the logger!" << std::endl;
  }

  int init(char* filesPrefix);
};

#endif
