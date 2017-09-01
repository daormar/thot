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
 
#ifndef _ThotDecoderUserPars_h
#define _ThotDecoderUserPars_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <WordGraph.h>
#include <string>

//--------------- Constants ------------------------------------------

#define TD_USER_S_DEFAULT         10
#define TD_USER_BE_DEFAULT     false
#define TD_USER_G_DEFAULT          0
#define TD_USER_NP_DEFAULT        10
#define TD_USER_WGP_DEFAULT        UNLIMITED_DENSITY
#define TD_USER_SP_DEFAULT         0

//--------------- Classes --------------------------------------------

class ThotDecoderUserPars
{
 public:

  unsigned int S;
  bool be;
  unsigned int G;
  unsigned int np;
  float wgp;
  std::string wgh_str;
  unsigned int sp;
  std::string uc_str;
  std::vector<float> catWeightsVec;
  
  ThotDecoderUserPars()
  {
    default_values();
  }

  void default_values(void)
  {
    S=TD_USER_S_DEFAULT;
    be=TD_USER_BE_DEFAULT;
    G=TD_USER_G_DEFAULT;
    np=TD_USER_NP_DEFAULT;
    wgp=TD_USER_WGP_DEFAULT;
    sp=TD_USER_SP_DEFAULT;
    uc_str="";//"/home/dortiz/traduccion/software/smt_preproc/data/training.enes.en.sim.tabla";
    wgh_str="";
    catWeightsVec.clear();
  }
};

#endif
