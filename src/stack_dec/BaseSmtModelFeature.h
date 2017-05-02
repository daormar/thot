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
 
/********************************************************************/
/*                                                                  */
/* Module: BaseSmtModelFeature                                      */
/*                                                                  */
/* Prototypes file: BaseSmtModelFeature.h                           */
/*                                                                  */
/* Description: Declares the BaseSmtModelFeature abstract template  */
/*              class this class is a base class for implementing   */
/*              different kinds of features to be used              */
/*              in SMT models.                                      */
/*                                                                  */
/********************************************************************/

/**
 * @file BaseSmtModelFeature.h
 * 
 * @brief Declares the BaseSmtModelFeature abstract template class, this
 * class is a base class for implementing different kinds of features
 * to be used in SMT models.
 */

#ifndef _BaseSmtModelFeature_h
#define _BaseSmtModelFeature_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "Score.h"
#include <string>

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- BaseSmtModelFeature class

/**
 * @brief The BaseSmtModelFeature abstract template class is a base
 * class for implementing different kinds of features to be used in SMT
 * models.
 */

template<class HYPOTHESIS>
class BaseSmtModelFeature
{
 public:

      // TO-BE-DONE

      // Weight related functions
  void setWeight(float w);
  float getWeight(float w);

      // Feature information
  void setFeatName(std::string fname);
  std::string getFeatName(void);
  virtual std::string getFeatType(void)=0;

      // Scoring functions
  Score extensionScore(const Hypothesis& pred_hyp,
                       const HypDataType& new_hypd)=0;

      // Destructor
  virtual ~BaseSmtModel(){};

 protected:

  float weight;
  std::string name;
};

//--------------- BaseSmtModelFeature class functions
//

template<class HYPOTHESIS>
void BaseSmtModelFeature<HYPOTHESIS>::setWeight(float w)
{
  weight=w;
}

//---------------------------------
template<class HYPOTHESIS>
float BaseSmtModelFeature<HYPOTHESIS>::getWeight(float w)
{
  return weight;
}

//---------------------------------
template<class HYPOTHESIS>
void BaseSmtModelFeature<HYPOTHESIS>::setFeatName(std::string fname)
{
  name=fname;
}

//---------------------------------
template<class HYPOTHESIS>
std::string BaseSmtModelFeature<HYPOTHESIS>::getFeatName(void)
{
  return name;
}

#endif
