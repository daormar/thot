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
/* Module: BaseStepwiseAligModel                                    */
/*                                                                  */
/* Prototype file: BaseStepwiseAligModel.h                          */
/*                                                                  */
/* Description: Defines the BaseStepwiseAligModel class.            */
/*              BaseStepwiseAligModel is a base class for           */
/*              derivating single-word statistical alignment models */
/*              using stepwise EM.                                  */
/*                                                                  */
/********************************************************************/

#ifndef _BaseStepwiseAligModel_h
#define _BaseStepwiseAligModel_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <string>
#include "SwDefs.h"
#include <ErrorDefs.h>

using namespace std;

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Function declarations ------------------------------


//--------------- Classes --------------------------------------------


//--------------- BaseStepwiseAligModel class

class BaseStepwiseAligModel
{
 public:

    // Constructor
    BaseStepwiseAligModel(void){};

    virtual void set_nu_val(float _nu)=0;
        // Function to set the value of alpha

    // Destructor
	virtual ~BaseStepwiseAligModel(){};
};

#endif
