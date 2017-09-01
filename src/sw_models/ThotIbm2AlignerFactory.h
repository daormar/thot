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

#ifndef _ThotIbm2AlignerFactory_h
#define _ThotIbm2AlignerFactory_h

//--------------- Include files --------------------------------------

#include "ThotIbm2Aligner.h"

//--------------- Classes --------------------------------------------

class ThotIbm2AlignerFactory: public IAlignmentFactory {
  Logger *_logger;
  ThotIbm2Aligner *ibm2a;
public:
  ThotIbm2AlignerFactory(): _logger(0) { };
  virtual ~ThotIbm2AlignerFactory() {
    LOG(INFO) << "ThotIbm2Aligner is free!" << std::endl;
     delete ibm2a;
  };

  virtual int init(int argc, char *argv[], Context *context = 0);

  virtual string getVersion() { return "HMM Aligner"; }

  virtual void setLogger(Logger *logger) {
    _logger = logger;
    LOG(INFO) << "ThotIbm2Aligner is joining the logger!" << std::endl;
  }

  virtual IAlignmentEngine *createInstance(const std::string &specialization_id = "") {
    return ibm2a;
  }
  virtual void deleteInstance(IAlignmentEngine *instance) {
    //delete instance;
  }
};

#endif
