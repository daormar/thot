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
 
#ifndef _thot_gen_sw_model_pars
#define _thot_gen_sw_model_pars

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <string>

//--------------- Structs --------------------------------------------

struct thot_gen_sw_model_pars
{
  bool s_given;
  std::string s_str;
  bool t_given;
  std::string t_str;
  bool l_given;
  std::string l_str;
  bool n_given;
  bool nl_given;
  unsigned int numIter;
  bool eb_given;
  bool i_given;
  bool c_given;
  bool r_given;
  unsigned int r;
  bool mb_given;
  unsigned int mb;
  bool lr_given;
  std::vector<float> lrPars;
  bool in_given;
  bool p_given;
  bool lf_given;
  bool af_given;
  float lf_val;
  float af_val;
  bool np_given;
  float np_val;
  bool o_given;
  std::string o_str;
  bool v_given;
  bool v1_given;

  thot_gen_sw_model_pars()
    {
      default_values();
    }

  void default_values(void)
    {
      s_given=false;
      t_given=false;
      l_given=false;
      n_given=false;
      nl_given=false;
      eb_given=false;
      i_given=false;
      c_given=false;
      r_given=false;
      mb_given=false;
      lr_given=false;
      in_given=false;
      p_given=false;
      lf_given=false;
      af_given=false;
      np_given=false;
      o_given=false;
      v_given=false;
      v1_given=false;      
    }
};

#endif
