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

/**
 * @file EditDistForStr.h
 * 
 * @brief Defines the EditDistForStr class that calculates the edit
 * distance between strings.
 */

#ifndef _EditDistForStr_h
#define _EditDistForStr_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_editDist.h"
#include <string.h>
#include <string>

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------


//--------------- Classes --------------------------------------------


//--------------- EditDistForStr class declaration

class EditDistForStr: public _editDist<std::string>
{
 public:
	
	EditDistForStr(void);

    Score calculateEditDistPrefix(const std::string& x,
                                  const std::string& y,
                                  int verbose=0);
        // Calculates edit distance between x and y, given that y is an
        // incomplete prefix
    Score calculateEditDistPrefixOps(const std::string& x,
                                     const std::string& y,
                                     std::vector<unsigned int>& ops,
                                     int verbose=0);
        // The same as the previous function, but it also returns the
        // sequence of string operations
    Score calculateEditDistPrefixOpsNoPrefDel(const std::string& x,
                                              const std::string& y,
                                              std::vector<unsigned int>& ops,
                                              int verbose=0);
        // The same as the previous function, but the special PREF_DEL_OP
        // operation is not allowed    
	  
	~EditDistForStr(void);
    
  protected:

    Score processMatrixCell(const std::string& x,
                            const std::string& y,
                            const DistMatrix& dm,
                            int i,
                            int j,
                            int& pred_i,
                            int& pred_j,
                            int& op_id);
        // Basic function to calculate edit distance

    Score processMatrixCellPref(const std::string& x,
                                const std::string& y,
                                const DistMatrix& dm,
                                bool usePrefDelOp,
                                int i,
                                int j,
                                int& pred_i,
                                int& pred_j,
                                int& op_id);
        // Basic function to calculate edit distance given a prefix

    void obtainOperationsPref(const std::string& x,
                              const std::string& y,
                              const DistMatrix& dm,
                              bool usePrefDelOp,
                              int i,
                              int j,
                              std::vector<unsigned int> &opsCharLevel);
        // After an edit distance calculation given a prefix, this
        // function obtains the optimal sequence of operations.

    Score calculateEditDistPrefixOpsAux(const std::string& x,
                                        const std::string& y,
                                        bool usePrefDelOp,
                                        std::vector<unsigned int>& ops,
                                        int verbose);
        // Auxiliary function for calculateEditDistPrefixOps()

    inline Score insertionCost(char /*c*/)
      {
        return insCost;
      }
    inline Score deletionCost(char /*c*/)
      {
        return delCost;
      }
    inline Score substitutionCost(char /*x*/,char /*y*/)
      {
        return substCost;
      }
};

#endif
