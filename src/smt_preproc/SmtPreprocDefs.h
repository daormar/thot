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
 * @file SmtPreprocDefs.h
 * 
 * @brief Defines constants to be used in pre-pos processing tasks.
 */

#ifndef _SmtPreprocDefs_h
#define _SmtPreprocDefs_h

//--------------- Constants ------------------------------------------

#define DISABLE_PREPROC             0
#define DISABLE_PREPROC_LABEL       "Preprocessing disabled"

#define XRCE_PREPROC1               1
#define XRCE_PREPROC1_LABEL         "XRCE_PREPROC type 1 (tok+categ+capit)"

#define XRCE_PREPROC2               2
#define XRCE_PREPROC2_LABEL         "XRCE_PREPROC type 2 (tok+categ+capit)"

#define XRCE_PREPROC3               3
#define XRCE_PREPROC3_LABEL         "XRCE_PREPROC type 3 (tok)"

#define XRCE_PREPROC4               4
#define XRCE_PREPROC4_LABEL         "XRCE_PREPROC type 4 (tok+categ)"

#define EU_PREPROC1                 5
#define EU_PREPROC1_LABEL           "EU_PREPROC type 1 (categ)"

#define EU_PREPROC2                 6
#define EU_PREPROC2_LABEL           "EU_PREPROC type 2 (tok+categ)"

#define END_OF_PREF_MARK            "<END_OF_PREF_MARK>"

#endif
