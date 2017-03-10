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
 
#ifndef _thot_server_pars
#define _thot_server_pars

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "client_server_defs.h"

using namespace std;

//--------------- Structs --------------------------------------------

struct thot_server_pars
{
  bool i_given;
  bool c_given;
  std::string c_str;
  bool p_given;
  unsigned int server_port;
  bool w_given;
  bool v_given;

  thot_server_pars()
    {
      default_values();
    }

  void default_values(void)
    {
      i_given=false;
      c_given=false;
      p_given=false;
      server_port=DEFAULT_SERVER_PORT;
      w_given=false;
      v_given=false;
    }
};

#endif
