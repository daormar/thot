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
 * @file Prob.h
 * 
 * @brief Defines the Prob and LgProb classes to store probabilities and
 * log-probabilities respectively.
 */

#ifndef _Prob_h
#define _Prob_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <math.h>
#include <iomanip>
#include <iostream>

//--------------- Constants ------------------------------------------

#define UNINIT_PROB   99
#define UNINIT_LGPROB 99

//--------------- Classes --------------------------------------------

class LgProb;

class Prob
{
 private:
  double x;
  
 public:
  Prob(){x=UNINIT_PROB;}
  Prob(double y):x(y){}
  Prob(float y):x(y){}
  Prob(int y):x(y){}
  operator double() const {return x;}
  operator float() const {return (float)x;}
  Prob operator *= (double y) { x *= y; return *this;}
  Prob operator *= (Prob y) { x *= y.x; return *this;}
  Prob operator /= (double y) { x /= y; return *this;}
  Prob operator /= (Prob y) { x /= y.x; return *this;}
  Prob operator += (double y) { x += y; return *this;}
  Prob operator += (Prob y) { x += y.x; return *this;}
  Prob operator + (double y) { return x + y;}
  Prob operator + (Prob y) { return x + y.x;}
  Prob operator -= (double y) { x -= y; return *this;}
  Prob operator -= (Prob y) { x -= y.x; return *this;}
  Prob operator - (double y)const { return x - y;}
  Prob operator - (Prob y)const { return x - y.x;}
  Prob operator * (double y)const { return x * y;}
  Prob operator * (Prob y)const { return x * y.x;}
  bool operator == (Prob y)const { if(this->x == y.x) return true; else return false;}
  bool operator != (Prob y)const { if(this->x != y.x) return true; else return false;}
  bool operator < (Prob y)const { if(this->x < y.x) return true; else return false;}
  bool operator > (Prob y)const { if(this->x > y.x) return true; else return false;}
  bool operator <= (Prob y)const { if(this->x <= y.x) return true; else return false;}
  bool operator >= (Prob y)const { if(this->x >= y.x) return true; else return false;}

  Prob get_p(void)const 
    {return *this;}
  LgProb get_lp(void)const;
  friend std::ostream& operator <<(std::ostream &outS,const Prob& p)
  {
    outS<<(double) p.x;
    return outS;
  }
  friend std::istream& operator>>(std::istream& is, Prob& p)
  {
    is >> p.x;
    return is;
  }
};

class greaterProb
{
 public:
  bool operator() (const Prob &a,const Prob &b)const
    {
      if((double) a>(double) b) return true;
      else return false;
    }
};

//------------------------------
class LgProb
{
 private:
  double x;
  
 public:
  LgProb(){x=UNINIT_LGPROB;}
  LgProb(double y):x(y){}
  LgProb(float y):x(y){}
  LgProb(int y):x(y){}
  operator double() const {return x;}
  operator float() const {return (float)x;}
  LgProb operator *= (double y) { x *= y; return *this;}
  LgProb operator *= (LgProb y) { x *= y.x; return *this;}
  LgProb operator /= (double y) { x /= y; return *this;}
  LgProb operator /= (LgProb y) { x /= y.x; return *this;}
  LgProb operator += (double y) { x += y; return *this;}
  LgProb operator += (LgProb y) { x += y.x; return *this;}
  LgProb operator + (double y)const { return x+y;}
  LgProb operator + (LgProb y)const { return x+y.x;}
  LgProb operator -= (double y) { x -= y; return *this;}
  LgProb operator -= (LgProb y) { x -= y.x; return *this;}
  LgProb operator - (double y)const { return x-y;}
  LgProb operator - (LgProb y)const { return x-y.x;}
  LgProb operator * (double y)const { return x*y;}
  LgProb operator * (LgProb y)const { return x*y.x;}
  bool operator == (LgProb y)const { if(this->x == y.x) return true; else return false;}
  bool operator != (LgProb y)const { if(this->x != y.x) return true; else return false;}
  bool operator < (LgProb y)const { if(this->x < y.x) return true; else return false;}
  bool operator > (LgProb y)const { if(this->x > y.x) return true; else return false;}
  bool operator <= (LgProb y)const { if(this->x <= y.x) return true; else return false;}
  bool operator >= (LgProb y)const { if(this->x >= y.x) return true; else return false;}

  Prob get_p(void)const
  {
    Prob p(exp(x));
    return p;
  }
  LgProb get_lp(void)const
    {return *this;}
  friend std::ostream& operator << (std::ostream &outS,const LgProb& lp)
  {
    outS<<(double) lp.x;
    return outS;
  }  
};

class greaterLgProb
{
 public:
  bool operator() (const LgProb &a,const LgProb &b)const
    {
      if((double) a>(double) b) return true;
      else return false;
    }
};

#endif
