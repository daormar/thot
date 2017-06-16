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

  //---------------------------------
  int linInterpWeightDhsEval(const Vector<Vector<PhrasePair> >& invPhrPairs,
                             FILE* tmp_file,
                             double* x,
                             double& obj_func);

      // Function definitions
  
  //---------------------------------
  int updateLinInterpWeights(std::string srcDevCorpusFileName,
                             std::string trgDevCorpusFileName,
                             int verbose/*=0*/)
  {
        // Initialize downhill simplex input parameters
    Vector<double> initial_weights;
        // Obtain weights (TO-BE-DONE)
        // initial_weights.push_back(swModelInfoPtr->lambda_swm);
        // initial_weights.push_back(swModelInfoPtr->lambda_invswm);
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
      return ERROR;
    }

        // Extract phrase pairs from development corpus
    Vector<Vector<PhrasePair> > invPhrPairs;
        // TO-BE-DONE
    int ret;
//  ret=extractPhrPairsFromDevCorpus(srcDevCorpusFileName,trgDevCorpusFileName,invPhrPairs,verbose);
    if(ret!=OK)
      return ERROR;
  
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
        case OK: end=true;
          break;
        case DSO_NMAX_ERROR: cerr<<"Error updating linear interpolation weights of the phrase model, maximum number of iterations exceeded"<<endl;
          end=true;
          break;
        case DSO_EVAL_FUNC: // A new function evaluation is requested by downhill simplex
          double perp;
          int retEval=linInterpWeightDhsEval(invPhrPairs,tmp_file,x,perp);
          if(retEval==ERROR)
          {
            end=true;
            break;
          }
              // Print verbose information
          if(verbose>=1)
          {
            cerr<<"niter= "<<nfunk<<" ; current ftol= "<<curr_dhs_ftol<<" (FTOL="<<PHRSWLITM_DHS_FTOL<<") ; ";
            // cerr<<"weights= "<<swModelInfoPtr->lambda_swm<<" "<<swModelInfoPtr->lambda_invswm;
            cerr<<" ; perp= "<<perp<<endl; 
          }
          break;
      }
    }
  
        // Set new weights if updating was successful
        // TO-BE-DONE
        // if(ret==OK)
        // {
        //   swModelInfoPtr->lambda_swm=start[0];
        //   swModelInfoPtr->lambda_invswm=start[1];
        // }
        // else
        // {
        //   swModelInfoPtr->lambda_swm=initial_weights[0];
        //   swModelInfoPtr->lambda_invswm=initial_weights[1];
        // }
  
        // Clear variables
    free(start);
    free(x);
    fclose(tmp_file);

    if(ret!=OK)
      return ERROR;
    else
      return OK; 
  }
  
  //---------------
  int linInterpWeightDhsEval(const Vector<Vector<PhrasePair> >& invPhrPairs,
                             FILE* tmp_file,
                             double* x,
                             double& obj_func)
  {
        // TO-BE-DONE
  }
}
