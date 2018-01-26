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
 * @file StatModelDefs.h
 * 
 * @brief Constants, typedefs and basic classes for statistical models.
 */

#ifndef _StatModelDefs_h
#define _StatModelDefs_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#ifdef THOT__LARGEFILE_SOURCE
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE 1
#endif
#endif

// IMPORTANT WARNING: _FILE_OFFSET_BITS should be defined before any STL
// header files to avoid conflicts and execution errors that can be
// difficult to be debugged
#if defined(_SYS_STAT_H)
#error "sys/stat.h should not have been included before the _FILE_OFFSET_BITS macro is set!"
#endif

#ifdef THOT__FILE_OFFSET_BITS
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS  THOT__FILE_OFFSET_BITS
#endif
#endif

#ifdef THOT__LARGE_FILES 
#ifndef _LARGE_FILES 
#define _LARGE_FILES
#endif
#endif

#include "WordIndex.h"
#include "PositionIndex.h"
#include "Prob.h"
#include "Count.h"
#include "Score.h"
#include "LogCount.h"

//--------------- Constants ------------------------------------------

#define NULL_WORD         0
#define NULL_WORD_STR     "NULL"
#define UNK_WORD          1
#define UNK_WORD_STR      "UNKNOWN_WORD"
#define UNUSED_WORD       2
#define UNUSED_WORD_STR   "<UNUSED_WORD>"

//--------------- typedefs -------------------------------------------

typedef int ClassIndex;

//---------------

#endif
