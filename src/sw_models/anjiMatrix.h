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
 * @file anjiMatrix.h
 * 
 * @brief Defines the anjiMatrix class.  anjiMatrix class stores
 * expected values used in the estimation of IBM 1 and IBM 2 statistical
 * alignment model.
 * 
 */

#ifndef _anjiMatrix_h
#define _anjiMatrix_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <AwkInputStream.h>
#include <vector>
#include <utility>
#include <limits.h>
#include <StatModelDefs.h>
#include <MathDefs.h>

//--------------- Constants ------------------------------------------

#define INVALID_ANJI_VAL             99
#define UNRESTRICTED_ANJI_SIZE UINT_MAX

//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------

//--------------- Classes --------------------------------------------

//--------------- anjiMatrix class

class anjiMatrix
{
  public:

      // Constructor
   anjiMatrix(void);

       // Functions to initialize entries
   bool init_nth_entry(unsigned int n,
                       PositionIndex nslen,
                       PositionIndex tlen,
                       unsigned int& mapped_n);
   bool reset_entries(void);
   
      // Functions to handle anji
   void set_maxnsize(unsigned int _anji_maxnsize);
   unsigned int get_maxnsize(void);
   unsigned int n_size(void);
   unsigned int nj_size(unsigned int n);
   unsigned int nji_size(unsigned int n,
                         unsigned int j);
   void set(unsigned int n,
            unsigned int j,
            unsigned int i,
            float f);
   void set_fast(unsigned int mapped_n,
                 unsigned int j,
                 unsigned int i,
                 float f);
   float get(unsigned int n,
             unsigned int j,
             unsigned int i);
   float get_fast(unsigned int mapped_n,
                  unsigned int j,
                  unsigned int i);
   float get_invp(unsigned int n,
                  unsigned int j,
                  unsigned int i);
   float get_invp_fast(unsigned int mapped_n,
                       unsigned int j,
                       unsigned int i);
   float get_invlogp(unsigned int n,
                     unsigned int j,
                     unsigned int i);
   float get_invlogp_fast(unsigned int mapped_n,
                          unsigned int j,
                          unsigned int i);

       // load function
   bool load(const char* prefFileName);
   
       // print function
   bool print(const char* prefFileName);

       // clear() function
   void clear(void);
   
  protected:
   
   unsigned int anji_maxnsize;
   unsigned int anji_pointer;
   std::vector<std::vector<std::vector<float> > > anji;
       // Use simple precission floating-point numbers for expected
       // values
   std::vector<std::pair<bool,unsigned int> > np_to_n_vector;
       // For each index of anji stores if it is already used and the
       // real index of the sample
   std::vector<std::pair<bool,unsigned int> > n_to_np_vector;
       // For each sample n stores if it is mapped in anji, and its
       // corresponding index
       
       // Auxiliary functions
   bool resizeIsRequired(unsigned int mapped_n,
                         PositionIndex nslen,
                         PositionIndex tlen);
   bool n_is_mapped_in_matrix(unsigned int n,
                              unsigned int &np);
   void map_n_in_matrix(unsigned int n,
                        unsigned int &np);
       // Return index for n in anji, the index is created if it does
       // not exist
   std::pair<bool,unsigned int> read_np_to_n_vector(unsigned int np);
   std::pair<bool,unsigned int> read_n_to_np_vector(unsigned int n);
   void update_np_to_n_vector(unsigned int np,
                              std::pair<bool,unsigned int> pbui);
   void update_n_to_np_vector(unsigned int n,
                              std::pair<bool,unsigned int> pbui);

       // Functions to load and print anji matrices
   bool load_anji_values(const char* anjiFile);   
   bool print_anji_values(const char* anjiFile);

       // Functions to load and print maximum size data
   bool load_maxnsize_data(const char* maxnsizeDataFile);
   bool print_maxnsize_data(const char* maxnsizeDataFile);
};

#endif
