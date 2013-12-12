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
 
#ifndef _bleu_h
#define _bleu_h

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <math.h>
#include "myVector.h"
#include <string>
#include <utility>
#include "awkInputStream.h"
#include "ErrorDefs.h"

#define MAX_N 4

using namespace std;

int calc_bleu(const char* ref,
              const char* sys,
              float& bleu,
              float &bp,
              Vector<float>& bleu_n,
              int verbosity);

int calc_bleuf(FILE *reff,
               FILE *sysf,
               float& bleu,
               float &bp,
               Vector<float>& bleu_n,
               int verbosity);

void prec_n(Vector<std::string> refsen,
            Vector<std::string> syssen,
            unsigned int n,
            unsigned int& prec,
            unsigned int& total);

double my_log(double x);

#endif
