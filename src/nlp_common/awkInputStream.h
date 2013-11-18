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
 
/* ------------------------------------------ */
/*                                            */
/* awkInputStream class                       */
/*                                            */
/* Daniel Ortiz <dortiz@iti.upv.es>, May 2006 */
/* ------------------------------------------ */

#ifndef _awk_input_stream_h
#define _awk_input_stream_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#ifdef THOT__LARGEFILE_SOURCE
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE 1
#endif
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

#include "ErrorDefs.h"
#include "getline.h"
#include <stdio.h>
#include <stdlib.h>
#if THOT_HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <string>
#include <iostream>
#include <fstream>

//--------------- awkInputStream class: awk-like input stream class

class awkInputStream
{
 public:
	unsigned int NF;
	unsigned int FNR;
        
	char FS;
	char fileName[512];
	
	awkInputStream(void);
	awkInputStream& operator= (const awkInputStream &awk);
	bool getline(void);
    std::string dollar(unsigned int n);
    bool open(const char *str);
    bool open_stream(FILE *stream);
	void close(void);
	bool rewind(void);
	void printFields(void);	
	~awkInputStream();
	
 protected:        
	std::string fieldStr;
    char * buff;
    size_t buftlen;
    FILE* filePtr;
    bool fopen_called;
    
	int  get_NF(void);
	void retrieveField(unsigned int n);
};

#endif
