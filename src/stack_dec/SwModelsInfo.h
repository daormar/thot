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
 
#ifndef _SwModelsInfo_h
#define _SwModelsInfo_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include THOT_PPINFO_H // Define PpInfo type. It is set in
                       // configure by checking PPINFO_H variable
                       // (default value: PpInfo.h)
#include "BaseSwAligModel.h"
#include "myVector.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- SwModelsInfo struct

struct SwModelsInfo
{
      // sw model members
  Vector<BaseSwAligModel<PpInfo>*> swAligModelPtrVec;
  Vector<std::string> featNameVec;

      // Inverse sw model members
  Vector<BaseSwAligModel<PpInfo>*> invSwAligModelPtrVec;
  Vector<std::string> invFeatNameVec;
};

#endif
