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

/**
 * @file ThotHmmAlignerFactory.cc
 * 
 * @brief Definitions file for ThotHmmAlignerFactory.h
 */

//--------------- Include files --------------------------------------

#include "ThotHmmAlignerFactory.h"

//--------------- ThotHmmAlignerFactory class function definitions

int ThotHmmAlignerFactory::init(int argc, char *argv[], Context *context /*= 0*/)
{
  if (argc!=2) {
    std::cerr << "Invalid number of arguments for initialization of the aligner!\nReceived: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }
  ha = new ThotHmmAligner();
  ha->init(argv[1]);
  ha->setLogger(_logger);
  
  return EXIT_SUCCESS;
}

//---------------- plugins
EXPORT_CASMACAT_PLUGIN_NAME(IAlignmentEngine,ThotHmmAlignerFactory,thot_align_plugin);
