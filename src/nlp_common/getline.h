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
 * @file getline.h
 * @brief Defines the getline function
 */

#ifndef _getline_h
#define _getline_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#else
#  define THOT_TIME_WITH_SYS_TIME 1
#endif /* HAVE_CONFIG_H */

#include "getdelim.h"

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef THOT_HAVE_GETLINE
  ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif
#ifdef __cplusplus  
}
#endif

#endif
