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

// Basic error correcting components
#define ECM        9

// Specific components for PbNoWpTm
#define LMODEL_NOWP       0
#define TSEGMLEN_NOWP     1
#define SJUMP_NOWP        2
#define SSEGMLEN_NOWP     3
#define PTS_NOWP          4
#define PST_NOWP          5

// Specific components for SimplePhrTransModel
#define LMODEL_SIMPLE     0
#define PST_SIMPLE        1

// Specific components for SimplePhrSwTm
#define LMODEL_SIMPLE     0
#define SWNOLEN_SIMPLE    1
#define SWLEN_SIMPLE      2

// Specific components for SimplePhrSwTm2
#define LMODEL_SIMPLE     0
#define PST_SIMPLE        1
#define SWLEN_SIMPLE      2

// Specific components for PhrLocalSwLiTm
#define SWLENLI           7

#endif
