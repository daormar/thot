/*
thot package for statistical machine translation
Copyright (C) 2013 Vicent Alabau, Jes\'us Gonz\'alez-Rubio, Daniel Ortiz-Mart\'inez
 
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

//--------------- Include files --------------------------------------

#include "ThotIbmMaxConfid.h"

//--------------- ThotIbmMaxConfid class function definitions


//-------------------------
float ThotIbmMaxConfid::getWordConfidences(const vector<string> &source,
                                           const vector<string> &target,
                                           const vector<bool> &validated,
                                           vector<float> &confidences)
{
  vector<WordIndex> srcSnt;
  vector<WordIndex> trgSnt;
  float nconf=0,nconf2=0;
  string aux;
    
      // snt-ize source and target 
  srcSnt.clear();
  for(unsigned int i=0; i<source.size(); ++i)
    srcSnt.push_back(_ibm.stringToSrcWordIndex(source[i]));
  
  trgSnt.clear();
  for(unsigned int i=0; i<target.size(); ++i)
    trgSnt.push_back(_ibm.stringToTrgWordIndex(target[i]));
  
		
      // clean and prepare the confidences vector
  confidences.clear();
  confidences.resize(trgSnt.size());
  
    // we implement max ibm1 
    // validated words have confidence == 1
  for(unsigned int i=0; i<trgSnt.size(); ++i){
    if(!validated[i]){
      confidences[i]=float(_ibm.pts(NULL_WORD,trgSnt[i]));
      for(unsigned int j=0; j<srcSnt.size(); ++j){
        nconf=float(_ibm.pts(srcSnt[j],trgSnt[i]));
		
            // Eufemistic E. is heuristic
        aux=target[i];
        if(isupper(aux[0]) && !isupper(aux[1])){
          transform(aux.begin(), (aux.begin())+1, aux.begin(), ::tolower );
          nconf2=_ibm.pts(srcSnt[j],_ibm.stringToTrgWordIndex(aux));
          if( nconf2 > nconf)
            nconf=nconf2;
        }
        aux=source[j];
        if(isupper(aux[0]) && !isupper(aux[1])){
          transform(aux.begin(), (aux.begin())+1, aux.begin(), ::tolower );
          nconf2=_ibm.pts(_ibm.stringToSrcWordIndex(aux),trgSnt[i]);
          if( nconf2 > nconf)
            nconf=nconf2;
        }
            // Confidence 1.0 for numbers
        if(atoi(target[i].c_str())!=0 && target[i]==source[j]) nconf=1.0;
		
        if ( nconf > confidences[i] ) confidences[i]=nconf;
      }
    }else
      confidences[i]=1.0;
  }
  
  return EXIT_SUCCESS;
}

//-------------------------
float ThotIbmMaxConfid::getSentenceConfidence(const vector<string> &source,
                                              const vector<string> &target,
                                              const vector<bool> &validated)
{
  vector<float> confidences;
  float nconf;
  
  getWordConfidences(source, target, validated, confidences);
  
      // score combination average
  nconf=0.0;
  for(unsigned int i=0; i<confidences.size(); ++i)
    nconf+=confidences[i];
  nconf/=confidences.size();
  
  return nconf;
}
	

//-------------------------
void ThotIbmMaxConfid::update(const std::vector<std::string> &source,
                              const std::vector<std::string> &target) 
{
  pair<unsigned int, unsigned int> sentRange;
  
  if(!source.empty() && !target.empty()){
    _ibm.addSentPair(source, target, 1, sentRange);
    _ibm.trainSentPairRange(sentRange,0);
    LOG(INFO) << "Updated with new bilingual pair" << endl; 
  } 
}

//-------------------------
void ThotIbmMaxConfid::setLogger(Logger *logger) {
  _logger = logger;
  LOG(INFO) << "ThotIbmMaxConfid is joining the logger";
}
