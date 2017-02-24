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
/* Module: IncrMuxPhraseModel                                       */
/*                                                                  */
/* Prototype file: IncrMuxPhraseModel                               */
/*                                                                  */
/* Description: Defines the IncrMuxPhraseModel base class.          */
/*              IncrMuxPhraseModel is derived from the              */
/*              abstract class BasePhraseModel.                     */
/*                                                                  */
/********************************************************************/

#ifndef _IncrMuxPhraseModel_h
#define _IncrMuxPhraseModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrMuxPhraseModel.h"
#include "SimpleDynClassLoader.h"
#include <sys/stat.h>
#include <map>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- IncrMuxPhraseModel class

class IncrMuxPhraseModel: public _incrMuxPhraseModel
{
 public:

    typedef _incrMuxPhraseModel::SrcTableNode SrcTableNode;
    typedef _incrMuxPhraseModel::TrgTableNode TrgTableNode;

        // Constructor
    IncrMuxPhraseModel(void);

        // Loading functions
    bool load(const char *prefix);

        // Printing functions
    bool print(const char* prefix);

        // clear function
    void clear(void);

        // model index related functions
    std::string getModelPrefixFileName(int idx);
    std::string getModelStatus(int idx);

        // destructor
    ~IncrMuxPhraseModel();
	
 protected:

    typedef std::map<std::string,SimpleDynClassLoader<BasePhraseModel> > SimpleDynClassLoaderMap;
    Vector<std::string> tmTypeVec;
    Vector<std::string> modelFileNameVec;
    Vector<std::string> modelStatusVec;
    SimpleDynClassLoaderMap simpleDynClassLoaderMap;
      
    bool loadTmEntries(const char *fileName);
    bool loadTmEntry(std::string tmType,
                     std::string modelFileName,
                     std::string statusStr);
    bool printTmEntries(const char *fileName);
    bool printTm(const char* fileDescName,
                 unsigned int entry_index);
    bool printIntraModelWeights(const char *fileName);
    std::string obtainFileNameForTmEntry(const std::string fileDescName,
                                         unsigned int entry_index);
    std::string obtainDirNameForTmEntry(const std::string fileDescName,
                                        unsigned int entry_index);
    BasePhraseModel* createTmPtr(std::string tmType);
    void deleteModelPointers(void);
    void closeDynamicModules(void);
};

#endif
