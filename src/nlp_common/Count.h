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
 * @file Count.h
 * 
 * @brief Defines the Count class to work with counts.
 */

#ifndef _Count_h
#define _Count_h

//--------------- Include files --------------------------------------

#include <iomanip>
#include <iostream>
#include <math.h>

//--------------- Classes --------------------------------------------

class Count
{
 private:
  float x;
  
 public:
  Count(){x=0;}
  Count(double y):x(y){}
  Count(float y):x(y){}
  Count(int y):x(y){}
  Count(unsigned int y):x(y){}
  operator double() const {return (float)x;}
  operator float() const {return x;}
  Count operator *= (double y) { x *= y; return *this;}
  Count operator *= (Count y) { x *= y.x; return *this;}
  Count operator /= (double y) { x /= y; return *this;}
  Count operator /= (Count y) { x /= y.x; return *this;}
  Count operator += (double y) { x += y; return *this;}
  Count operator += (Count y) { x += y.x; return *this;}
  Count operator + (double y) { return x + y;}
  Count operator + (Count y) { return x + y.x;}
  Count operator -= (double y) { x -= y; return *this;}
  Count operator -= (Count y) { x -= y.x; return *this;}
  Count operator - (double y) { return x - y;}
  Count operator - (Count y) { return x - y.x;}
  Count operator * (double y)const { return x * y;}
  Count operator * (Count y)const { return x * y.x;}
  bool operator < (Count y)const { if(this->x < y.x) return true; else return false;}
  bool operator > (Count y)const { if(this->x > y.x) return true; else return false;}
  bool operator <= (Count y)const { if(this->x <= y.x) return true; else return false;}
  bool operator >= (Count y)const { if(this->x >= y.x) return true; else return false;}

  void set_count(float y){x=y;}
  void set_count(double y){x=y;}
  void set_count(int y){x=y;}
  void set_count(Count y){x=y.x;}
  void set_logcount(float y){x=exp(y);}
  void set_logcount(double y){x=exp(y);}

  void incr_count(float y){x+=y;}
  void incr_count(double y){x+=y;}
  void incr_count(int y){x+=y;}
  void incr_count(Count y){x+=y.x;}
  void incr_logcount(float y){x=x+exp(y);}
  void incr_logcount(double y){x=x+exp(y);}

  float get_c_s(void)const {return x;}
  float get_c_st(void)const {return x;}
  float get_lc_s(void)const {return log(x);}
  float get_lc_st(void)const {return log(x);}

  friend std::ostream& operator <<(std::ostream &outS,const Count& p)
  {
    outS<<(double) p.x;
    return outS;
  }
  friend std::istream& operator>>(std::istream& is, Count& p)
  {
    is >> p.x;
    return is;
  }
};

class greaterCount
{
 public:
  bool operator() (const Count &a,const Count &b)const
    {
      if((double) a>(double) b) return true;
      else return false;
    }
};

#endif
