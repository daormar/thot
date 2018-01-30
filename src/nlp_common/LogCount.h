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
 * @file LogCount.h
 * 
 * @brief Defines the LogCount class to work with logarithms of counts.
 */

#ifndef _LogCount_h
#define _LogCount_h

//--------------- Include files --------------------------------------

#include "MathFuncs.h"
#include <iomanip>
#include <iostream>

//--------------- Classes --------------------------------------------

class LogCount
{
 private:
  float x;
  
 public:
  LogCount(){x=SMALL_LG_NUM;}
  LogCount(double y):x(y){}
  LogCount(float y):x(y){}
  LogCount(int y):x(y){}
  operator double() const {return (float) x;}
  operator float() const {return x;}
  LogCount operator *= (double y) { x *= y; return *this;}
  LogCount operator *= (LogCount y) { x *= y.x; return *this;}
  LogCount operator /= (double y) { x /= y; return *this;}
  LogCount operator /= (LogCount y) { x /= y.x; return *this;}
  LogCount operator += (double y) { x += y; return *this;}
  LogCount operator += (LogCount y) { x += y.x; return *this;}
  LogCount operator + (double y) { return x + y;}
  LogCount operator + (LogCount y) { return x + y.x;}
  LogCount operator -= (double y) { x -= y; return *this;}
  LogCount operator -= (LogCount y) { x -= y.x; return *this;}
  LogCount operator - (double y) { return x - y;}
  LogCount operator - (LogCount y) { return x - y.x;}
  LogCount operator * (double y)const { return x * y;}
  LogCount operator * (LogCount y)const { return x * y.x;}
  bool operator < (LogCount y)const { if(this->x < y.x) return true; else return false;}
  bool operator > (LogCount y)const { if(this->x > y.x) return true; else return false;}
  bool operator <= (LogCount y)const { if(this->x <= y.x) return true; else return false;}
  bool operator >= (LogCount y)const { if(this->x >= y.x) return true; else return false;}

  void set_count(float y){x=y;}
  void set_count(double y){x=y;}
  void set_count(int y){x=y;}
  void set_count(Count y){x=log((double)y);}  
  void set_logcount(float y){x=y;}
  void set_logcount(double y){x=y;}

  void incr_count(float y){x=MathFuncs::lns_sumlog(x,log((double)y));}
  void incr_count(double y){x=MathFuncs::lns_sumlog(x,log((double) y));}
  void incr_count(int y){x=MathFuncs::lns_sumlog(x,log((double) y));}
  void incr_count(Count y){x=MathFuncs::lns_sumlog(x,log((double) y));}
  void incr_logcount(float y){x=MathFuncs::lns_sumlog(x,(double) y);}
  void incr_logcount(double y){x=MathFuncs::lns_sumlog(x,(double) y);}

  float get_c_s(void)const {return (float)exp(x);}
  float get_c_st(void)const {return (float)exp(x);}
  float get_lc_s(void)const {return x;}
  float get_lc_st(void)const {return x;}

  friend std::ostream& operator <<(std::ostream &outS,const LogCount& p)
  {
    outS<<(double) p.x;
    return outS;
  }
  friend std::istream& operator>>(std::istream& is, LogCount& p)
  {
    is >> p.x;
    return is;
  }
};

class greaterLogCount
{
 public:
  bool operator() (const LogCount &a,const LogCount &b)const
    {
      if((double) a>(double) b) return true;
      else return false;
    }
};

#endif
