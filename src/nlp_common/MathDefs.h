/*
thot package for statistical machine translation
Copyright (C) 2013-2017 Daniel Ortiz-Mart\'inez, Adam Harasimowicz
 
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
/* Module: MathDefs.h                                               */
/*                                                                  */
/* Prototype file: MathDefs                                         */
/*                                                                  */
/* Description: Constants, typedefs and basic classes for           */
/*              mathematic functions.                               */
/*                                                                  */
/********************************************************************/

#ifndef _MathDefs_h
#define _MathDefs_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include<float.h>

//--------------- Constants ------------------------------------------

#define EPSILON                   (100 * FLT_EPSILON)
#define SMALL_LG_NUM              -99999

//---------------

#endif
