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
/* Module: getline                                                  */
/*                                                                  */
/* Definitions file: getline.c                                      */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "getline.h"

#ifndef THOT_HAVE_GETLINE
ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
  return getdelim( lineptr, n, '\n', stream );
}
#endif
