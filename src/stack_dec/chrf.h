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
