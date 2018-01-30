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

/**
 * @file ThotIbmMaxConfidFactory.cc
 * 
 * @brief Definitions file for ThotIbmMaxConfidFactory.h
 */

//--------------- Include files --------------------------------------

#include "ThotIbmMaxConfidFactory.h"

//--------------- ThotIbmMaxConfidFactory class function definitions

//-------------------------
int ThotIbmMaxConfidFactory::init(int argc, char *argv[], Context *context /*= 0*/)
{
  if (argc != 2)
    return EXIT_FAILURE;
    
  rc = new ThotIbmMaxConfid();
  rc->init(argv[1]);
  rc->setLogger(_logger);
    
  return EXIT_SUCCESS;
}

//---------------- plugins
EXPORT_CASMACAT_PLUGIN_NAME(IConfidenceEngine, ThotIbmMaxConfidFactory, thot_cm_plugin);
