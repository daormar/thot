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
 * @file ctimer.h
 * @brief Defines the ctimer function
 */

#ifndef _ctimer_h
#define _ctimer_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#else
#  define THOT_TIME_WITH_SYS_TIME 1
#endif /* HAVE_CONFIG_H */

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
  
#if THOT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <time.h>

#ifndef THOT_MINGW
#include <sys/times.h>
#endif
  
#include <unistd.h>

void ctimer(double *elapsed, double *ucpu, double *scpu);
// Function for time measuring
#ifdef __cplusplus  
}
#endif

#endif

