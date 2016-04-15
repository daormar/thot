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
 
#ifndef _ScoreCompDefs_h
#define _ScoreCompDefs_h


//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

// Basic phrase-based model components
#define WPEN       0
#define LMODEL     1
#define TSEGMLEN   2
#define SJUMP      3
#define SSEGMLEN   4
#define PTS        5
#define PST        6

// Basic lexical components
#define SW         7
#define INVSW      8

// Basic components for PhrSwTransModel
#define SWNOLEN    7
#define SWLEN      8

// Specific components for PhrLocalSwLiTm
#define SWLENLI           7

#endif
