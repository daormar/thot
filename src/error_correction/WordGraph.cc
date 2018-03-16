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
 * @file WordGraph.cc
 * 
 * @brief Definitions file for WordGraph.h
 */

//--------------- Include files --------------------------------------

#include "WordGraph.h"

//--------------- WordGraph class function definitions

WordGraph::WordGraph(void)
{
  initialStateScore=0;
}

//---------------------------------------
void WordGraph::setCompWeights(const std::vector<std::pair<std::string,float> >& _compWeights)
{
      // Set new component weight vector
  compWeights=_compWeights;

      // Re-score arcs
  rescoreArcsGivenWeights(compWeights);
}

//---------------------------------------
void WordGraph::rescoreArcsGivenWeights(const std::vector<std::pair<std::string,float> >& _compWeights)
{
      // Iterate over arcs
  for(WordGraphArcId wgArcId=0;wgArcId<wordGraphArcs.size();++wgArcId)
  {
        // Check if there is a component vector for wgArcId
    if(wgArcId<scrCompsVec.size())
    {
          // Check if the component vector has the same number of
          // components as the weight vector
      if(scrCompsVec[wgArcId].size()==_compWeights.size())
      {
            // Re-score arc
        wordGraphArcs[wgArcId].arcScore=0;
        for(unsigned int i=0;i<_compWeights.size();++i)
          wordGraphArcs[wgArcId].arcScore+=_compWeights[i].second * scrCompsVec[wgArcId][i];
      }
    }
  }
}

//---------------------------------------
void WordGraph::getCompWeights(std::vector<std::pair<std::string,float> >& _compWeights)const
{
  _compWeights=compWeights;
}

//---------------------------------------
void WordGraph::addArc(HypStateIndex predStateIndex,
                       HypStateIndex succStateIndex,
                       const std::vector<std::string>& words,
                       Score arcScore)
{
  WordGraphArc wordGraphArc;
  WordGraphArcId wordGraphArcId;
  
      // Fill wordGraphArc data structure
  wordGraphArc.predStateIndex=predStateIndex;
  wordGraphArc.succStateIndex=succStateIndex;
  wordGraphArc.arcScore=arcScore;
  wordGraphArc.words=words;

      // Insert arc and retrieve index
  wordGraphArcs.push_back(wordGraphArc);
  wordGraphArcId=wordGraphArcs.size()-1;

      // Register arc as not selected for pruning
  arcsPruned.push_back(false);
  
      // Add the new arc   
  if(succStateIndex<wordGraphStates.size())
  {
        // Add arc to predecessor and succesor states
    wordGraphStates[predStateIndex].arcsToSuccStates.push_back(wordGraphArcId);
    wordGraphStates[succStateIndex].arcsToPredStates.push_back(wordGraphArcId);
  }
  else
  {
        // There were not any arcs to the successor state
    WordGraphStateData wordGraphStateData;

        // Add the required number of empty elements to wordGraphStates
    while(succStateIndex>=wordGraphStates.size())
      wordGraphStates.push_back(wordGraphStateData);

        // Add arc to succesor state
    wordGraphStates[predStateIndex].arcsToSuccStates.push_back(wordGraphArcId);

        // Add arc to predecessor state
    wordGraphStates[succStateIndex].arcsToPredStates.push_back(wordGraphArcId);
  }

      // Add empty score vector
  std::vector<Score> emptyScrVec;
  scrCompsVec.push_back(emptyScrVec);
}

//---------------------------------------
void WordGraph::addArcWithScrComps(HypStateIndex predStateIndex,
                                   HypStateIndex succStateIndex,
                                   const std::vector<std::string>& words,
                                   Score arcScore,
                                   std::vector<Score> scrVec)
{
      // Add arc
  addArc(predStateIndex,succStateIndex,words,arcScore);

      // Store components
  std::vector<Score> emptyScrVec;
  while(scrCompsVec.size()!=wordGraphArcs.size())
  {
    scrCompsVec.push_back(emptyScrVec);
  }
  scrCompsVec[scrCompsVec.size()-1]=scrVec;
}

//---------------------------------------
void WordGraph::addFinalState(HypStateIndex finalStateIndex)
{
  FinalStateSet::iterator finalStateSetIter;

  finalStateSetIter=finalStateSet.find(finalStateIndex);
  if(finalStateSetIter==finalStateSet.end())
  {
    finalStateSet.insert(finalStateIndex);
  }
}

//---------------------------------------
void WordGraph::setInitialStateScore(Score _initialStateScore)
{
  initialStateScore=_initialStateScore;
}

//---------------------------------------
Score WordGraph::getInitialStateScore(void)const
{
  return initialStateScore;
}

//---------------------------------------
std::pair<HypStateIndex,HypStateIndex> WordGraph::getHypStateIndexRange(void)const
{
  if(wordGraphStates.empty())
    return std::make_pair(INVALID_STATE,INVALID_STATE);
  else
    return std::make_pair(INITIAL_STATE,wordGraphStates.size()-1);
}

//---------------------------------------
std::pair<WordGraphArcId,WordGraphArcId> WordGraph::getArcIndexRange(void)const
{
  if(wordGraphArcs.empty())
    return std::make_pair(INVALID_ARCID,INVALID_ARCID);  
  else
    return std::make_pair(0,wordGraphArcs.size()-1);  
}

//---------------------------------------
WordGraphStateData WordGraph::getWordGraphStateData(HypStateIndex hypStateIndex)const
{  
  if(hypStateIndex<wordGraphStates.size())
  {
    return wordGraphStates[hypStateIndex];
  }
  else
  {
    WordGraphStateData wordGraphStateData;
    return wordGraphStateData;
  }
}

//---------------------------------------
WordGraphArc WordGraph::wordGraphArcId2WordGraphArc(WordGraphArcId wordGraphArcId)const
{
  if(wordGraphArcId<wordGraphArcs.size())
  {
    return wordGraphArcs[wordGraphArcId];
  }
  else
  {
    WordGraphArc wordGraphArc;
    wordGraphArc.predStateIndex=INVALID_STATE;
    wordGraphArc.succStateIndex=INVALID_STATE;
    wordGraphArc.arcScore=0;
    
    return wordGraphArc;
  }
}

//---------------------------------------
void WordGraph::getArcsToPredStates(HypStateIndex hypStateIndex,
                                    std::vector<WordGraphArc>& wgArcs)const
{
  std::vector<WordGraphArcId> wgArcIds;

  getArcIdsToPredStates(hypStateIndex,wgArcIds);

  wgArcs.clear();
  for(unsigned int i=0;i<wgArcIds.size();++i)
  {
    wgArcs.push_back(wordGraphArcId2WordGraphArc(wgArcIds[i]));
  }  
}

//---------------------------------------
void WordGraph::getArcIdsToPredStates(HypStateIndex hypStateIndex,
                                      std::vector<WordGraphArcId>& wgArcIds)const
{
  if(hypStateIndex<wordGraphStates.size())
  {
    std::vector<WordGraphArcId>::const_iterator arcIdVecIter;

    wgArcIds.clear();
        // Iterate over the arcs to predecessors
    for(arcIdVecIter=wordGraphStates[hypStateIndex].arcsToPredStates.begin();arcIdVecIter!=wordGraphStates[hypStateIndex].arcsToPredStates.end();++arcIdVecIter)
    {
      if(!arcsPruned[*arcIdVecIter])
        wgArcIds.push_back(*arcIdVecIter);
    }
  }
  else
  {
    wgArcIds.clear();
  }  
}

//---------------------------------------
void WordGraph::getArcsToSuccStates(HypStateIndex hypStateIndex,
                                    std::vector<WordGraphArc>& wgArcs)const
{
  std::vector<WordGraphArcId> wgArcIds;

  getArcIdsToSuccStates(hypStateIndex,wgArcIds);

  wgArcs.clear();
  for(unsigned int i=0;i<wgArcIds.size();++i)
  {
    wgArcs.push_back(wordGraphArcId2WordGraphArc(wgArcIds[i]));
  }  
}

//---------------------------------------
void WordGraph::getArcIdsToSuccStates(HypStateIndex hypStateIndex,
                                      std::vector<WordGraphArcId>& wgArcIds)const
{
  if(hypStateIndex<wordGraphStates.size())
  {
    std::vector<WordGraphArcId>::const_iterator arcIdVecIter;

    wgArcIds.clear();
        // Iterate over the arcs to succesors
    for(arcIdVecIter=wordGraphStates[hypStateIndex].arcsToSuccStates.begin();arcIdVecIter!=wordGraphStates[hypStateIndex].arcsToSuccStates.end();++arcIdVecIter)
    {
      if(!arcsPruned[*arcIdVecIter])
        wgArcIds.push_back(*arcIdVecIter);
    }
  }
  else
  {
    wgArcIds.clear();
  }    
}

//---------------------------------------
WordGraph::FinalStateSet
WordGraph::getFinalStateSet(void)const
{
  return finalStateSet;
}

//---------------------------------------
bool WordGraph::stateIsFinal(HypStateIndex hypStateIndex)const
{
  FinalStateSet::const_iterator finalStateSetIter;

  finalStateSetIter=finalStateSet.find(hypStateIndex);
  if(finalStateSetIter==finalStateSet.end())
    return false;
  else
    return true;
}

//---------------------------------------
unsigned int WordGraph::getNumberOfPrunedAndNonPrunedArcs(void)const
{
  return wordGraphArcs.size();
}

//---------------------------------------
unsigned int WordGraph::getNumberOfNonPrunedArcs(void)const
{
  unsigned int numArcsNotPruned=0;
  for(unsigned int i=0;i<wordGraphArcs.size();++i)
  {
    if(!arcPruned(i))
      ++numArcsNotPruned;
  }
  return numArcsNotPruned;
}

//---------------------------------------
float WordGraph::calculateDensity(unsigned int numRefSentWords)const
{
  return (float) getNumberOfNonPrunedArcs()/numRefSentWords;
}

//---------------------------------------
unsigned int WordGraph::prune(float threshold)
{
  if(threshold==UNLIMITED_DENSITY)
  {
        // unlimited density, every arc is marked as not pruned
    for(unsigned int i=0;i<arcsPruned.size();++i)
      arcsPruned[i]=false;
    return 0;
  }
  else
  {
        // Prune arcs to predecessor states
    unsigned int numPrunedArcs=pruneArcsToPredStates(threshold);

    return numPrunedArcs;
  }
}

//---------------------------------------
bool WordGraph::arcPruned(WordGraphArcId wordGraphArcId)const
{
  return arcsPruned[wordGraphArcId];
}

//---------------------------------------
void WordGraph::obtainNbestList(unsigned int len,
                                std::vector<std::pair<Score,std::string> >& nblist,
                                std::vector<NbSearchHighLevelHyp>& highLevelHypList,
                                std::vector<std::vector<Score> >& scoreCompsVec,
                                int verbosity/*=false*/)
{
      // Check if word-graph is empty
  if(wordGraphArcs.empty())
  {
        // clear nblist and scoreCompsVec output variables
    nblist.clear();
    highLevelHypList.clear();
    scoreCompsVec.clear();
  }
  else
  {
        // Word-graph is not empty
        // Obtain heuristic values
    std::vector<Score> heurForEachState;
    obtainNbSearchHeurInfo(heurForEachState);
  
        // Execute A-star search
    std::vector<NbSearchHyp> hypList;
    nbSearch(len,heurForEachState,nblist,hypList,scoreCompsVec,verbosity);

        // Obtain high level hypothesis list
    highLevelHypList.clear();
    for(unsigned int i=0;i<hypList.size();++i)
    {
      highLevelHypList.push_back(hypToHighLevelHyp(hypList[i]));
    }
  }
}

//---------------------------------------
NbSearchHighLevelHyp WordGraph::hypToHighLevelHyp(const NbSearchHyp& hyp)
{
  NbSearchHighLevelHyp result;
  for(unsigned int i=0;i<hyp.size();++i)
  {
    result.push_back(wordGraphArcId2WordGraphArc(hyp[i]));
  }
  return result;
}

//---------------------------------------
void WordGraph::obtainNbSearchHeurInfo(std::vector<Score>& heurForEachState)
{
      // Clear vector
  heurForEachState.clear();
      // Initialize costs
  heurForEachState.insert(heurForEachState.begin(),wordGraphStates.size(),SMALL_SCORE);
      // Set zero cost for final states
  FinalStateSet::const_iterator iter;
  for(iter=finalStateSet.begin();iter!=finalStateSet.end();++iter)
    heurForEachState[*iter]=0;
      
      // Explore arcs in reverse order
      // WARNING: arcs must be topologically ordered
  if(!wordGraphArcs.empty())
  {
    for(WordGraphArcId wgArcId=0;wgArcId<wordGraphArcs.size();++wgArcId)
    {
      WordGraphArcId reverseWgArcId=wordGraphArcs.size()-wgArcId-1;
      if(!arcPruned(reverseWgArcId))
      {
        WordGraphArc wgArc=wordGraphArcId2WordGraphArc(reverseWgArcId);
        Score scr=wgArc.arcScore+heurForEachState[wgArc.succStateIndex];
        if(heurForEachState[wgArc.predStateIndex]<scr)
          heurForEachState[wgArc.predStateIndex]=scr;
      }
    }
  }
}

//---------------------------------------
void WordGraph::nbSearch(unsigned int len,
                         const std::vector<Score>& heurForEachState,
                         std::vector<std::pair<Score,std::string> >& nblist,
                         std::vector<NbSearchHyp>& hypList,
                         std::vector<std::vector<Score> >& scoreCompsVec,
                         int verbosity/*=false*/)
{
  // Perform A-star search

      // Create null hypothesis
  NbSearchHyp nbSearchHyp;

      // Declare stack and set maximum size
  NbSearchStack nbSearchStack;
  nbSearchStack.setMaxStackSize(NBEST_MAX_STACK_SIZE);

      // Push null hypothesis
  nbSearchStack.push(initialStateScore+heurForEachState[INITIAL_STATE],nbSearchHyp);

      // Declare stack to store complete hyps
  NbSearchStack completeHypStack;
  
      // Iterate
  unsigned int numIters=0;
  bool end=false;
  while(!end)
  {
    if(verbosity>=1)
      std::cerr<<"* Iteration "<<numIters<<std::endl;
    
        // Check if there are "len" complete hypotheses
    if(completeHypStack.size()>=len)
    {
      end=true;
    }
    else
    {
          // Check if stack is empty
      if(!nbSearchStack.empty())
      {
            // Pop top of the stack
        std::pair<Score,NbSearchHyp> scrHypPair=nbSearchStack.top();
        nbSearchStack.pop();

            // Obtain index of last state
        HypStateIndex lastHypStateIndex;
        if(scrHypPair.second.empty())
          lastHypStateIndex=INITIAL_STATE;
        else
        {
          WordGraphArc wgArc=wordGraphArcId2WordGraphArc(scrHypPair.second.back());
          lastHypStateIndex=wgArc.succStateIndex;
        }
        
            // Subtract heuristic
        scrHypPair.first-=heurForEachState[lastHypStateIndex];

        if(verbosity>=1)
        {
          std::cerr<<"- Top of the stack: "<<scrHypPair.first<<" ;";
          for(unsigned int j=0;j<scrHypPair.second.size();++j)
            std::cerr<<" "<<scrHypPair.second[j];
          std::cerr<<std::endl;
        }

            // Check if hyp is complete
        if(hypIsComplete(scrHypPair.second))
        {
          completeHypStack.push(scrHypPair.first,scrHypPair.second);
        }
        else
        {
          if(verbosity>=1)
            std::cerr<<"- Expanding top of the stack..."<<std::endl;

            // Expand hypothesis
          std::vector<WordGraphArcId> wgArcIds;
          getArcIdsToSuccStates(lastHypStateIndex,wgArcIds);

          std::vector<std::pair<Score,NbSearchHyp> > scrHypPairVec;
          for(unsigned int i=0;i<wgArcIds.size();++i)
          {
            if(!arcPruned(wgArcIds[i]))
            {
              WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcIds[i]);
              std::pair<Score,NbSearchHyp> newScrHypPair;
              newScrHypPair=scrHypPair;
                  // Obtain new score
              newScrHypPair.first+=wgArc.arcScore;
                  // Add heuristic
              newScrHypPair.first+=heurForEachState[wgArc.succStateIndex];   
                  // Add new arc
              newScrHypPair.second.push_back(wgArcIds[i]);
                  // Push into vector
              scrHypPairVec.push_back(newScrHypPair);

              if(verbosity>=1)
              {
                std::cerr<<"  Adding extension, score contribution: "<<wgArc.arcScore<<" ; successor state: "<<wgArc.succStateIndex<<std::endl;
              }
            }
          }
          
            // Push results into the stack
          for(unsigned int i=0;i<scrHypPairVec.size();++i)
          {
            nbSearchStack.push(scrHypPairVec[i].first,scrHypPairVec[i].second);
          }
        }
      }
      else
      {
            // Stack is empty
        end=true;
      }
    }
    ++numIters;
    if(verbosity>=1) std::cerr<<std::endl;
  }          
      // Obtain result
  nblist.clear();
  hypList.clear();
  scoreCompsVec.clear();
  if(verbosity>=1)
  {
    std::cerr<<"* Verbose info about complete hypotheses..."<<std::endl;
  }
  
  while(!completeHypStack.empty())
  {
        // Pop top of the stack
    std::pair<Score,NbSearchHyp> scrHypPair=completeHypStack.top();
    completeHypStack.pop();

        // Add hypothesis to list
    hypList.push_back(scrHypPair.second);
    
        // Obtain string from hyp
    std::vector<Score> scoreComps;
    std::string translation=stringAssociatedToHyp(scrHypPair.second,scoreComps);
    if(!scoreComps.empty()) scoreCompsVec.push_back(scoreComps);
      
        // Add to vector
    nblist.push_back(make_pair(scrHypPair.first,translation));

        // Print verbose information
    if(verbosity>=1)
    {
      std::cerr<<scrHypPair.first<<" ||| "<<translation<<" |||";
      for(unsigned int j=0;j<scrHypPair.second.size();++j)
      {
        WordGraphArc wgArc=wordGraphArcId2WordGraphArc(scrHypPair.second[j]);
        HypStateIndex hidx=wgArc.succStateIndex;
        std::cerr<<" "<<hidx;
      }
      std::cerr<<std::endl;
    }
  }
}

//---------------------------------------
bool WordGraph::hypIsComplete(const NbSearchHyp& nbSearchHyp)
{
  if(nbSearchHyp.empty()) return false;
  else
  {
    WordGraphArc wgArc=wordGraphArcId2WordGraphArc(nbSearchHyp.back());
    HypStateIndex hidx=wgArc.succStateIndex;
    if(stateIsFinal(hidx))
      return true;
    else
      return false;
  }
}

//---------------------------------------
std::string WordGraph::stringAssociatedToHyp(const NbSearchHyp& nbSearchHyp,
                                             std::vector<Score>& scoreComps)
{
  std::string str;
  for(unsigned int i=0;i<nbSearchHyp.size();++i)
  {
    WordGraphArcId wgArcId=nbSearchHyp[i];
    WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcId);

        // Add words to str
    if(i!=0)
      str=str+" ";
    
    for(unsigned int k=0;k<wgArc.words.size();++k)
    {
      str=str+wgArc.words[k];
      if(k!=wgArc.words.size()-1)
        str=str+" ";
    }

        // Sum score components
    if(wgArcId<scrCompsVec.size())
    {
      if(i==0)
        scoreComps=scrCompsVec[wgArcId];
      else
      {
        for(unsigned int k=0;k<scoreComps.size();++k)
          scoreComps[k]+=scrCompsVec[wgArcId][k];
      }
    }
  }
  return str;
}

//---------------------------------------
void WordGraph::obtainWgComposedOfUsefulStates(void)
{
  if(!empty())
  {
        // Obtain useful states
    std::vector<bool> stateIsUsefulVec;
    std::map<HypStateIndex,HypStateIndex> remappedStates;
    obtainUsefulStates(stateIsUsefulVec,remappedStates);

        // Save current arc information
    WordGraphArcs wordGraphArcsAux=wordGraphArcs;
    FinalStateSet finalStateSetAux=finalStateSet;
    std::vector<bool> arcsPrunedAux=arcsPruned;
    std::vector<std::vector<Score> > scrCompsVecAux=scrCompsVec;
    
        // Clear information subject to change
    wordGraphArcs.clear();
    arcsPruned.clear();
    wordGraphStates.clear();
    finalStateSet.clear();
    scrCompsVec.clear();

        // Regenerate final states
    FinalStateSet::iterator iter;
    for(iter=finalStateSetAux.begin();iter!=finalStateSetAux.end();++iter)
    {
      HypStateIndex idx=*iter;
      std::map<HypStateIndex,HypStateIndex>::iterator iter;
      iter=remappedStates.find(idx);
      if(iter!=remappedStates.end())
      {
        addFinalState(iter->second);
      }
    }
        // Regenerate arcs (states are regenerated implicitly)
    for(unsigned int arcid=0;arcid<wordGraphArcsAux.size();++arcid)
    {
          // Check if arc is not pruned
      if(!arcsPrunedAux[arcid])
      {
            // Obtain wordgraph arc information
        WordGraphArc wgArc=wordGraphArcsAux[arcid];
        std::vector<Score> scrVec=scrCompsVecAux[arcid];
        
            // Check if arc connects two useful states
        if(stateIsUsefulVec[wgArc.predStateIndex] && stateIsUsefulVec[wgArc.succStateIndex])
        {
              // Obtain remapped indices
          std::map<HypStateIndex,HypStateIndex>::iterator iter;

          iter=remappedStates.find(wgArc.predStateIndex);
          HypStateIndex newPredStateIndex=iter->second;
      
          iter=remappedStates.find(wgArc.succStateIndex);
          HypStateIndex newSuccStateIndex=iter->second;

              // Insert arc
          addArcWithScrComps(newPredStateIndex,
                             newSuccStateIndex,
                             wgArc.words,
                             wgArc.arcScore,
                             scrVec);
        }
      }
    }
  }
}

//---------------------------------------
void WordGraph::orderArcsTopol(void)
{
      // Define auxiliary variables
  WordGraphArcs wordGraphArcsAux;

  std::vector<bool> arcAdded;
  arcAdded.insert(arcAdded.begin(),wordGraphArcs.size(),false);
  
  std::vector<bool> stateClosed;
  stateClosed.insert(stateClosed.begin(),wordGraphStates.size(),false);
  
      // Repeat until all arcs has been reintroduced
  while(wordGraphArcsAux.size()<wordGraphArcs.size())
  {
    unsigned int atLeastOneArcAdded=false;

    std::pair<WordGraphArcId,WordGraphArcId> wgArcIdPair=getArcIndexRange();

    for(WordGraphArcId wgArcId=wgArcIdPair.first;wgArcId<=wgArcIdPair.second;++wgArcId)
    {
          // Check if arc has already been added
      if(!arcAdded[wgArcId])
      {
            // Obtain arc info
        WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcId);

            // Obtain predecessors of predecessor node
        std::vector<WordGraphArcId> wgArcIds;
        getArcIdsToPredStates(wgArc.predStateIndex,wgArcIds);

            // Check if all precessor states are closed
        bool allPredStatesClosed=true;
        for(unsigned int i=0;i<wgArcIds.size();++i)
        {
          WordGraphArc wgArcAux=wordGraphArcId2WordGraphArc(wgArcIds[i]);
          if(!stateClosed[wgArcAux.predStateIndex])
          {
            allPredStatesClosed=false;
            break;
          }
        }
            // Check if all precessor arcs have been added
        bool allPredArcsAdded=true;
        for(unsigned int i=0;i<wgArcIds.size();++i)
        {
          if(!arcAdded[wgArcIds[i]])
          {
            allPredArcsAdded=false;
            break;
          }
        }
            // If all predecessor states are closed, add arc and close
            // state
        if(allPredStatesClosed && allPredArcsAdded)
        {
              // Update atLeastOneArcAdded flag
          atLeastOneArcAdded=true;
              // Add arc
          wordGraphArcsAux.push_back(wgArc);
              // Mark arc as added
          arcAdded[wgArcId]=true;
              // Close state
          stateClosed[wgArc.predStateIndex]=true;
        }
      }
    }
        // Check atLeastOneArcAdded flag
    if(!atLeastOneArcAdded)
    {
          // Print error message
      std::cerr<<"Error while executing orderArcsTopol() function, anomalous word-graph"<<std::endl;
          // End while loop
      break;
    }
  }
      // Check if new arc ordering has been successfully obtained
  if(wordGraphArcsAux.size()==wordGraphArcs.size())
  {
        // Replace wordGraphArcs with wordGraphArcsAux
    wordGraphArcs=wordGraphArcsAux;
  }

}

//---------------------------------------
void WordGraph::calcPrevScores(HypStateIndex hypStateIndex,
                               const std::set<WordGraphArcId>& excludedArcs,
                               std::vector<Score>& prevScores,
                               std::vector<WordGraphArc>& bestPredArcForStateVec)const
{
      // Invoke calcPrevScoresWeights() using an empty weight vector
  std::vector<float> altCompWeights;
  
  calcPrevScoresWeights(hypStateIndex,
                        excludedArcs,
                        altCompWeights,
                        prevScores,
                        bestPredArcForStateVec);
}

//---------------------------------------
void WordGraph::calcPrevScoresWeights(HypStateIndex hypStateIndex,
                                     const std::set<WordGraphArcId>& excludedArcs,
                                     const std::vector<float>& altCompWeights,
                                     std::vector<Score>& prevScores,
                                     std::vector<WordGraphArc>& bestPredArcForStateVec)const
{
      // Check if word graph is empty
  if(empty())
  {
        // Word graph is empty
    prevScores.clear();
    bestPredArcForStateVec.clear();
  }
  else
  {
        // Word graph is not empty
        // Check if alternative weights can be applied
    bool altWeightsAppliable=checkIfAltWeightsAppliable(altCompWeights);
  
        // Show warning if alt. weights cannot be applied
    if(!altCompWeights.empty() && altWeightsAppliable==false)
      std::cerr<<"Warning: alternative weights cannot be applied!"<<std::endl;

        // Make room for vectors
    prevScores.clear();
    prevScores.insert(prevScores.begin(),wordGraphStates.size()-INITIAL_STATE,SMALL_SCORE);

    WordGraphArc wgArc;
    bestPredArcForStateVec.clear();
    bestPredArcForStateVec.insert(bestPredArcForStateVec.begin(),wordGraphStates.size()-INITIAL_STATE,wgArc);

        // Set previous score for the initial state
    if(hypStateIndex==INITIAL_STATE)
      prevScores[INITIAL_STATE]=initialStateScore;
    else
      prevScores[hypStateIndex]=0;

        // Initialize boolean vector of accessible states
    std::vector<bool> accessibleStateVec;
    accessibleStateVec.insert(accessibleStateVec.begin(),wordGraphStates.size()-INITIAL_STATE,false);
    accessibleStateVec[hypStateIndex]=true;
  
        // Iteration over the arcs (arcs are assumed to be topologically
        // ordered)
    std::pair<WordGraphArcId,WordGraphArcId> wgArcIdPair=getArcIndexRange();

    for(WordGraphArcId wgArcId=wgArcIdPair.first;wgArcId<=wgArcIdPair.second;++wgArcId)
    {
          // Check if arc has not been pruned
      if(!arcPruned(wgArcId))
      {
        WordGraphArc wordGraphArc=wordGraphArcId2WordGraphArc(wgArcId);

            // Check if wordGraphArc.predStateIndex is accessible
        if(accessibleStateVec[wordGraphArc.predStateIndex])
        {
              // Determine score of arc
          Score arcScore=0;
          if(altWeightsAppliable)
          {
            for(unsigned int i=0;i<altCompWeights.size();++i)
              arcScore+=altCompWeights[i]*scrCompsVec[wgArcId][i];
          }
          else
            arcScore=wordGraphArc.arcScore;
        
              // Update score
          Score score=arcScore+prevScores[wordGraphArc.predStateIndex];
          if(!excludedArcs.empty())
          {
                // If predecessor state is the initial state, check that the
                // first word is not contained in the set of rejected words
            std::set<WordGraphArcId>::const_iterator wgArcIdSetIter;
            wgArcIdSetIter=excludedArcs.find(wgArcId);
            if(wgArcIdSetIter!=excludedArcs.end())
              score=SMALL_SCORE;
          }
        
          if(score<SMALL_SCORE) score=SMALL_SCORE;
          if(score>prevScores[wordGraphArc.succStateIndex])
          {
            prevScores[wordGraphArc.succStateIndex]=score;
            bestPredArcForStateVec[wordGraphArc.succStateIndex]=wordGraphArc;
          }
              // Update accessibleStateVec vector
          accessibleStateVec[wordGraphArc.succStateIndex]=true;
        }
        else
        {
              // If wordGraphArc.succStateIndex is not accessible, assign
              // SMALL_SCORE to it
          if(!accessibleStateVec[wordGraphArc.succStateIndex])
            prevScores[wordGraphArc.succStateIndex]=SMALL_SCORE;
        }
      }
    }
  }
}

//---------------------------------------
bool WordGraph::checkIfAltWeightsAppliable(const std::vector<float>& altCompWeights)const
{
      // Check if alternative weights can be applied
  bool altWeightsAppliable=true;
  if(altCompWeights.empty())
  {
        // No alt. weights were given
    altWeightsAppliable=false;
  }
  else
  {
    if(altCompWeights.size()!=compWeights.size())
      altWeightsAppliable=false;

    for(unsigned int i=0;i<scrCompsVec.size();++i)
      if(altCompWeights.size()!=scrCompsVec[i].size())
      {
        altWeightsAppliable=false;
        break;
      }
  }
      // Return result
  return altWeightsAppliable;
}
//---------------------------------------
void WordGraph::calcRestScores(std::vector<Score>& restScores)const
{
      // Make room for vector
  restScores.clear();
  restScores.insert(restScores.begin(),wordGraphStates.size(),SMALL_SCORE);
  
      // Update rest scores for final states
  FinalStateSet::const_iterator finalStateSetIter;  
  for(finalStateSetIter=finalStateSet.begin();finalStateSetIter!=finalStateSet.end();++finalStateSetIter)
  {
    restScores[*finalStateSetIter]=initialStateScore;
  }
  
      // Reverse iteration over the arcs (arcs are assumed to be
      // topologically ordered)
  for(unsigned int i=0;i<wordGraphArcs.size();++i)
  {
    unsigned int r=wordGraphArcs.size()-i-1;
        // Check if arc has not been pruned
    if(!arcPruned(r))
    {
      WordGraphArc wordGraphArc=wordGraphArcs[r];
    
      Score score=wordGraphArc.arcScore+restScores[wordGraphArc.succStateIndex];
      if(score<SMALL_SCORE) score=SMALL_SCORE;
      if(score>restScores[wordGraphArc.predStateIndex])
        restScores[wordGraphArc.predStateIndex]=score;
    }
  }  
}

//---------------------------------------
Score WordGraph::bestPathFromFinalStateToIdx(HypStateIndex hypStateIndex,
                                             const std::set<WordGraphArcId>& excludedArcs,
                                             std::vector<WordGraphArc>& arcVec)const
{
  std::vector<float> altCompWeights;

  return bestPathFromFinalStateToIdxWeights(hypStateIndex,
                                            excludedArcs,
                                            altCompWeights,
                                            arcVec);
}

//---------------------------------------
Score WordGraph::bestPathFromFinalStateToIdxWeights(HypStateIndex hypStateIndex,
                                                    const std::set<WordGraphArcId>& excludedArcs,
                                                    const std::vector<float>& altCompWeights,
                                                    std::vector<WordGraphArc>& arcVec)const
{
      // Obtain previous scores
  std::vector<Score> prevScores;
  std::vector<WordGraphArc> bestPredArcForStateVec;

  calcPrevScoresWeights(hypStateIndex,
                        excludedArcs,
                        altCompWeights,
                        prevScores,
                        bestPredArcForStateVec);

      // Invoke auxiliary function
  return bestPathFromFinalStateToIdxAux(hypStateIndex,
                                        prevScores,
                                        bestPredArcForStateVec,
                                        arcVec);  
}

//---------------------------------------
Score WordGraph::bestPathFromFinalStateToIdxAux(HypStateIndex hypStateIndex,
                                                const std::vector<Score>& prevScores,
                                                const std::vector<WordGraphArc>& bestPredArcForStateVec,
                                                std::vector<WordGraphArc>& arcVec)const
{  
      // Initialize variables
  Score bestFinalStateScore=SMALL_SCORE;
  HypStateIndex bestFinalState=INITIAL_STATE;

      // Obtain best final state
  for(FinalStateSet::iterator fssIter=finalStateSet.begin();fssIter!=finalStateSet.end();++fssIter)
  {
    if(bestFinalStateScore<prevScores[*fssIter])
    {
      bestFinalState=*fssIter;
      bestFinalStateScore=prevScores[*fssIter];
    }
  }
  arcVec.clear();

      // Check if hypStateIndex can lead to a final state
  if(stateIsFinal(bestFinalState))
  {
        // hypStateIndex can lead to a final state
    bool end=false;
    HypStateIndex idx=bestFinalState;
    while(!end)
    {
      if(idx==hypStateIndex)
      {
            // Starting node reached
        end=true;
      }
      else
      {
            // Starting node not yet reached
        arcVec.push_back(bestPredArcForStateVec[idx]);      
        idx=bestPredArcForStateVec[idx].predStateIndex;
      }
    }
  }
  return bestFinalStateScore;
}

//---------------------------------------
unsigned int WordGraph::pruneArcsToPredStates(float threshold)
{
      // Obtain logarithm of threshold
  float logThreshold;
  if(threshold==0)
    logThreshold=SMALL_SCORE;
  else logThreshold=log(threshold);

      // Calculate rest scores
  std::vector<Score> restScores;
  calcRestScores(restScores);
  
      // Calculate previous scores
  std::set<WordGraphArcId> emptyWgArcIdSet;
  std::vector<Score> prevScores;
  std::vector<WordGraphArc> bestPredArcForStateVec;
  calcPrevScores(INITIAL_STATE,
                 emptyWgArcIdSet,
                 prevScores,
                 bestPredArcForStateVec);

        // Obtain best hypothesis score
  Score bestHypScore=SMALL_SCORE;
  FinalStateSet::iterator iter;
  for(iter=finalStateSet.begin();iter!=finalStateSet.end();++iter)
  {
    if(bestHypScore<prevScores[*iter])
      bestHypScore=prevScores[*iter];
  }

      // Declare and initialize numPrunedArcs variables
  unsigned int numPrunedArcs=0;
  
      // Explore nodes
  for(HypStateIndex hidx=0;hidx<wordGraphStates.size();++hidx)  
  {
    std::vector<std::pair<Score,WordGraphArcId> > scoreArcIdVec;
    
        // Iterate over the arcs to predecessors
    std::vector<WordGraphArcId>::iterator arcIdVecIter;
    for(arcIdVecIter=wordGraphStates[hidx].arcsToPredStates.begin();arcIdVecIter!=wordGraphStates[hidx].arcsToPredStates.end();++arcIdVecIter)
    {
          // Extract relevant arc information
      WordGraphArcId wordGraphArcId=*arcIdVecIter;
      HypStateIndex predStateIndex=wordGraphArcs[wordGraphArcId].predStateIndex;
      HypStateIndex succStateIndex=wordGraphArcs[wordGraphArcId].succStateIndex;
      Score arcScore=wordGraphArcs[wordGraphArcId].arcScore;
      Score bestScoreAssociatedToArc=prevScores[predStateIndex]+arcScore+restScores[succStateIndex];
      // std::cerr<<predStateIndex<<" -> "<<succStateIndex<<" , "<<prevScores[predStateIndex]<<" ";
      // std::cerr<<arcScore<<" "<<restScores[succStateIndex]<<" , "<<bestScoreAssociatedToArc<<" , "<<bestHypScore<<" "<<bestHypScore+logThreshold<<std::endl;

          // Decide if the arc has to be pruned
      if(bestScoreAssociatedToArc<bestHypScore+logThreshold)
      {
        arcsPruned[wordGraphArcId]=true;
        ++numPrunedArcs;
      }
      else
      {
        arcsPruned[wordGraphArcId]=false;        
      }      
    }
  }  
  return numPrunedArcs;
}

//---------------------------------------
bool WordGraph::finalStatePruned(HypStateIndex hypStateIndex)const
{
      // Obtain arcs to predecessors for final state
  std::vector<WordGraphArcId> wgArcIds;
  getArcIdsToPredStates(hypStateIndex,wgArcIds);

      // Verify if there is at least one arc that has not been pruned
  bool finalStatePrunedBool=true;
  for(unsigned int i=0;i<wgArcIds.size();++i)
  {
    if(!arcPruned(wgArcIds[i]))
    {
      finalStatePrunedBool=false;
      break;
    }
  }
      // Return result
  return finalStatePrunedBool;
}

//---------------------------------------
void WordGraph::obtainStatesReachableFromInit(std::vector<bool>& stateReachableFromInitVec)const
{
      // Initialize stateReachableFromInitVec variable
  stateReachableFromInitVec.clear();
  for(unsigned int i=0;i<wordGraphStates.size();++i)
    stateReachableFromInitVec.push_back(false);
  if(INITIAL_STATE<stateReachableFromInitVec.size())
    stateReachableFromInitVec[INITIAL_STATE]=true;
      // Direct iteration over the arcs (arcs are assumed to be
      // topologically ordered)
  for(WordGraphArcId wgArcId=0;wgArcId<wordGraphArcs.size();++wgArcId)
  {
    if(!arcPruned(wgArcId))
    {
      WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcId);
      if(stateReachableFromInitVec[wgArc.predStateIndex])
        stateReachableFromInitVec[wgArc.succStateIndex]=true;
    }
  }
}

//---------------------------------------
void WordGraph::obtainUsefulStates(std::vector<bool>& stateIsUsefulVec,
                                   std::map<HypStateIndex,HypStateIndex>& remappedStates)const
{  
      // Initialize vector
  stateIsUsefulVec.clear();
  for(unsigned int i=0;i<wordGraphStates.size();++i)
    stateIsUsefulVec.push_back(false);

      // Set vector values for final states
  FinalStateSet::const_iterator fssIter;
  for(fssIter=finalStateSet.begin();fssIter!=finalStateSet.end();++fssIter)
  {
    bool atLeastOneArcArrivesToState=false;
    
        // Obtain arcs to final state
    std::vector<WordGraphArcId> wgArcIds;
    getArcIdsToPredStates(*fssIter,wgArcIds);

        // Check if there is at least one not pruned arc
    for(unsigned int j=0;j<wgArcIds.size();++j)
    {
      if(!arcPruned(wgArcIds[j]))
      {
        atLeastOneArcArrivesToState=true;
        break;
      }
    }
        // If at least one arc arrives to the final state, mark it as
        // a useful one
    if(atLeastOneArcArrivesToState)
      stateIsUsefulVec[*fssIter]=true;
  }

      // Obtain boolean vector of states reachable from initial node
  std::vector<bool> stateReachableFromInitVec;
  obtainStatesReachableFromInit(stateReachableFromInitVec);
  
      // Reverse iteration over the arcs (arcs are assumed to be
      // topologically ordered)
  for(WordGraphArcId wgArcId=0;wgArcId<wordGraphArcs.size();++wgArcId)
  {
    WordGraphArcId reverseWgArcId=wordGraphArcs.size()-wgArcId-1;
    if(!arcPruned(reverseWgArcId))
    {
      WordGraphArc wgArc=wordGraphArcId2WordGraphArc(reverseWgArcId);
      if(stateReachableFromInitVec[wgArc.predStateIndex] && stateIsUsefulVec[wgArc.succStateIndex])
        stateIsUsefulVec[wgArc.predStateIndex]=true;
    }
  }

      // Initialize remappedStates
  remappedStates.clear();

      // Build remappedStates
  HypStateIndex newIdx=INITIAL_STATE;
  std::pair<HypStateIndex,HypStateIndex> range=getHypStateIndexRange();
  if(range.first!=INVALID_STATE && range.second!=INVALID_STATE)
  {
    for(HypStateIndex idx=range.first;idx<=range.second;++idx)
    {
      if(idx<stateIsUsefulVec.size() && stateIsUsefulVec[idx])
      {
        remappedStates[idx]=newIdx;
        ++newIdx;
      }
    }
  }
}

//---------------------------------------
bool WordGraph::load(const char * filename)
{
  AwkInputStream awk;
  
  if(awk.open(filename)==THOT_ERROR)
  {
    std::cerr<<"Error while opening word graph file: "<<filename<<"\n";
    return THOT_ERROR;
  }
  else
  {
    std::cerr<<"Reading word graph from file: "<<filename<<"\n";
    
        // Clear word graph
    clear();
    
        // File with word graph successfully opened
    awk.getln();
    
        // Check if first line has component weights
    if(awk.dollar(1)=="#")
    {
          // Read weights
      std::vector<std::pair<std::string,float> > _compWeights;
      for(unsigned int i=2;i<=awk.NF;i+=3)
      {
        std::pair<std::string,float> compWeight;
        compWeight.first=awk.dollar(i);
        compWeight.second=atof(awk.dollar(i+1).c_str());  
        _compWeights.push_back(compWeight);
      }
      compWeights=_compWeights;
          // setCompWeights(_compWeights);
      
          // Read next line
      awk.getln();
    }

        // Read final states
    for(unsigned int i=1;i<=awk.NF;++i)
    {
      HypStateIndex finalState;
      finalState=atoi(awk.dollar(i).c_str());
      finalStateSet.insert(finalState);
    }

        // Read file entries
    while(awk.getln())
    {
      if(awk.NF>=3)
      {
            // Read state indices
        HypStateIndex predStateIndex=atoi(awk.dollar(1).c_str());
        HypStateIndex succStateIndex=atoi(awk.dollar(2).c_str());

            // Read arcScore
        Score arcScore=atof(awk.dollar(3).c_str());

            // Read score components if given
        std::vector<Score> scrVec;
        unsigned int col=4;
        if(awk.dollar(4)=="|||")
        {
          col=5;
          while(awk.dollar(col)!="|||" && col<=awk.NF)
          {
            scrVec.push_back(atof(awk.dollar(col).c_str()));
            ++col;
          }
          ++col;
        }
        
            // Read words
        std::vector<std::string> words;
            // The vector "words" may be empty if the entry only has 3
            // columns
        for(;col<=awk.NF;++col)
        {
          words.push_back(awk.dollar(col));
        }
        addArcWithScrComps(predStateIndex,
                           succStateIndex,
                           words,
                           arcScore,
                           scrVec);
      }
    }
    awk.close();
    return THOT_OK;
  }
}

//---------------------------------------
bool WordGraph::print(const char* filename,
                      bool printOnlyUsefulStates/*=false*/)const
{
  std::ofstream outS;

  outS.open(filename,std::ios::trunc);
  if(!outS)
  {
    std::cerr<<"Error while printing recombination graph to file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
    print(outS,printOnlyUsefulStates);
    outS.close();	
    return THOT_OK;
  }
}

//---------------------------------------
void WordGraph::print(std::ostream &outS,
                      bool printOnlyUsefulStates/*=false*/)const
{
  FinalStateSet::const_iterator finalStateSetIter;

      // Print component weights if they were given
  if(!compWeights.empty())
  {
    outS<<"# ";
    for(unsigned int i=0;i<compWeights.size();++i)
    {
      outS<<compWeights[i].first<<" "<<compWeights[i].second;
      if(i!=compWeights.size()-1) outS<<" , ";
    }
    outS<<std::endl;
  }
  
      // Print final states  
  for(finalStateSetIter=finalStateSet.begin();finalStateSetIter!=finalStateSet.end();++finalStateSetIter)
  {
    if(!finalStatePruned(*finalStateSetIter))
    {
      outS<<*finalStateSetIter<<" ";
    }
  }
  outS<<std::endl;

      // Obtain useful states (may be misplaced)
  std::vector<bool> stateIsUsefulVec;
  std::map<HypStateIndex,HypStateIndex> remappedStates;
  if(printOnlyUsefulStates)
    obtainUsefulStates(stateIsUsefulVec,remappedStates);
  
      // Print arcs
  for(unsigned int i=0;i<wordGraphArcs.size();++i)
  {
        // Print arc if it is useful and has not been pruned
    bool arcIsUseful=false;
    if(printOnlyUsefulStates)
      arcIsUseful=stateIsUsefulVec[wordGraphArcs[i].predStateIndex] && stateIsUsefulVec[wordGraphArcs[i].succStateIndex];
    
    if( (!printOnlyUsefulStates || arcIsUseful) && !arcsPruned[i])
    {
      WordGraphArc wordGraphArc=wordGraphArcs[i];

          //Print indices
      outS<<wordGraphArc.predStateIndex<<" "<<wordGraphArc.succStateIndex<<" "<<wordGraphArc.arcScore<<" ";

      if(!scrCompsVec[i].empty())
      {
            // Print components if the corresponding vector is not empty
        outS<<"||| ";
        for(unsigned int j=0;j<scrCompsVec[i].size();++j)
        {
          outS<<scrCompsVec[i][j]<<" ";
        }
        outS<<"||| ";
      }
      
      for(unsigned int i=0;i<wordGraphArc.words.size();++i)
      {
        outS<<wordGraphArc.words[i];
        if(i<wordGraphArc.words.size()-1) outS<<" ";
      }
      outS<<std::endl;
    }
  }
}

//---------------------------------------
bool WordGraph::empty(void)const
{
  return wordGraphArcs.empty();
}

//---------------------------------------
size_t WordGraph::numArcs(void)const
{
  return wordGraphArcs.size();  
}

//---------------------------------------
size_t WordGraph::numStates(void)const
{
  return wordGraphStates.size();  
}

//---------------------------------------
void WordGraph::clear(void)
{
  wordGraphArcs.clear();
  arcsPruned.clear();
  wordGraphStates.clear();
  finalStateSet.clear();
  initialStateScore=0;
  scrCompsVec.clear();
  compWeights.clear();
}
