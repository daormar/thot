#ifndef _chrf_h
#define _chrf_h

#include <stdio.h>
#include <math.h>
#include "myVector.h"
#include <string>
#include <utility>
#include "awkInputStream.h"
#include "ErrorDefs.h"
#include "StrProcUtils.h"


#define MAX_NGRAM_LENGTH 4
#define BETA 3
#define CONSIDER_WHITESPACE false

using namespace std;

int calculate_chrf_file_name(const char* ref,
                             const char* sys,
                             float& chrf,
                             Vector<float>& chrf_n,
                             int verbosity);

int calculate_chrf_file(FILE *refFile,
                        FILE *sysFile,
                        float& chrf,
                        Vector<float>& chrf_n,
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
