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
 * @file MathFuncs.h
 * @brief Defines mathematical functions
 */

#ifndef _MathFuncs_h
#define _MathFuncs_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <math.h>
#ifdef THOT_HAVE_GMP
#include <gmp.h>
#endif
#include "Prob.h"
#include "Bitset.h"
#include "MathDefs.h"

//---------------

namespace MathFuncs
{  
  double logFactorial(unsigned int n);
      // Obtains the logarithm of n!
  double logCombmn(unsigned int m,unsigned int n);
      // Obtains the log of combinatorial numbers
  double logNumbPermsOfTwoElems(unsigned int m,
                                unsigned int n);
      // Obtains the number of permutations of two elements

#ifdef THOT_HAVE_GMP
      // Multiple precision versions of the previous functions. "result"
      // must be a previously initialized integer
  void mp_factorial(unsigned int n,
                    mpz_t & result);
  void mp_logCombmn(unsigned int m,
                    unsigned int n,
                    mpz_t & result);
  void mp_numbPermsOfTwoElems(unsigned int m,
                              unsigned int n,
                              mpz_t & result);
  void mp_randPermOfTwoElems(unsigned int m,
                             unsigned int n,
                             mpz_t & result);
#endif

  template<size_t N>
#ifdef THOT_HAVE_GMP
  Bitset<N> bs_numbPermsOfTwoElems(unsigned int m,
                                   unsigned int n)
        // Obtains the number of permutations of two elements and
        // returns it in a Bitset
  {
    unsigned int  ui_arr[NUM_WORDS(N)];
    size_t countp;
    mpz_t mp_perms;
    
/*       std::cerr<<"******************\n"; */
        // Initialize mp_perms
    mpz_init(mp_perms);
    
        // Obtain number of permutations
    mp_numbPermsOfTwoElems(m,n,mp_perms);

        // Initialize array of unsigned ints
    for(unsigned int i=0;i<NUM_WORDS(N);++i)
      ui_arr[i]=0;
/*       std::cerr<<"countp: "<<countp<<std::endl; */
/*       for(unsigned int i=0;i<NUM_WORDS(N);++i) */
/*         std::cerr<<ui_arr[i]<<" "; */

        // Export mp_perms to array
    mpz_export (ui_arr, &countp, -1, sizeof(unsigned int), 0, 0, mp_perms);
      
/*       std::cerr<<"countp: "<<countp<<std::endl; */
/*       for(unsigned int i=0;i<NUM_WORDS(N);++i) */
/*         std::cerr<<ui_arr[i]<<" "; */
/*       std::cerr<<std::endl; */
/*       gmp_printf ("num perms: %Zd\n", mp_perms); */
    mpz_clear(mp_perms);

          // Initialize bitset with array of ints
    Bitset<N> result(ui_arr);
/*       std::cerr<<m<<" "<<n<<" "<<result<<std::endl; */
/*       Bitset<N> resultAlt((unsigned long) round(exp(logNumbPermsOfTwoElems(m,n)))); */
/*       std::cerr<<"*********\n"; */
/*       std::cerr<<result<<std::endl; */
/*       std::cerr<<resultAlt<<std::endl; */

          // Return result
    return result;
  }
#else
  Bitset<N> bs_numbPermsOfTwoElems(unsigned int m,
                                   unsigned int n)
        // Obtains the number of permutations of two elements and
        // returns it in a Bitset
  {
    Bitset<N> result((unsigned long) round(exp(logNumbPermsOfTwoElems(m,n))));
    return result;
  }
#endif
  
#ifdef THOT_HAVE_GMP
  template<size_t N>
  Bitset<N> bs_randPermOfTwoElems(unsigned int m,
                                  unsigned int n)
  {
    unsigned int  ui_arr[NUM_WORDS(N)];
    size_t countp;
    mpz_t mp_randperm;
    
        // Initialize mp_perms
    mpz_init(mp_randperm);
    
        // Obtain number of permutations
    mp_randPermOfTwoElems(m,n,mp_randperm);

        // Initialize array of unsigned ints
    for(unsigned int i=0;i<NUM_WORDS(N);++i)
      ui_arr[i]=0;

        // Export mp_perms to array
    mpz_export(ui_arr, &countp, -1, sizeof(unsigned int), 0, 0, mp_randperm);
      
    mpz_clear(mp_randperm);

          // Initialize bitset with array of ints
    Bitset<N> result(ui_arr);

          // Return result
    return result;
  }
#else
  template<size_t N>
  Bitset<N> bs_randPermOfTwoElems(unsigned int /*m*/,
                                  unsigned int /*n*/)
  {
    Bitset<N> result;
    return result;
  }
#endif    
  
  Prob poisson(double x,double lambda);
      // Generates probabilities for a poisson distribution
  LgProb log_poisson(double x,double lambda);

  Prob poisson_cdf(double x,double lambda);
      // Generates probabilities for a poisson cumulative distribution
      // function
  LgProb log_poisson_cdf(double x,double lambda);

  Prob norm_std_cdf(double x);
      // Obtain the image of x under the standard normal cumulative
      // distribution function
  LgProb log_norm_std_cdf(double x);

  Prob norm_cdf(double mean,double stddev,double x);
      // Obtain the image of x under the normal cumulative
      // distribution function with parameters mean and stddev
  LgProb log_norm_cdf(double mean,double stddev,double x);

  Prob logarithmic_cdf(double a,double b,double x);
      // Obtain the image of x\in[a,b] under the logarithmic cumulative
      // distribution function.
  LgProb log_logarithmic_cdf(double a,double b,double x);

  Prob triang_cdf(double a,double b,double c,double x);
      // Obtain the image of x\in[a,b] under the triangular cumulative
      // distribution function with mode c.
  LgProb log_triang_cdf(double a,double b,double c,double x);

  Prob geom(double p,unsigned int n);
      // Obtain the image of x under the geometric mass
      // probability function.
  LgProb log_geom(double p,unsigned int n);

  Prob geom_cdf(double p,unsigned int n);
      // Obtain the image of x under the geometric cumulative
      // distribution function.
  LgProb log_geom_cdf(double p,unsigned int n);

  double lns_sum(double x,double y);
      // calculates log(x+y) in the LNS system
  double lns_sumlog(double logx,double logy);
      // calculates log(x+y) in the LNS system, logarithms of x and y
      // are given

  double lns_sub(double x,double y);
      // calculates log(x - y) in the LNS system
  double lns_sublog(double logx,double logy);
      // calculates log(x - y) in the LNS system, logarithms of x and y
      // are given

  float lns_sum_float(float x,float y);
      // calculates log(x+y) in the LNS system (float version)
  float lns_sumlog_float(float logx,float logy);
      // calculates log(x+y) in the LNS system, logarithms of x and y
      // are given (float version)

  float lns_sub_float(float x,float y);
      // calculates log(x - y) in the LNS system (float version)
  float lns_sublog_float(float logx,float logy);
      // calculates log(x - y) in the LNS system, logarithms of x and y
      // are given (float version)

  void initRandNumbers(void);
      // Initialises random number generation
}

#endif
