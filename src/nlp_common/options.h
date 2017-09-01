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
 * @file options.h
 * @brief Declares several functions to take command-line parameters.
 */

#ifndef _options_h
#define _options_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include "mem_alloc_utils.h"
#include <map>
#include <string.h>
#include <string>
#include <vector>
#include "ErrorDefs.h"
#include "awkInputStream.h"

bool verifyParams(int argc, char **argv, std::map<std::string,bool> posPars);
// Reads the boolean value associated to a given label.

bool existParam(int argc, char **argv, const char *pszLabel);
// Verifies if a parameter was introduced in the command line.

int readOption(int argc, char **argv, const char *pszLabel);
// Returns 0 if a certain option was introduced in the command line.

int isOption(const char *str);
// Returns true if str is an option

int readBool(int argc, char **argv, const char *pszLabel, bool *Value);
// Reads the boolean value associated to a given label.

int readInt(int argc, char **argv, const char *pszLabel, int *Value);
// Reads the integer associated to a given label.

int readUnsignedInt(int argc,
                    char **argv,
                    const char *pszLabel,
                    unsigned int *Value);
// Reads the unsigned integer associated to a given label.

int readFloat(int argc, char **argv, const char *pszLabel, float *Value);
// Reads the floating point number associated to a given label.

int readFloatSeq(int argc,
                 char **argv,
                 const char *pszLabel,
                 std::vector<float>& floatVec);
// Reads a sequence of floating point numbers associated to a given label

int readDouble(int argc, char **argv, const char *pszLabel, double *Value);
// Reads the double associated to a given label.

int readString(int argc, char **argv, const char *pszLabel, char *Value);
// Reads the string associated to a given label.

int readStringSafe(int argc, char **argv, const char *pszLabel, char **strPtr);
// Reads the string associated to a given label. Size of the string
// pointed by strPtr is modified if necessary

int readTwoStrings(int argc,
                   char **argv,
                   const char *pszLabel,
                   char *val1,
                   char *val2);
// Reads two consecutive strings associated to a given label.

int readTwoStringsSafe(int argc,
                       char **argv,
                       const char *pszLabel,
                       char** strPtr1,
                       char** strPtr2);
// Reads two consecutive strings associated to a given label. Sizes of
// the pointed strings are modified if necessary

int readSTLstring(int argc, char **argv, const char *pszLabel, std::string *Value);
// Reads the stl string associated to a given label.

int readTwoSTLstrings(int argc,
                      char **argv,
                      const char *pszLabel,
                      std::string *val1,
                      std::string *val2);
// Reads two consecutive strings associated to a given label.

int readStringSeq(int argc,
                  char **argv,
                  const char *pszLabel,
                  std::vector<std::string>& strVec);
// Reads a sequence of strings associated to a given label

std::vector<std::string> argv2argv_stl(int argc, char **argv);
// Converts const char **argv into std::vector<std::string>

bool extractParsFromFile(const char* filename,
                         int& argc,
                         std::vector<std::string>& argv_stl,
                         std::string comment="#");
// Given a file name, extracts the parameters contained in it, ignoring
// those lines starting with the string "comment"

#endif
