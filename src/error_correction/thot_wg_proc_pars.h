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
 
#ifndef _thot_wg_proc_pars
#define _thot_wg_proc_pars

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <string>
#include <vector>
#include <options.h>

//--------------- Structs --------------------------------------------

struct thot_wg_proc_pars
{
  bool w_given;
  std::string w_str;
  bool wgp_given;
  float pruningThreshold;
  bool bp_given;
  unsigned int hypStateIndex;
  std::vector<float> compWeights;
  bool n_given;
  unsigned int nbListLen;
  bool u_given;
  bool t_given;
  bool o_given;
  std::string o_str;
  bool v_given;
  bool v1_given;

  thot_wg_proc_pars()
    {
      default_values();
    }

  void default_values(void)
    {
      w_given=false;
      wgp_given=false;
      bp_given=false;
      n_given=false;
      u_given=false;
      t_given=false;
      o_given=false;
      v_given=false;
      v1_given=false;      
    }
};

#endif
