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
 

#include "WeightUpdateUtils.h"

namespace WeightUpdateUtils
{
      // Non-public function declarations

  int linInterpWeightsDhsEval(const Vector<Vector<PhrasePair> >& invPhrPairs,
                              DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPhrModelFeatPtr,
                              InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPhrModelFeatPtr,
                              FILE* tmp_file,
                              double* x,
                              double& obj_func);
  double phraseModelPerplexity(const Vector<Vector<PhrasePair> >& invPhrPairs,
                               DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPhrModelFeatPtr,
                               InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPhrModelFeatPtr,
                               int verbose=0);

      // Function definitions

  //---------------------------------
  void updateLogLinearWeights(std::string refSent,
                              WordGraph* wgPtr,
                              BaseLogLinWeightUpdater* llWeightUpdaterPtr,
                              const Vector<pair<std::string,float> >& compWeights,
                              Vector<float>& newWeights,
                              int verbose/*=0*/)
  {
        // Obtain n-best list
    unsigned int len=NBLIST_SIZE_FOR_LLWEIGHT_UPDATE;
    Vector<pair<Score,std::string> > nblist;
    Vector<Vector<double> > scoreCompsVec;
    wgPtr->obtainNbestList(len,nblist,scoreCompsVec);

        // Obtain current weights
    vector<double> currentWeights;
    for(unsigned int i=0;i<compWeights.size();++i)
      currentWeights.push_back(compWeights[i].second);
  
        // Print verbose information
    if(verbose)
    {
      cerr<<"Training log linear combination weights (n-best list size= "<<nblist.size()<<")..."<<endl;
    }
  
        // Obtain new weights
    newWeights.clear();
    
        // Check if n-best list is empty 
    if(nblist.empty())
    {
      newWeights.clear();
      for(unsigned int i=0;i<currentWeights.size();++i)
        newWeights.push_back(currentWeights[i]);
    }
    else
    {    
          // Invoke weight update engine
      Vector<double> newWeightsDouble;
      std::string reference=refSent;
      vector<string> nblistWithNoScr;
      for(unsigned int i=0;i<nblist.size();++i) nblistWithNoScr.push_back(nblist[i].second);
      llWeightUpdaterPtr->update(reference,
                                 nblistWithNoScr,
                                 scoreCompsVec,
                                 currentWeights,
                                 newWeightsDouble);

          // Create float vector with new weights
      newWeights.clear();
      for(unsigned int i=0;i<newWeightsDouble.size();++i)
        newWeights.push_back(newWeightsDouble[i]);
    }
  
    if(verbose)
    {
      cerr<<"The weights of the loglinear combination have been trained:"<<endl;
      cerr<<" - Previous weights:";
      for(unsigned int i=0;i<currentWeights.size();++i) cerr<<" "<<currentWeights[i];
      cerr<<endl;
      cerr<<" - New weights     :";
      for(unsigned int i=0;i<newWeights.size();++i) cerr<<" "<<newWeights[i];
      cerr<<endl;
    }
  }
  
  //---------------------------------
  int updatePmLinInterpWeights(std::string srcCorpusFileName,
                               std::string trgCorpusFileName,
                               DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPhrModelFeatPtr,
                               InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPhrModelFeatPtr,
                               int verbose/*=0*/)
  {
        // Initialize downhill simplex input parameters
    Vector<double> initial_weights;
        // Obtain weights
    initial_weights.push_back(dirPhrModelFeatPtr->get_lambda());
    initial_weights.push_back(invPhrModelFeatPtr->get_lambda());
    int ndim=initial_weights.size();
    double* start=(double*) malloc(ndim*sizeof(double));
    int nfunk;
    double* x=(double*) malloc(ndim*sizeof(double));
    double y;

        // Create temporary file
    FILE* tmp_file=tmpfile();
  
    if(tmp_file==0)
    {
      cerr<<"Error updating linear interpolation weights of the phrase model, tmp file could not be created"<<endl;
      return THOT_ERROR;
    }

        // Extract phrase pairs from development corpus
    Vector<Vector<PhrasePair> > unfiltInvPhrPairs;
    int ret=PhraseExtractUtils::extractPhrPairsFromCorpusFiles(invPhrModelFeatPtr->get_swmptr(),
                                                               dirPhrModelFeatPtr->get_swmptr(),
                                                               trgCorpusFileName,
                                                               srcCorpusFileName,
                                                               unfiltInvPhrPairs,
                                                               verbose);
        // Filter phrase pairs
    Vector<Vector<PhrasePair> > invPhrPairs;
    for(unsigned int i=0;i<unfiltInvPhrPairs.size();++i)
    {
      Vector<PhrasePair> invPhrPairVec;
      PhraseExtractUtils::filterPhrasePairs(unfiltInvPhrPairs[i],invPhrPairVec);
      invPhrPairs.push_back(invPhrPairVec);
    }

    if(ret!=THOT_OK)
      return THOT_ERROR;
  
        // Execute downhill simplex algorithm
    bool end=false;
    while(!end)
    {
          // Set initial weights (each call to step_by_step_simplex starts
          // from the initial weights)
      for(unsigned int i=0;i<initial_weights.size();++i)
        start[i]=initial_weights[i];
    
          // Execute step by step simplex
      double curr_dhs_ftol;
      ret=step_by_step_simplex(start,ndim,PHRSWLITM_DHS_FTOL,PHRSWLITM_DHS_SCALE_PAR,NULL,tmp_file,&nfunk,&y,x,&curr_dhs_ftol,false);
      switch(ret)
      {
        case THOT_OK: end=true;
          break;
        case DSO_NMAX_ERROR: cerr<<"Error updating linear interpolation weights of the phrase model, maximum number of iterations exceeded"<<endl;
          end=true;
          break;
        case DSO_EVAL_FUNC: // A new function evaluation is requested by downhill simplex
          double perp;
          int retEval=linInterpWeightsDhsEval(invPhrPairs,dirPhrModelFeatPtr,invPhrModelFeatPtr,tmp_file,x,perp);
          if(retEval==THOT_ERROR)
          {
            end=true;
            break;
          }
              // Print verbose information
          if(verbose>=1)
          {
            cerr<<"niter= "<<nfunk<<" ; current ftol= "<<curr_dhs_ftol<<" (FTOL="<<PHRSWLITM_DHS_FTOL<<") ; ";
            cerr<<"weights= "<<dirPhrModelFeatPtr->get_lambda()<<" "<<invPhrModelFeatPtr->get_lambda();
            cerr<<" ; perp= "<<perp<<endl; 
          }
          break;
      }
    }
  
        // Set new weights if updating was successful
    if(ret==THOT_OK)
    {
      dirPhrModelFeatPtr->set_lambda(start[0]);
      invPhrModelFeatPtr->set_lambda(start[1]);
    }
    else
    {
      dirPhrModelFeatPtr->set_lambda(initial_weights[0]);
      invPhrModelFeatPtr->set_lambda(initial_weights[1]);
    }

        // Clear variables
    free(start);
    free(x);
    fclose(tmp_file);

    if(ret!=THOT_OK)
      return THOT_ERROR;
    else
      return THOT_OK; 
  }
  
  //---------------------------------
  int linInterpWeightsDhsEval(const Vector<Vector<PhrasePair> >& invPhrPairs,
                              DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPhrModelFeatPtr,
                              InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPhrModelFeatPtr,
                              FILE* tmp_file,
                              double* x,
                              double& obj_func)
  {
    LgProb totalLogProb;
    bool weightsArePositive=true;
    bool weightsAreBelowOne=true;
  
        // Fix weights to be evaluated
    dirPhrModelFeatPtr->set_lambda(x[0]);
    invPhrModelFeatPtr->set_lambda(x[1]);
    for(unsigned int i=0;i<2;++i)
    {
      if(x[i]<0) weightsArePositive=false;
      if(x[i]>=1) weightsAreBelowOne=false;
    }
  
    if(weightsArePositive && weightsAreBelowOne)
    {
          // Obtain perplexity
      obj_func=phraseModelPerplexity(invPhrPairs,
                                     dirPhrModelFeatPtr,
                                     invPhrModelFeatPtr,
                                     obj_func);
    }
    else
    {
      obj_func=DBL_MAX;
    }
  
        // Print result to tmp file
    fprintf(tmp_file,"%g\n",obj_func);
    fflush(tmp_file);
        // step_by_step_simplex needs that the file position
        // indicator is set at the start of the stream
    rewind(tmp_file);

    return THOT_OK;
  }

  //---------------------------------
  double phraseModelPerplexity(const Vector<Vector<PhrasePair> >& invPhrPairs,
                               DirectPhraseModelFeat<SmtModel::HypScoreInfo>* dirPhrModelFeatPtr,
                               InversePhraseModelFeat<SmtModel::HypScoreInfo>* invPhrModelFeatPtr,
                               int /*verbose=0*/)
  {
        // Iterate over all sentences
    double loglikelihood=0;
    unsigned int numPhrPairs=0;
  
        // Obtain perplexity contribution for consistent phrase pairs
    for(unsigned int i=0;i<invPhrPairs.size();++i)
    {
      for(unsigned int j=0;j<invPhrPairs[i].size();++j)
      {
        Vector<std::string> srcPhrasePair=invPhrPairs[i][j].t_;
        Vector<std::string> trgPhrasePair=invPhrPairs[i][j].s_;

            // Obtain unweighted score for target given source
        Score unweightedPtsScr=dirPhrModelFeatPtr->scorePhrasePairUnweighted(srcPhrasePair,trgPhrasePair);
          
            // Obtain unweighted score for source given target
        Score unweightedPstScr=invPhrModelFeatPtr->scorePhrasePairUnweighted(srcPhrasePair,trgPhrasePair);
        
            // Update loglikelihood
        loglikelihood+=unweightedPtsScr+unweightedPstScr;
      }
          // Update number of phrase pairs
      numPhrPairs+=invPhrPairs[i].size();
    }

        // Return perplexity
    return -1*(loglikelihood/(double)numPhrPairs);
  }

}
