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
 
/********************************************************************/
/*                                                                  */
/* Module: ThotDecoder.h                                            */
/*                                                                  */
/* Prototype file: ThotDecoder.h                                    */
/*                                                                  */
/* Description: thot decoder class.                                 */
/*                                                                  */
/********************************************************************/

#ifndef _ThotDecoder_h
#define _ThotDecoder_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#ifndef THOT_DISABLE_PREPROC_CODE
#include "XRCE_PrePosProcessor1.h" 
#include "XRCE_PrePosProcessor2.h" 
#include "XRCE_PrePosProcessor3.h" 
#include "XRCE_PrePosProcessor4.h" 
#include "EU_PrePosProcessor1.h" 
#include "EU_PrePosProcessor2.h"
#endif

#include "WeightUpdateUtils.h"

// Types defining decoder architecture
#include "SmtModelUtils.h"
#include "_pbTransModel.h"
#include "_phraseBasedTransModel.h"
#include "_phrSwTransModel.h"
#include "BaseSmtModel.h"
#include "BaseErrorCorrectionModel.h"
#include "ThotDecoderCommonVars.h"
#include "ThotDecoderPerUserVars.h"
#include "ThotDecoderState.h"
#include "ThotDecoderUserPars.h"
#include "ModelDescriptorUtils.h"

#include "StdCerrThreadSafePrint.h"
#include "StdCerrThreadSafeTidPrint.h"
#include <options.h>
#include <pthread.h>
#include <sstream>

//--------------- Constants ------------------------------------------

// Default parameter values
#define TDEC_W_DEFAULT                0.4
#define TDEC_A_DEFAULT               10
#define TDEC_E_DEFAULT                2
#define TDEC_HEUR_DEFAULT             LOCAL_TD_HEURISTIC
#define TDEC_NOMON_DEFAULT            0

#define MINIMUM_WORD_LENGTH_TO_EXPAND 1    // Define the minimum
                                           // length in characters that
                                           // is required to expand a
                                           // a word using the word
                                           // predictor

#define THOTDEC_NON_VERBOSE_MODE              0
#define THOTDEC_NORMAL_VERBOSE_MODE           1
#define THOTDEC_DEBUG_VERBOSE_MODE            2

//--------------- Classes --------------------------------------------

//--------------- ThotDecoder class

class ThotDecoder
{
 public:

      // Constructor
  ThotDecoder();

      // User related functions
  bool user_id_new(int user_id);
  void release_user_data(int user_id);
  
      // Functions to initialize the decoder
  int initUsingCfgFile(std::string cfgFile,
                       ThotDecoderUserPars& tdup,
                       int verbose);
  int initUserPars(int user_id,
                   const ThotDecoderUserPars& tdup,
                   int verbose);

      // Functions to test decoder configuration
  int testSoftwareModulesForModels(std::string cfgFile,
                                   int verbose);
  
      // Functions to train models
  int onlineTrainSentPair(int user_id,
                          const char *srcSent,
                          const char *refSent,
                          int verbose=0);
  void updateLogLinearWeights(std::string refSent,
                              WordGraph* wgPtr,
                              int verbose=0);
  int trainEcm(int user_id,
               const char *strx,
               const char *stry,
               int verbose=0);

      // Functions to translate sentences
  void translateSentence(int user_id,
                         const char *sentenceToTranslate,
                         std::string& result,
                         std::string& bestHypInfo,
                         int verbose=0);
  void sentPairVerCov(int user_id,
                      const char *srcSent,
                      const char *refSent,
                      std::string& result,
                      int verbose=0);
  
      // CAT-related functions
  void startCat(int user_id,
                const char *sentenceToTranslate,
                std::string &catResult,
                int verbose=0);
  void addStrToPref(int user_id,
                    const char *strToAddToPref,
                    const RejectedWordsSet& rejectedWords,
                    std::string &catResult,
                    int verbose=0);
  void setPref(int user_id,
               const char *prefStr,
               const RejectedWordsSet& rejectedWords,
               std::string &catResult,
               int verbose=0);
  void resetPrefix(int user_id,
                   int verbose=0);
  int use_caseconv(int user_id,
                    const char *caseConvFile,
                    int verbose=0);
  
      // Clear translator data structures
  void clearTrans(int verbose=0);

      // Function to print the models
  int printModels(int verbose=0);

      // Model weights related functions
  int printModelWeights(void);
  int printCatWeights(void);

      // Destructor
  ~ThotDecoder();

 private:

      // Data members
  std::map<int,size_t> userIdToIdx;
  std::vector<bool> idxDataReleased;
  ThotDecoderState tdState;
  ThotDecoderCommonVars tdCommonVars;
  std::vector<ThotDecoderPerUserVars> tdPerUserVarsVec;
  std::vector<std::string> totalPrefixVec;

      // Mutexes and conditions
  pthread_mutex_t user_id_to_idx_mut;
  pthread_mutex_t atomic_op_mut;
  pthread_mutex_t non_atomic_op_mut;
  pthread_mutex_t preproc_mut;
  pthread_cond_t non_atomic_op_cond;
  unsigned int non_atomic_ops_running;
  std::vector<pthread_mutex_t> per_user_mut;
  
      // Mutex- and condition-related functions
  void wait_on_non_atomic_op_cond(void);
  void increase_non_atomic_ops_running(void);
  void decrease_non_atomic_ops_running(void);

      // Functions to initialize translator
  bool featureBasedImplIsEnabled(void);
  void init_translator_legacy_impl(void);
  void init_translator_feat_impl(void);
  
      // Functions to load models
  BasePhraseModel* createPmPtr(std::string modelType);
  bool process_tm_descriptor(std::string tmDescFile,
                             int verbose/*=0*/);
  bool process_tm_files_prefix(std::string tmFilesPrefix,
                               int verbose/*=0*/);
  bool load_tm_legacy_impl(const char* tmFilesPrefix,
                           int verbose=0);
  bool load_tm_feat_impl(const char* tmFilesPrefix,
                         int verbose=0);
  BaseNgramLM<LM_State>* createLmPtr(std::string modelType);
  int createLangModelFeat(std::string featName,
                          const ModelDescriptorEntry& modelDescEntry,
                          LangModelFeat<SmtModel::HypScoreInfo>** langModelFeatPtrRef);
  bool load_lm_legacy_impl(const char* lmFileName,
                           int verbose=0);
  bool load_lm_feat_impl(const char* lmFileName,
                         int verbose=0);
  bool load_ecm(const char* ecmFilesPrefix,
                int verbose=0);

      // Functions to test model descriptors
  int testModulesInTmDesc(const char* tmDescFileName,
                           int verbose=0);
  int testTmModule(std::string soFileName,
                   int verbose=0);
  int testModulesInLmDesc(const char* lmDescFileName,
                          int verbose=0);
  int testLmModule(std::string soFileName,
                   int verbose=0);
  
      // Functions to print models
  int printModelsLegacyImpl(int verbose=0);
  int printModelsFeatImpl(int verbose=0);

      // Training-related functions
  void setOnlineTrainPars(OnlineTrainingPars onlineTrainingPars,
                          int verbose=0);

      // Functions to set decoder parameters
  void setNonMonotonicity(int nomon,
                          int verbose=0);
  void set_W(float W_par,
             int verbose=0);
  void set_S(int user_id,
             unsigned int S_par,
             int verbose=0);
  void set_A(unsigned int A_par,
             int verbose=0);
  void set_E(unsigned int E_par,
             int verbose=0);
  void set_be(int user_id,
              int _be,
              int verbose=0);
  bool set_G(int user_id,
             unsigned int G_par,
             int verbose=0);
  void set_h(unsigned int h_par,
             int verbose=0);
  bool set_np(int user_id,
              unsigned int np_par,
              int verbose=0);
  bool set_wgp(int user_id,
               float wgp_par,
               int verbose=0);
  void set_preproc(int user_id,
                   unsigned int preprocId_par,
                   int verbose=0);
  void set_tmw(std::vector<float> tmwVec_par,
               int verbose=0);
  void set_ecw(std::vector<float> ecwVec_par,
               int verbose=0);
  void set_catw(int user_id,
                std::vector<float> catwVec_par,
                int verbose=0);
  bool set_wgh(const char *wgHandlerFileName,
               int verbose=0);

      // Functions to handle variables for each user
  size_t get_vecidx_for_user_id(int user_id);
  int init_idx_data(size_t idx);
  void release_idx_data(size_t idx);

      // Auxiliary functions for translation
  std::string translateSentenceAux(size_t idx,
                                   std::string sentenceToTranslate,
                                   std::string& bestHypInfo,
                                   int verbose=0);

      // Auxiliary functions for online training
  void addSentenceToWordPred(std::string sentence,
                             int verbose=0);
  int onlineTrainFeats(std::string srcSent,
                       std::string refSent,
                       std::string sysSent,
                       int verbose=0);
  void onlineTrainLogLinWeights(size_t idx,
                                const char *srcSent,
                                const char *refSent,
                                int verbose=0);
  
      // Auxiliary functions for assisted translation
  void resetPrefixAux(size_t idx);
  void addStrToPrefAux(size_t idx,
                       const char *strToAddToPref,
                       const RejectedWordsSet& rejectedWords,
                       std::string &catResult,
                       int verbose=0);

      // Pre-posprocessing related functions
  std::string robustObtainFinalOutput(BasePrePosProcessor* prePosProcessorPtr,
                                      std::string unpreprocPref,
                                      std::string preprocPrefUnexpanded,
                                      std::string preprocPref,
                                      std::string trans,
                                      bool caseconv);
  std::string postprocWithCriticalError(BasePrePosProcessor* prePosProcessorPtr,
                                        std::string unpreprocPref,
                                        std::string preprocPrefUnexpanded,
                                        std::string preprocPref,
                                        std::string trans,
                                        bool caseconv);
  std::string robustMergeTransWithUserPref(std::string trans,
                                           std::string totalPrefix);
  std::string robustMergePostProcTransWithUserPref(std::string postproctrans,
                                                   std::string totalPrefix);
  std::string expandLastWord(std::string& partialSent);
  std::pair<Count,std::string> getBestSuffixGivenHist(std::vector<std::string> hist,
                                                      std::string input);
  std::pair<Count,std::string> getBestSuffixGivenHistFeatImpl(std::vector<std::string> hist,
                                                              std::string input);
  std::string getWordCompletion(std::string uncompleteWord,
                                std::string completeWord);
  std::string preprocLine(BasePrePosProcessor* prePosProcessorPtr,
                          std::string str,
                          bool caseconv,
                          bool keepPreprocInfo);
  std::string postprocLine(BasePrePosProcessor* prePosProcessorPtr,
                           std::string str,
                           bool caseconv);

      // Memory handling related functions
  bool instantiate_swm_info(const char* tmFilesPrefix,
                            int verbose=0);
  void deleteSwModelPtrs(void);
  void destroy_feat_impl(void);
  void destroy_legacy_impl(void);

      // Verbose output-related functions
  bool threadIdShouldBePrinted(int verbosity);
  int externalFuncVerbosity(int verbosity);
};
#endif
