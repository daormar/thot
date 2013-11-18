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
 
#ifndef _StackDecWgProcessorForAnlpTypes_h
#define _StackDecWgProcessorForAnlpTypes_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

//--------------- Constants ------------------------------------------

#define STD_WGP                1
#define STD_WGP_LABEL          "Standard word-graph processor"
#define STD_WGP_NOTES          ""

// Set the error correcting model type used by uncoupled assisted
// translators.  The valid options are: STD_WGP, ... These options can
// be given before executing the configure script.
//
// Example:
// $ export THOT_WGP_TYPE="STD_WGP"
// $ configure --prefix=$PWD
// $ make
// $ make install

#if THOT_WGP_TYPE == STD_WGP
# include <WgProcessorForAnlp.h>
# define CURR_WGP_TYPE WgProcessorForAnlp
# define CURR_WGP_LABEL STD_WGP_LABEL
# define CURR_WGP_NOTES STD_WGP_NOTES

#endif

#endif
