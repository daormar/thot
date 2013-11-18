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

#ifndef _ThotIbmMaxConfidFactory_h
#define _ThotIbmMaxConfidFactory_h

//--------------- Include files --------------------------------------

#include "ThotIbmMaxConfid.h"

//--------------- Classes --------------------------------------------

class ThotIbmMaxConfidFactory: public IConfidenceFactory {
  Logger *_logger;
  ThotIbmMaxConfid *rc;

public:
  ThotIbmMaxConfidFactory(): _logger(0) { }
  // do not forget to free all allocated resources
  // otherwise define the destructor with an empty body
  virtual ~ThotIbmMaxConfidFactory() {
    // TODO: delete rc here?;
    LOG(INFO) << "IBMConfidencer freed.";
    delete rc;
  };


  // creation and initialization of the CM
  // only the prefix of the files is required
  virtual int init(int argc, char *argv[], Context *context = 0);

  virtual string getVersion() { return "IBMConfidencer"; }

  virtual void setLogger(Logger *logger) {
    _logger = logger;
    LOG(INFO) << "ThotIbmMaxConfidFactory is joining the logger";
  }

  virtual IConfidenceEngine *createInstance(const string &specialization_id = "") {
    //TODO: consider to generate one new based on specialization_id
    return rc;
  }
  virtual void deleteInstance(IConfidenceEngine *instance) {
    //delete instance;
  }

};

#endif
