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
 

#ifndef __stack_decoder_statistics_h
#define __stack_decoder_statistics_h

//--------------- Include files ----------------------------------------------

#include <iostream>
#include <iomanip>
#include "ctimer.h"
#include "Prob.h"

using namespace std;

//--------------- Constants --------------------------------------------------


//--------------- _stack_decoder_statistics class: class for calculating statistics of
//                                                 the _stack_decoder class

class _stack_decoder_statistics
{
  public:
  
   unsigned long sentencesTranslated;	
   unsigned long totalExpansionNo;
   unsigned long totalPushNo;
   unsigned long pushPerIter;
   unsigned long pushAborted;
   unsigned long numIter;
   Prob nullHypHeuristicValue;
   Prob scoreOfOptimalHyp; 

   _stack_decoder_statistics(void)
     {
       sentencesTranslated=0;
       nullHypHeuristicValue=0;
       scoreOfOptimalHyp=0;
     }
   void clear(void)
     {
       numIter=0;	
       totalExpansionNo=0;
       totalPushNo=0;
       pushAborted=0;
     }
   ostream & print(ostream &outS)
     {
       outS<< " * Number of iterations           : " << numIter <<"\n";	 
       outS<< " * Total number of expansions     : " << totalExpansionNo <<"\n";
       outS<< " * Total push operations          : " << totalPushNo <<"\n";
       outS<< " * Push op's per expansion        : " << (float)totalPushNo/totalExpansionNo <<"\n";
       outS<< " * Push op's aborted (best score) : " << pushAborted <<"\n";
       return outS;
     }
   ostream & heuristicInfo(ostream &outS)
    {
      Prob heuristicVal;
      heuristicVal=(double)nullHypHeuristicValue/(double)sentencesTranslated;
      outS<<"* Predictive power of heuristic function: ";
      outS<<(float)scoreOfOptimalHyp/sentencesTranslated<<" - "<< heuristicVal<<" = ";
      outS<<((float)scoreOfOptimalHyp/(float)sentencesTranslated)-(float)heuristicVal<<"\n";
      return outS;
    }
};

#endif
