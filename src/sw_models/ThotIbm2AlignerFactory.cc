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

//--------------- Include files --------------------------------------

#include "ThotIbm2AlignerFactory.h"

//--------------- ThotIbm2AlignerFactory class function definitions

int ThotIbm2AlignerFactory::init(int argc, char *argv[], Context *context /*= 0*/)
{
  if (argc!=2) {
    std::cerr << "Invalid number of arguments for initialization of the aligner!\nReceived: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }
  ibm2a = new ThotIbm2Aligner();
  ibm2a->init(argv[1]);
  ibm2a->setLogger(_logger);
  
  return EXIT_SUCCESS;
}

//---------------- plugins
EXPORT_CASMACAT_PLUGIN_NAME(IAlignmentEngine,ThotIbm2AlignerFactory,thot_ibm2align_plugin);
