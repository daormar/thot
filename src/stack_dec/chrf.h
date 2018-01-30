/*
thot package for statistical machine translation
Copyright (C) 2017 Marina Fomicheva
 
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
 * @file chrf.h
 *
 * @brief Code useful to calculate the ChrF measure.
 */

#ifndef _chrf_h
#define _chrf_h

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include "AwkInputStream.h"
#include "ErrorDefs.h"
#include "StrProcUtils.h"

#define MAX_NGRAM_LENGTH 4
#define BETA 3
#define CONSIDER_WHITESPACE true

int calculate_chrf_file_name(const char* ref,
                             const char* sys,
                             double& chrf,
                             std::vector<double>& chrf_n,
                             int verbosity);

int calculate_chrf_file(FILE *refFile,
                        FILE *sysFile,
                        double& chrf,
                        std::vector<double>& chrf_n,
                        int verbosity);

void calculate_chrf(const std::string& refSentence,
                    const std::string& sysSentence,
                    double& chrf);

void count_ngrams(const std::string& refSentence,
                  const std::string& sysSentence,
                  unsigned int ngramLength,
                  float& precision,
                  float& recall,
                  unsigned int& count);

#endif
