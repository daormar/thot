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
 
#ifndef _im_pair_h
#define _im_pair_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <math.h>
#include <Prob.h>
#include <iomanip>
#include <iostream>

using namespace std;

//--------------- Classes --------------------------------------------

template<class T1,class T2>
class im_pair /* incremental model pair */
{
 private:
  
 public:
  T1 first;
  T2 second;


  Prob get_p(void)
    {
      return (float)this->second.get_c_st()/(float)this->first.get_c_s();
    }

  LgProb get_lp(void)
    {
      return (float)log((float)get_p());
    }

  bool operator < (im_pair y)const
    {
      if(this->first < y.first) return true;
      if(y.first < this->first) return false;
      if(this->second < y.second) return true;
      if(y.second < this->second) return false;
      return false;
    }
  
  bool operator > (im_pair y)const
    {
      if(this->first > y.first) return true;
      if(y.first > this->first) return false;
      if(this->second > y.second) return true;
      if(y.second > this->second) return false;
      return false;
    }
  
  friend ostream& operator <<(ostream &outS,const im_pair& imp)
    {
      outS<<imp.first<<" "<<imp.second;
      return outS;
    }
  
  friend istream& operator>>(istream& is, const im_pair& imp)
    {
      is >> imp.first>>" ">>imp.second;
      return is;
    }
};

#endif
