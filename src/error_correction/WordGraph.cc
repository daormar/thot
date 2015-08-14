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
/* Module: WordGraph                                                */
/*                                                                  */
/* Definitions file: WordGraph.cc                                   */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "WordGraph.h"

//--------------- WordGraph class function definitions

WordGraph::WordGraph(void)
{
  initialStateScore=0;
}

//---------------------------------------
void WordGraph::setCompWeights(const Vector<pair<std::string,float> >& _compWeights)
{
      // Set new component weight vector
  compWeights=_compWeights;

      // Re-score arcs
  rescoreArcsGivenWeights(compWeights);
}

//---------------------------------------
void WordGraph::rescoreArcsGivenWeights(const Vector<pair<std::string,float> >& _compWeights)
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
void WordGraph::getCompWeights(Vector<pair<std::string,float> >& _compWeights)const
{
  _compWeights=compWeights;
}

//---------------------------------------
void WordGraph::addArc(HypStateIndex predStateIndex,
                       HypStateIndex succStateIndex,
                       const Vector<std::string>& words,
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
  Vector<Score> emptyScrVec;
  scrCompsVec.push_back(emptyScrVec);
}

//---------------------------------------
void WordGraph::addArcWithScrComps(HypStateIndex predStateIndex,
                                   HypStateIndex succStateIndex,
                                   const Vector<std::string>& words,
                                   Score arcScore,
                                   Vector<Score> scrVec)
{
      // Add arc
  addArc(predStateIndex,succStateIndex,words,arcScore);

      // Store components
  Vector<Score> emptyScrVec;
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
pair<HypStateIndex,HypStateIndex> WordGraph::getHypStateIndexRange(void)const
{
  if(wordGraphStates.empty())
    return make_pair(INVALID_STATE,INVALID_STATE);
  else
    return make_pair(INITIAL_STATE,wordGraphStates.size()-1);
}

//---------------------------------------
pair<WordGraphArcId,WordGraphArcId> WordGraph::getArcIndexRange(void)const
{
  if(wordGraphArcs.empty())
    return make_pair(INVALID_ARCID,INVALID_ARCID);  
  else
    return make_pair(0,wordGraphArcs.size()-1);  
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

    return wordGraphArc;
  }
}

//---------------------------------------
void WordGraph::getArcsToPredStates(HypStateIndex hypStateIndex,
                                    Vector<WordGraphArc>& wgArcs)const
{
  Vector<WordGraphArcId> wgArcIds;

  getArcIdsToPredStates(hypStateIndex,wgArcIds);

  wgArcs.clear();
  for(unsigned int i=0;i<wgArcIds.size();++i)
  {
    wgArcs.push_back(wordGraphArcId2WordGraphArc(wgArcIds[i]));
  }  
}

//---------------------------------------
void WordGraph::getArcIdsToPredStates(HypStateIndex hypStateIndex,
                                      Vector<WordGraphArcId>& wgArcIds)const
{
  if(hypStateIndex<wordGraphStates.size())
  {
    Vector<WordGraphArcId>::const_iterator arcIdVecIter;

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
                                    Vector<WordGraphArc>& wgArcs)const
{
  Vector<WordGraphArcId> wgArcIds;

  getArcIdsToSuccStates(hypStateIndex,wgArcIds);

  wgArcs.clear();
  for(unsigned int i=0;i<wgArcIds.size();++i)
  {
    wgArcs.push_back(wordGraphArcId2WordGraphArc(wgArcIds[i]));
  }  
}

//---------------------------------------
void WordGraph::getArcIdsToSuccStates(HypStateIndex hypStateIndex,
                                      Vector<WordGraphArcId>& wgArcIds)const
{
  if(hypStateIndex<wordGraphStates.size())
  {
    Vector<WordGraphArcId>::const_iterator arcIdVecIter;

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
                                Vector<pair<Score,std::string> >& nblist,
                                Vector<Vector<Score> >& scoreCompsVec)
{
      // Check if word-graph is empty
  if(wordGraphArcs.empty())
  {
        // clear nblist and scoreCompsVec output variables
    nblist.clear();
    scoreCompsVec.clear();
  }
  else
  {
        // Word-graph is not empty
        // Obtain heuristic values
    Vector<Score> heurForEachState;
    obtainNbSearchHeurInfo(heurForEachState);
  
        // Execute A-star search
    nbSearch(len,heurForEachState,nblist,scoreCompsVec);
  }
}

//---------------------------------------
void WordGraph::obtainNbSearchHeurInfo(Vector<Score>& heurForEachState)
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
                         const Vector<Score>& heurForEachState,
                         Vector<pair<Score,std::string> >& nblist,
                         Vector<Vector<Score> >& scoreCompsVec)
{
  // Perform A-star search

      // Create null hypothesis
  NbSearchHyp nbSearchHyp;
  nbSearchHyp.push_back(INITIAL_STATE);

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
        pair<Score,NbSearchHyp> scrHypPair=nbSearchStack.top();
        nbSearchStack.pop();

            // Subtract heuristic
        scrHypPair.first-=heurForEachState[scrHypPair.second.back()];        
        
            // Check if hyp is complete
        if(hypIsComplete(scrHypPair.second))
        {
          completeHypStack.push(scrHypPair.first,scrHypPair.second);
        }
        else
        {
            // Expand hypothesis
          HypStateIndex hidx=scrHypPair.second.back();
          Vector<WordGraphArcId> wgArcIds;
          getArcIdsToSuccStates(hidx,wgArcIds);

          Vector<pair<Score,NbSearchHyp> > scrHypPairVec;
          for(unsigned int i=0;i<wgArcIds.size();++i)
          {
            if(!arcPruned(wgArcIds[i]))
            {
              WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcIds[i]);
              pair<Score,NbSearchHyp> newScrHypPair;
              newScrHypPair=scrHypPair;
                  // Obtain new score
              newScrHypPair.first+=wgArc.arcScore;
                  // Add heuristic
              newScrHypPair.first+=heurForEachState[wgArc.succStateIndex];   
                  // Add new state
              newScrHypPair.second.push_back(wgArc.succStateIndex);
                  // Push into vector
              scrHypPairVec.push_back(newScrHypPair);
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
  }
      // Print number of iterations
  cerr<<"Number of iterations required to obtain n-best list: "<<numIters<<endl;
          
      // Obtain result
  nblist.clear();
  scoreCompsVec.clear();
  while(!completeHypStack.empty())
  {
        // Pop top of the stack
    pair<Score,NbSearchHyp> scrHypPair=completeHypStack.top();
    completeHypStack.pop();

        // Obtain string from hyp
    Vector<Score> scoreComps;
    std::string translation=stringAssociatedToHyp(scrHypPair.second,scoreComps);
    if(!scoreComps.empty()) scoreCompsVec.push_back(scoreComps);
      
        // Add to vector
    nblist.push_back(make_pair(scrHypPair.first,translation));
  }
}

//---------------------------------------
bool WordGraph::hypIsComplete(const NbSearchHyp& nbSearchHyp)
{
  if(nbSearchHyp.empty()) return false;
  else
  {
    HypStateIndex hidx=nbSearchHyp.back();
    if(stateIsFinal(hidx))
      return true;
    else
      return false;
  }
}

//---------------------------------------
std::string WordGraph::stringAssociatedToHyp(const NbSearchHyp& nbSearchHyp,
                                             Vector<Score>& scoreComps)
{
  std::string str;
  for(unsigned int i=0;i<nbSearchHyp.size();++i)
  {
        // Obtain predecessor state
    HypStateIndex predIdx=nbSearchHyp[i];

        // Check if there is a successor
    if(i+1<nbSearchHyp.size())
    {
          // Obtain successor state
      HypStateIndex succIdx=nbSearchHyp[i+1];

          // Obtain arcs from predIdx
      Vector<WordGraphArcId> wgArcIds;
      getArcIdsToSuccStates(predIdx,wgArcIds);

          // Find arc from predIdx to succIdx
      for(unsigned int j=0;j<wgArcIds.size();++j)
      {
        WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcIds[j]);
        if(succIdx==wgArc.succStateIndex)
        {
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
          if(wgArcIds[j]<scrCompsVec.size())
          {
            if(i==0)
              scoreComps=scrCompsVec[wgArcIds[j]];
            else
            {
              for(unsigned int k=0;k<scoreComps.size();++k)
                scoreComps[k]+=scrCompsVec[wgArcIds[j]][k];
            }
          }
              // Break for loop (arc has already been found)
          break;
        }
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
    Vector<bool> stateIsUsefulVec;
    std::map<HypStateIndex,HypStateIndex> remappedStates;
    obtainUsefulStates(stateIsUsefulVec,remappedStates);

        // Save current arc information
    WordGraphArcs wordGraphArcsAux=wordGraphArcs;
    FinalStateSet finalStateSetAux=finalStateSet;
    Vector<bool> arcsPrunedAux=arcsPruned;
    Vector<Vector<Score> > scrCompsVecAux=scrCompsVec;
    
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
        Vector<Score> scrVec=scrCompsVecAux[arcid];
        
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
void WordGraph::order_arcs_topol(void)
{
      // Define auxiliary variables
  WordGraphArcs wordGraphArcsAux;

  Vector<bool> arcAdded;
  arcAdded.insert(arcAdded.begin(),wordGraphArcs.size(),false);
  
  Vector<bool> stateClosed;
  stateClosed.insert(stateClosed.begin(),wordGraphStates.size(),false);
  
      // Repeat until all arcs has been reintroduced
  while(wordGraphArcsAux.size()<wordGraphArcs.size())
  {
    unsigned int atLeastOneArcAdded=false;

    pair<WordGraphArcId,WordGraphArcId> wgArcIdPair=getArcIndexRange();

    for(WordGraphArcId wgArcId=wgArcIdPair.first;wgArcId<=wgArcIdPair.second;++wgArcId)
    {
          // Check if arc has already been added
      if(!arcAdded[wgArcId])
      {
            // Obtain arc info
        WordGraphArc wgArc=wordGraphArcId2WordGraphArc(wgArcId);

            // Obtain predecessors of predecessor node
        Vector<WordGraphArcId> wgArcIds;
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
      cerr<<"Error while executing order_arcs_topol() function, anomalous word-graph"<<endl;
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
                               Vector<Score>& prevScores,
                               Vector<WordGraphArc>& bestPredArcForStateVec)const
{
      // Invoke calcPrevScoresWeights() using an empty weight vector
  Vector<float> altCompWeights;
  
  calcPrevScoresWeights(hypStateIndex,
                        excludedArcs,
                        altCompWeights,
                        prevScores,
                        bestPredArcForStateVec);
}

//---------------------------------------
void WordGraph::calcPrevScoresWeights(HypStateIndex hypStateIndex,
                                     const std::set<WordGraphArcId>& excludedArcs,
                                     const Vector<float>& altCompWeights,
                                     Vector<Score>& prevScores,
                                     Vector<WordGraphArc>& bestPredArcForStateVec)const
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
      cerr<<"Warning: alternative weights cannot be applied!"<<endl;

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
    Vector<bool> accessibleStateVec;
    accessibleStateVec.insert(accessibleStateVec.begin(),wordGraphStates.size()-INITIAL_STATE,false);
    accessibleStateVec[hypStateIndex]=true;
  
        // Iteration over the arcs (arcs are assumed to be topologically
        // ordered)
    pair<WordGraphArcId,WordGraphArcId> wgArcIdPair=getArcIndexRange();

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
bool WordGraph::checkIfAltWeightsAppliable(const Vector<float>& altCompWeights)const
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
void WordGraph::calcRestScores(Vector<Score>& restScores)const
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
                                             Vector<WordGraphArc>& arcVec)const
{
  Vector<float> altCompWeights;

  return bestPathFromFinalStateToIdxWeights(hypStateIndex,
                                            excludedArcs,
                                            altCompWeights,
                                            arcVec);
}

//---------------------------------------
Score WordGraph::bestPathFromFinalStateToIdxWeights(HypStateIndex hypStateIndex,
                                                    const std::set<WordGraphArcId>& excludedArcs,
                                                    const Vector<float>& altCompWeights,
                                                    Vector<WordGraphArc>& arcVec)const
{
      // Obtain previous scores
  Vector<Score> prevScores;
  Vector<WordGraphArc> bestPredArcForStateVec;

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
                                                const Vector<Score>& prevScores,
                                                const Vector<WordGraphArc>& bestPredArcForStateVec,
                                                Vector<WordGraphArc>& arcVec)const
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
  Vector<Score> restScores;
  calcRestScores(restScores);
  
      // Calculate previous scores
  std::set<WordGraphArcId> emptyWgArcIdSet;
  Vector<Score> prevScores;
  Vector<WordGraphArc> bestPredArcForStateVec;
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
    Vector<pair<Score,WordGraphArcId> > scoreArcIdVec;
    
        // Iterate over the arcs to predecessors
    Vector<WordGraphArcId>::iterator arcIdVecIter;
    for(arcIdVecIter=wordGraphStates[hidx].arcsToPredStates.begin();arcIdVecIter!=wordGraphStates[hidx].arcsToPredStates.end();++arcIdVecIter)
    {
          // Extract relevant arc information
      WordGraphArcId wordGraphArcId=*arcIdVecIter;
      HypStateIndex predStateIndex=wordGraphArcs[wordGraphArcId].predStateIndex;
      HypStateIndex succStateIndex=wordGraphArcs[wordGraphArcId].succStateIndex;
      Score arcScore=wordGraphArcs[wordGraphArcId].arcScore;
      Score bestScoreAssociatedToArc=prevScores[predStateIndex]+arcScore+restScores[succStateIndex];
      // cerr<<predStateIndex<<" -> "<<succStateIndex<<" , "<<prevScores[predStateIndex]<<" ";
      // cerr<<arcScore<<" "<<restScores[succStateIndex]<<" , "<<bestScoreAssociatedToArc<<" , "<<bestHypScore<<" "<<bestHypScore+logThreshold<<endl;

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
  Vector<WordGraphArcId> wgArcIds;
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
void WordGraph::obtainStatesReachableFromInit(Vector<bool>& stateReachableFromInitVec)const
{
      // Initialize stateReachableFromInitVec variable
  stateReachableFromInitVec.clear();
  for(unsigned int i=0;i<wordGraphStates.size();++i)
    stateReachableFromInitVec.push_back(false);
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
void WordGraph::obtainUsefulStates(Vector<bool>& stateIsUsefulVec,
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
    Vector<WordGraphArcId> wgArcIds;
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
  Vector<bool> stateReachableFromInitVec;
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
  pair<HypStateIndex,HypStateIndex> range=getHypStateIndexRange();
  for(HypStateIndex idx=range.first;idx<=range.second;++idx)
  {
    if(stateIsUsefulVec[idx])
    {
      remappedStates[idx]=newIdx;
      ++newIdx;
    }
  }
}

//---------------------------------------
bool WordGraph::load(const char * filename)
{
  awkInputStream awk;
  
  if(awk.open(filename)==ERROR)
  {
    cerr<<"Error while opening word graph file: "<<filename<<"\n";
    return ERROR;
  }
  else
  {
    cerr<<"Reading word graph from file: "<<filename<<"\n";
    
        // Clear word graph
    clear();
    
        // File with word graph successfully opened
    awk.getln();
    
        // Check if first line has component weights
    if(awk.dollar(1)=="#")
    {
          // Read weights
      Vector<pair<std::string,float> > _compWeights;
      for(unsigned int i=2;i<=awk.NF;i+=3)
      {
        pair<std::string,float> compWeight;
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
        Vector<Score> scrVec;
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
        Vector<std::string> words;
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
    return OK;
  }
}

//---------------------------------------
bool WordGraph::print(const char* filename,
                      bool printOnlyUsefulStates/*=false*/)const
{
  ofstream outS;

  outS.open(filename,ios::trunc);
  if(!outS)
  {
    cerr<<"Error while printing recombination graph to file."<<endl;
    return ERROR;
  }
  else
  {
    print(outS,printOnlyUsefulStates);
    outS.close();	
    return OK;
  }
}

//---------------------------------------
void WordGraph::print(ostream &outS,
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
    outS<<endl;
  }
  
      // Print final states  
  for(finalStateSetIter=finalStateSet.begin();finalStateSetIter!=finalStateSet.end();++finalStateSetIter)
  {
    if(!finalStatePruned(*finalStateSetIter))
    {
      outS<<*finalStateSetIter<<" ";
    }
  }
  outS<<endl;

      // Obtain useful states (may be misplaced)
  Vector<bool> stateIsUsefulVec;
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
      // // debug
      // cerr<<"*** "<<wordGraphArc.predStateIndex<<" "<<wordGraphArc.succStateIndex<<" "<<wordGraphArc.arcScore<<" "<<arcsPruned[i]<<endl;
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
      outS<<endl;
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

//---------------------------------------
void WordGraph::clearTempVars(void)
{
}
