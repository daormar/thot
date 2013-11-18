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
/* Module: ErrorDefs                                                */
/*                                                                  */
/* Prototype file: ErrorDefs                                        */
/*                                                                  */
/* Description: Return codes for functions.                         */
/*                                                                  */
/********************************************************************/

#ifndef _ErrorDefs
#define _ErrorDefs

//--------------- Constants ------------------------------------------

// General constants
#define OK             0
#define ERROR          1

// Downhill simplex optimization constants
#define DSO_EVAL_FUNC  2 // Error code to return when a new function evaluation
                         // is required
#define DSO_NMAX_ERROR 3 // Error code to return when the maximum number of
                         // function evaluations has been exceeded

#endif
