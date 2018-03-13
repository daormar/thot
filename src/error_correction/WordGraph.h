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
 * @file WordGraph.h
 *
 * @brief Declares the WordGraph class. This class implements a
 * word graph for being used in stack decoding.
 */

#ifndef _WordGraph_h
#define _WordGraph_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <math.h>
#include <set>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include "ErrorDefs.h"
#include "AwkInputStream.h"
#include "WordGraphArc.h"
#include "WordGraphArcId.h"
#include "WordGraphStateData.h"
#include "NbSearchHighLevelHyp.h"
#include "NbSearchHyp.h"
#include "NbSearchStack.h"
#include <algorithm>
#include <limits.h>

//--------------- Constants ------------------------------------------

#define INITIAL_STATE        0
#define INVALID_STATE        UINT_MAX
#define INVALID_ARCID        UINT_MAX
#define UNLIMITED_DENSITY   -1
#define DISABLE_WORDGRAPH    2
#define SMALL_SCORE          -999999999
#define NBEST_MAX_STACK_SIZE 10000

//--------------- Classes --------------------------------------------

//--------------- WordGraph template class

/**
 * @brief The WordGraph class implements a word graph for being
 * used in stack decoding.
 */

class WordGraph
{
 public:

  typedef std::set<HypStateIndex> FinalStateSet;

      // Constructor
  WordGraph(void);

      // Set weights for the components
  void setCompWeights(const std::vector<std::pair<std::string,float> >& _compWeights);
      // The setCompWeights() function revise all those arc scores for
      // which there exist a set of score components. As a result, the
      // arcScore data member of the WordGraphArc class is modified
  void getCompWeights(std::vector<std::pair<std::string,float> >& _compWeights)const;
  
      // Functions to extend word-graphs
  void addArc(HypStateIndex predStateIndex,
              HypStateIndex succStateIndex,
              const std::vector<std::string>& words,
              Score arcScore);
      // Word-graphs are restricted to have one initial state with index
      // equal to INITIAL_STATE. It is also recommended that the numbers
      // assigned to new states are as small as possible
  void addArcWithScrComps(HypStateIndex predStateIndex,
                          HypStateIndex succStateIndex,
                          const std::vector<std::string>& words,
                          Score arcScore,
                          std::vector<Score> scrVec);
      // The same as addArc, but a vector of score components is also
      // stored. IMPORTANT: scrVec must be an UNWEIGHTED vector
  void addFinalState(HypStateIndex finalStateIndex);
  void setInitialStateScore(Score _initialStateScore);

      // Functions to access word-graphs
  Score getInitialStateScore(void)const;
  std::pair<HypStateIndex,HypStateIndex> getHypStateIndexRange(void)const;
      // Returns range of states. Warning: this function does not
      // check if the word graph is empty, so the empty() function
      // should be called first
  std::pair<WordGraphArcId,WordGraphArcId> getArcIndexRange(void)const;
      // Returns range of arcs. Warning: this function does not
      // check if the word graph is empty, so the empty() function
      // should be called first
  WordGraphStateData getWordGraphStateData(HypStateIndex hypStateIndex)const;
  WordGraphArc wordGraphArcId2WordGraphArc(WordGraphArcId wordGraphArcId)const;
  void getArcsToPredStates(HypStateIndex hypStateIndex,
                           std::vector<WordGraphArc>& wgArcs)const;
  void getArcIdsToPredStates(HypStateIndex hypStateIndex,
                             std::vector<WordGraphArcId>& wgArcIds)const;
  void getArcsToSuccStates(HypStateIndex hypStateIndex,
                           std::vector<WordGraphArc>& wgArcs)const;
  void getArcIdsToSuccStates(HypStateIndex hypStateIndex,
                             std::vector<WordGraphArcId>& wgArcIds)const;
  FinalStateSet getFinalStateSet(void)const;
  bool stateIsFinal(HypStateIndex hypStateIndex)const;

      // Functions to calculate previous and rest scores for
      // each state
  void calcPrevScores(HypStateIndex idx,
                      const std::set<WordGraphArcId>& excludedArcs,
                      std::vector<Score>& prevScores,
                      std::vector<WordGraphArc>& bestPredArcForStateVec)const;
      // Calculate previous scores
  void calcPrevScoresWeights(HypStateIndex idx,
                             const std::set<WordGraphArcId>& excludedArcs,
                             const std::vector<float>& altCompWeights,
                             std::vector<Score>& prevScores,
                             std::vector<WordGraphArc>& bestPredArcForStateVec)const;
      // The same as the previous one, but a vector containing alternative
      // score component weights can be given
  void calcRestScores(std::vector<Score>& restScores)const;
      // Calculate rest scores from each node
  
      // IMPORTANT NOTE: these functions work correctly if and only if
      // whenever an arc is added with the addArc() function, the
      // predStateIndex state is not used as the succStateIndex argument
      // by subsequent calls to addArc(). If the previous condition is
      // true, then the arcs are topologically ordered

      // Specific algorithms for word-graphs
  Score bestPathFromFinalStateToIdx(HypStateIndex hypStateIndex,
                                    const std::set<WordGraphArcId>& excludedArcs,
                                    std::vector<WordGraphArc>& arcVec)const;
      // Stores best path from state in arcVec. Returns score for best
      // path.
  Score bestPathFromFinalStateToIdxWeights(HypStateIndex hypStateIndex,
                                           const std::set<WordGraphArcId>& excludedArcs,
                                           const std::vector<float>& altCompWeights,
                                           std::vector<WordGraphArc>& arcVec)const;
      // The same as the previous one, but it allows to change the weights of
      // the score components
     
      // Functions related to word-graph pruning
  unsigned int getNumberOfPrunedAndNonPrunedArcs(void)const;

  unsigned int getNumberOfNonPrunedArcs(void)const;

  float calculateDensity(unsigned int numRefSentWords)const;
      // Get current density of the word graph. The word graph density
      // is calculated as the total number of edges divided by the
      // number of reference sentence words
  
  unsigned int prune(float threshold);
      // Prune the word graph for the given threshold. Pruning is done
      // as it is described in [Ueffing et al. 2002] ("Generation of word-graphs in SMT").
      // If threshold=0, then no pruning is performed. If threshold=1,
      // only the best arc arriving to each state is retained.
      // The function returns the number of pruned arcs.
      // 
      // IMPORTANT NOTE: this function works correctly if and only if
      // whenever an arc is added with the addArc() function, the
      // predStateIndex state is not used as the succStateIndex argument
      // by subsequent calls to addArc(). If the previous condition is
      // true, then the arcs are topologically ordered.
  bool arcPruned(WordGraphArcId wordGraphArcId)const;
      // Return true if a given arc was pruned

      // Function to obtain n-best list
  void obtainNbestList(unsigned int len,
                       std::vector<std::pair<Score,std::string> >& nblist,
                       std::vector<NbSearchHighLevelHyp>& highLevelHypList,
                       std::vector<std::vector<Score> >& scoreCompsVec,
                       int verbosity=false);
  
      // Function to obtain a wordgraph composed of useful states
      // (if wordgraph has been pruned, this function obtains a pruned
      // word-graph composed of useful states)
  void obtainWgComposedOfUsefulStates(void);
  
      // Function to obtain a wordgraph whose arcs are topologically
      // ordered
  void orderArcsTopol(void);

      // Functions to load word graphs
  bool load(const char * filename);

      // Functions to print word graphs
      //
      // NOTE: If the printOnlyUsefulStates flag is equal to true, then
      // a word graph composed of only useful states is printed (in this
      // case the states are not remapped, if remapping is required, then
      // use the obtainWgComposedOfUsefulStates() function)
  bool print(const char* filename,
             bool printOnlyUsefulStates=false)const;
  void print(std::ostream &outS,
             bool printOnlyUsefulStates=false)const;
  
      // size related functions
  bool empty(void)const;
  size_t numArcs(void)const;
  size_t numStates(void)const;
  
      // clear() function
  void clear(void);

 protected:
  typedef std::vector<WordGraphArc> WordGraphArcs;
  typedef std::vector<WordGraphStateData> WordGraphStates;
    
  WordGraphArcs wordGraphArcs;
  std::vector<bool> arcsPruned;
  WordGraphStates wordGraphStates;
  FinalStateSet finalStateSet;
  Score initialStateScore;
  std::vector<std::pair<std::string,float> > compWeights;
  std::vector<std::vector<Score> > scrCompsVec;

      // Auxiliary functions for pruning
  unsigned int pruneArcsToPredStates(float threshold);
  bool finalStatePruned(HypStateIndex hypStateIndex)const;
  
      // Miscelaneous functions
  void rescoreArcsGivenWeights(const std::vector<std::pair<std::string,float> >& _compWeights);
  bool checkIfAltWeightsAppliable(const std::vector<float>& altCompWeights)const;
  void obtainNbSearchHeurInfo(std::vector<Score>& heurForEachState);
  NbSearchHighLevelHyp hypToHighLevelHyp(const NbSearchHyp& hyp);
  void nbSearch(unsigned int len,
                const std::vector<Score>& heurForEachState,
                std::vector<std::pair<Score,std::string> >& nblist,
                std::vector<NbSearchHyp>& hypList,
                std::vector<std::vector<Score> >& scoreCompsVec,
                int verbosity=false);
  bool hypIsComplete(const NbSearchHyp& nbSearchHyp);
  std::string stringAssociatedToHyp(const NbSearchHyp& nbSearchHyp,
                                    std::vector<Score>& scoreComps);
  Score bestPathFromFinalStateToIdxAux(HypStateIndex hypStateIndex,
                                       const std::vector<Score>& prevScores,
                                       const std::vector<WordGraphArc>& bestPredArcForStateVec,
                                       std::vector<WordGraphArc>& arcVec)const;
      // Stores best path from final state to given state in
      // arcVec. Returns score for best path. The prevScores and
      // bestPredArcForStateVec vectors have to be
      // previously obtained by means of the
      // calcPrevScores() function. Such function has to
      // be invoked for the same hyp. state index. Otherwise the result
      // is undefined.

  void obtainStatesReachableFromInit(std::vector<bool>& stateReachableFromInitVec)const;
      // Obtains bool vector indicating whether a state is reachable
      // from the initial state

  void obtainUsefulStates(std::vector<bool>& stateIsUsefulVec,
                          std::map<HypStateIndex,HypStateIndex>& remappedStates)const;
      // Obtains a bool vector indicating whether a state is useful (a
      // state is useful if there is a path from such state to a final state).
      // In addition to the above mentioned bool vector, the function
      // also returns a mapping from the current useful state indices to
      // a set of contiguous state indices.
      // This information can be used to generate a new word-graph
      // composed only of useful states.
};

#endif
