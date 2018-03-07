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
 * @file StrictCategPhrasePairFilter.h
 * 
 * @brief Defines the StrictCategPhrasePairFilter class.  It is intended
 * to filter phrase pairs containing unpaired category tags. In
 * particular, phrase pairs containing category tags should have exactly
 * the same elements in both the source and target sides.
 */

#ifndef _StrictCategPhrasePairFilter_h
#define _StrictCategPhrasePairFilter_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <map>
#include <set>
#include "BasePhrasePairFilter.h"
#include "SmtPreprocDefs.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------


//--------------- StrictCategPhrasePairFilter class

class StrictCategPhrasePairFilter: public BasePhrasePairFilter
{
 public:

    StrictCategPhrasePairFilter(void);

    bool phrasePairIsOk(std::vector<std::string> s_,
                        std::vector<std::string> t_);
	
    ~StrictCategPhrasePairFilter(){};

 private:

    std::set<std::string> categorySet;
};

#endif
