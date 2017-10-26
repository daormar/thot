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
 

#include "MathFuncs.h"

namespace MathFuncs
{
  //-------------------------
  double logFactorial(unsigned int n)
  {
    double logf;
    unsigned int i;
    
    logf=0;    
    for(i=1;i<=n;++i)
      logf=logf+log((double)i);
    return logf;     
  }

  //-------------------------
  double logCombmn(unsigned int m,unsigned int n)
  {
    return logFactorial(m)-(logFactorial(n)+logFactorial(m-n));  
  }

  //---------------------------------------
  double logNumbPermsOfTwoElems(unsigned int J,
                                unsigned int l)
  {
    unsigned int i;
    double result;	
 
    if(l<=J)
    {
      if(l==0) return 1;
      else
      {
        if(J-l>l)
        {
          result=0;
          for(i=J;i>J-l;--i)
            result=result+(double)log((double)i);
          result=result-(logFactorial(l));
          return result;
        }
        else
        { 
          result=0;
          for(i=J;i>l;--i)
            result=result+(double)log((double)i);
          result=result-(logFactorial(J-l));
          return result;
        }
      }
    }
    else return 0;  
  }
  
#ifdef THOT_HAVE_GMP
  gmp_randstate_t gmprstate;
  
  //-------------------------
  void mp_factorial(unsigned int n,
                    mpz_t & result)
  {
    mpz_set_ui(result,1);
    for(unsigned int i=1;i<=n;++i)
    {
      mpz_mul_ui(result,result,i);
    }
  }
  
  //-------------------------
  void mp_logCombmn(unsigned int m,
                    unsigned int n,
                    mpz_t & result)
  {
    mpz_t mp_f1;
    mpz_t mp_f2;
    mpz_t mp_num;
    mpz_t mp_den;

    mpz_init(mp_f1);
    mpz_init(mp_f2);
    mpz_init(mp_num);
    mpz_init(mp_den);

    mp_factorial(m,mp_num);
    mp_factorial(n,mp_f1);
    mp_factorial(m-n,mp_f2);

    mpz_mul(mp_den,mp_f1,mp_f2);
    mpz_cdiv_q(result,mp_num,mp_den);
    
    mpz_clear(mp_f1);
    mpz_clear(mp_f2);
    mpz_clear(mp_num);
    mpz_clear(mp_den);
  }
  
  //-------------------------
  void mp_numbPermsOfTwoElems(unsigned int m,
                              unsigned int n,
                              mpz_t & result)
  {
    if(n<=m)
    {
      if(n==0) mpz_set_ui(result,1);
      else
      {
        if(m-n>n)
        {
          mpz_t mp_fact;
          mpz_t mp_num;
          mpz_init(mp_fact);
          mpz_init(mp_num);

          mpz_set_ui(mp_num,1);
          for(unsigned int i=m;i>m-n;--i)
          {
            mpz_mul_ui(mp_num,mp_num,i);
          }
          mp_factorial(n,mp_fact);
          mpz_cdiv_q(result,mp_num,mp_fact);
          mpz_clear(mp_fact);
          mpz_clear(mp_num);
        }
        else
        {
          mpz_t mp_fact;
          mpz_t mp_num;
          mpz_init(mp_fact);
          mpz_init(mp_num);
          
          mpz_set_ui(mp_num,1);
          for(unsigned int i=m;i>n;--i)
          {
            mpz_mul_ui(mp_num,mp_num,i);
          }
          mp_factorial(m-n,mp_fact);
          mpz_cdiv_q(result,mp_num,mp_fact);
          mpz_clear(mp_fact);
          mpz_clear(mp_num);
        }
      }
    }
    else mpz_set_ui(result,0);
  }

  //-------------------------
  void mp_randPermOfTwoElems(unsigned int m,
                             unsigned int n,
                             mpz_t & result)
  {
    mpz_t mp_nump;
    mpz_init(mp_nump);
    mp_numbPermsOfTwoElems(m,n,mp_nump);
    mpz_urandomm (result,gmprstate,mp_nump);
    mpz_clear(mp_nump);
  }

#endif
  
  //-------------------------
  Prob poisson(double x,double lambda)
  {
    return (double)exp((double)log_poisson(x,lambda));
  }

  //-------------------------
  LgProb log_poisson(double x,double lambda)
  {
    LgProb logp;
    
    logp=-lambda+x*log(lambda);
    for(unsigned int i=1;i<=x;++i) logp=(double)logp-log((double)i);
    
    return logp;
  }

  //-------------------------
  Prob poisson_cdf(double x,double lambda)
  {
    Prob result=0;
    for(unsigned int k=0;k<=(unsigned int)trunc(x);++k)
    {
      result+=poisson(k,lambda);
    }
    return result;
  }

  //-------------------------
  LgProb log_poisson_cdf(double x,double lambda)
  {
    return (double) log((double)poisson_cdf(x,lambda));
  }

  //-------------------------
  Prob norm_std_cdf(double x)
  {
    return 0.5*(1.0+erf(x/sqrt(2.0)));
  }
  
  //-------------------------
  LgProb log_norm_std_cdf(double x)
  {
    return (double)log((double)norm_std_cdf(x));
  }

  //-------------------------
  Prob norm_cdf(double mean,double stddev,double x)
  {
    return norm_std_cdf((x-mean)/stddev);
  }
  
  //-------------------------
  LgProb log_norm_cdf(double mean,double stddev,double x)
  {
    return (double)log((double)norm_cdf(mean,stddev,x));
  }

  //-------------------------
  Prob logarithmic_cdf(double a,double b,double x)
  {
    return ((a*(1.0-log(a)))-(x*(1.0-log(x))))/((a*(1.0-log(a)))-(b*(1.0-log(b))));
  }
  
  //-------------------------
  LgProb log_logarithmic_cdf(double a,double b,double x)
  {
    return (double)log((double)logarithmic_cdf(a,b,x));    
  }
  
  //-------------------------
  Prob triang_cdf(double a,double b,double c,double x)
  {
    if(a<=x && x<=c)
    {
      return ((x-a)*(x-a))/((b-a)*(c-a));
    }
    else
    {
      if(c<x && x<=b)
      {
        return 1.0-(((b-x)*(b-x))/((b-a)*(b-c)));
      }
      else
        return 0;
    }
  }

  //-------------------------
  LgProb log_triang_cdf(double a,double b,double c,double x)
  {
    return (double)log((double)triang_cdf(a,b,c,x)); 
  }

  //-------------------------
  Prob geom(double p,unsigned int n)
  {
    return (double)exp((double)log_geom(p,n));
  }
  
  //-------------------------
  LgProb log_geom(double p,unsigned int n)
  {
    LgProb result=log(p);
    result+=n*log(1-p);
    return result;
  }

  //-------------------------
  Prob geom_cdf(double p,unsigned int n)
  {
    Prob result=0;
    for(unsigned int i=0;i<=n;++i)
    {
      result+=geom(p,i);
    }
    return result;
  }
  
  //-------------------------
  LgProb log_geom_cdf(double p,unsigned int n)
  {
    return (double)log((double)geom_cdf(p,n));
  }

  //-------------------------
  double lns_sum(double x,double y)
  {
    return lns_sumlog(log(x),log(y));
  }

  //-------------------------
  double lns_sumlog(double logx,double logy)
  {
    if(logx>logy)
    {
      return logx+log(1+exp(logy-logx));
    }
    else
    {
      return logy+log(1+exp(logx-logy));
    }
  }

  //-------------------------
  double lns_sub(double x,double y)
  {
    return lns_sublog(log(x),log(y));
  }

  //-------------------------
  double lns_sublog(double logx,double logy)
  { 
    return logx+log(1-exp(logy-logx));
  }
  
  //-------------------------
  float lns_sum_float(float x,float y)
  {
    return lns_sumlog(log(x),log(y));
  }

  //-------------------------
  float lns_sumlog_float(float logx,float logy)
  {
    if(logx>logy)
    {
      return logx+log(1+exp(logy-logx));
    }
    else
    {
      return logy+log(1+exp(logx-logy));
    }
  }

  //-------------------------
  float lns_sub_float(float x,float y)
  {
    return lns_sublog(log(x),log(y));
  }

  //-------------------------
  float lns_sublog_float(float logx,float logy)
  { 
    return logx+log(1-exp(logy-logx));
  }

  //-------------------------
  void initRandNumbers(void)
  {
#ifdef THOT_HAVE_GMP
    gmp_randinit_default(gmprstate);
#endif    
  }

}
