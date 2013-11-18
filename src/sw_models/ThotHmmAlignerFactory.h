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

#ifndef _ThotHmmAlignerFactory_h
#define _ThotHmmAlignerFactory_h

//--------------- Include files --------------------------------------

#include "ThotHmmAligner.h"

//--------------- Classes --------------------------------------------

class ThotHmmAlignerFactory: public IAlignmentFactory {
  Logger *_logger;
  ThotHmmAligner *ha;
public:
  ThotHmmAlignerFactory(): _logger(0) { };
  virtual ~ThotHmmAlignerFactory() {
    LOG(INFO) << "ThotHmmAligner is free!" << endl;
     delete ha;
  };

  virtual int init(int argc, char *argv[], Context *context = 0);

  virtual string getVersion() { return "HMM Aligner"; }

  virtual void setLogger(Logger *logger) {
    _logger = logger;
    LOG(INFO) << "ThotHmmAligner is joining the logger!" << endl;
  }

  virtual IAlignmentEngine *createInstance(const std::string &specialization_id = "") {
    return ha;
  }
  virtual void deleteInstance(IAlignmentEngine *instance) {
    //delete instance;
  }
};

#endif
