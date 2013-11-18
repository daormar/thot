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
 
#include "EditDistForVecString.h"


//--------------- EditDistForVecString function definitions

//---------------------------------------
EditDistForVecString::EditDistForVecString(void):EditDistForVec<std::string>()
{
}

//---------------------------------------
Score EditDistForVecString::calculateEditDist(const Vector<std::string>& x,
                                              const Vector<std::string>& y,
                                              int verbose)
{
  Vector<std::string> xVec;
  Vector<std::string> yVec;
  xVec=x;
  yVec=y;

#ifndef EDIT_DIST_WITHOUT_BLANKS
      // Calculate edit distance taking into account the blank characters
  addBlankCharacters(xVec);
  addBlankCharacters(yVec);
#endif

  return EditDistForVec<std::string>::calculateEditDist(xVec,yVec,verbose);
}

//---------------------------------------
Score EditDistForVecString::calculateEditDistPrefix(const Vector<std::string>& x,
                                                    const Vector<std::string>& y,
                                                    int verbose)
{
  Vector<unsigned int> opsWordLevel;
  Vector<unsigned int> opsCharLevel;
  
  return calculateEditDistPrefixOps(x,y,opsWordLevel,opsCharLevel,verbose);
}

//---------------------------------------
Score EditDistForVecString::calculateEditDistPrefixOps(const Vector<std::string>& x,
                                                       const Vector<std::string>& y,
                                                       Vector<unsigned int>& opsWordLevel,
                                                       Vector<unsigned int>& opsCharLevel,    
                                                       int verbose)
{
  return calculateEditDistPrefixOpsAux(x,y,opsWordLevel,opsCharLevel,USE_PREF_DEL_OP,verbose);
}

//---------------------------------------
Score EditDistForVecString::calculateEditDistPrefixOpsNoPrefDel(const Vector<std::string>& x,
                                                                const Vector<std::string>& y,
                                                                Vector<unsigned int>& opsWordLevel,
                                                                Vector<unsigned int>& opsCharLevel,                                                    
                                                                int verbose)
{
  return calculateEditDistPrefixOpsAux(x,y,opsWordLevel,opsCharLevel,DONT_USE_PREF_DEL_OP,verbose);
}

//---------------------------------------
Score EditDistForVecString::calculateEditDistPrefixOpsAux(const Vector<std::string>& x,
                                                          const Vector<std::string>& y,
                                                          Vector<unsigned int>& opsWordLevel,
                                                          Vector<unsigned int>& opsCharLevel,
                                                          bool usePrefDelOp,
                                                          int verbose)
{
  bool lastWordIsComplete=true;
  Vector<std::string> xVec;
  Vector<std::string> yVec;
  xVec=x;
  yVec=y;
  if(!y.empty())
  {
        // A blank character in the last word of y means that this word
        // should not be treated as a prefix
    std::string lasty=y[y.size()-1];
    std::string lastyWithoutBlanks;
    lastWordIsComplete=StrProcUtils::lastCharIsBlank(lasty);
    if(lastWordIsComplete)
      lastyWithoutBlanks=StrProcUtils::removeLastBlank(lasty);
    else lastyWithoutBlanks=lasty;

    yVec[yVec.size()-1]=lastyWithoutBlanks;
  }

#ifndef EDIT_DIST_WITHOUT_BLANKS
      // Calculate edit distance taking into account the blank characters
  addBlankCharacters(xVec);
  addBlankCharacters(yVec);
#endif

  DistMatrix dm;
  initDm(xVec,yVec,dm);

      // Fill edit distance matrix
  SubstCostMap substCostMap;
  int pred_i;
  int pred_j;
  int op_id;
  for (unsigned int i=0; i<=xVec.size(); i++)
  {
    for (unsigned int j=0; j<=yVec.size(); j++) 
    {
      dm[i][j]=processMatrixCellPref(xVec,yVec,dm,substCostMap,lastWordIsComplete,usePrefDelOp,i,j,pred_i,pred_j,op_id);
    }
  }

      // Retrieve word level operations
  Vector<Score> opCosts;
  obtainOperationsPref(xVec,yVec,dm,lastWordIsComplete,usePrefDelOp,x.size(),y.size(),opsWordLevel,opsCharLevel,opCosts);

      // Obtain costs per operation type
  Vector<unsigned int> opsPerType;
  Vector<Score> opCostsPerType;
  obtainOpsAndOpCostsPerType(opsWordLevel,opCosts,opsPerType,opCostsPerType);

      // Print verbose information

      // If verbose, print distance matrix
  if(verbose) printDistMatrix(xVec,yVec,dm,cerr);

      // If verbose, print operation costs per type
  if(verbose)
  {
    cerr<<"Operation costs per type: ";
    cerr<<HIT_OP_STR<<": "<<opCostsPerType[HIT_OP]<<" ; "<<INS_OP_STR<<": "<<opCostsPerType[INS_OP]<<" ; "<<SUBST_OP_STR<<": "<<opCostsPerType[SUBST_OP]<<" ; "<<DEL_OP_STR<<": "<<opCostsPerType[DEL_OP]<<endl;
  }

      // return edit distance
  return dm[x.size()][y.size()];	
}

//---------------------------------------
void EditDistForVecString::addBlankCharacters(Vector<std::string> strVec)
{
  for(unsigned int i=0;i<strVec.size();++i)
  {
    if(i<strVec.size()-1)
      strVec[strVec.size()-1].push_back(' ');
  }
}

//---------------------------------------
void EditDistForVecString::incrEditDistPrefixFirstRow(const Vector<std::string>& incr_y,
                                                      const Vector<Score> prevScoreVec,
                                                      Vector<Score>& newScoreVec)
{
  newScoreVec=prevScoreVec; 
  unsigned int startingPos=newScoreVec.size();
  
  for(unsigned int j=0; j<incr_y.size(); ++j)
  {
    if(startingPos+j==0)
      newScoreVec.push_back(insertionCost(incr_y[j]));
    else
      newScoreVec.push_back(newScoreVec[startingPos+j-1] + insertionCost(incr_y[j]));
  }
}

//---------------------------------------
void EditDistForVecString::incrEditDistPrefix(const std::string& xWord,
                                              const Vector<std::string>& incr_y,
                                              const Vector<Score> prevScoreVec,
                                              SubstCostMap& substCostMap,
                                              Vector<Score>& newScoreVec,
                                              Vector<int>& opIdVec)
{
      // Execute typical edit distance algorithm except for the specific
      // substitution cost for the last word (note: y is the incomplete
      // prefix)

      // A blank character in the last word of y means that this word
      // should not be treated as a prefix
  std::string lasty=incr_y[incr_y.size()-1];
  std::string lastyWithoutBlanks;
  bool lastWordIsComplete=StrProcUtils::lastCharIsBlank(lasty);
  if(lastWordIsComplete)
    lastyWithoutBlanks=StrProcUtils::removeLastBlank(lasty);
  else lastyWithoutBlanks=lasty;

      // Init x vector
  Vector<std::string> xVec;
  xVec.push_back(xWord);

      // Init y vector
  Vector<std::string> yVec;
  yVec.insert(yVec.begin(),prevScoreVec.size()-1,"");
  for(unsigned int i=0;i<incr_y.size();++i)
    yVec[prevScoreVec.size()-incr_y.size()-1+i]=incr_y[i];
  yVec[yVec.size()-1]=lastyWithoutBlanks;

#ifndef EDIT_DIST_WITHOUT_BLANKS
      // Calculate edit distance taking into account the blank
      // characters
  addBlankCharacters(xVec);
  addBlankCharacters(yVec);
#endif

      // Initialize data structures
  DistMatrix dm;
  initDm(xVec,yVec,dm);

      // Fill dm
  for(unsigned int j=0;j<prevScoreVec.size();++j)
    dm[0][j]=prevScoreVec[j];
  for(unsigned int j=0;j<newScoreVec.size();++j)
    dm[1][j]=newScoreVec[j];
  
      // Make room for newScoreVec
  while(newScoreVec.size()<prevScoreVec.size())
    newScoreVec.push_back(0);

      // Set starting position
  unsigned int startyPos=prevScoreVec.size()-incr_y.size();

      // Fill newScoreVec and opIdVec with the
      // appropriate values
  opIdVec.clear();
  int pred_i;
  int pred_j;
  int op_id;
  for (unsigned int j=0; j<incr_y.size(); j++)
  {
    Score dist=processMatrixCellPref(xVec,
                                     yVec,
                                     dm,
                                     substCostMap,
                                     lastWordIsComplete,
                                     DONT_USE_PREF_DEL_OP,
                                     1,
                                     startyPos+j,
                                     pred_i,
                                     pred_j,
                                     op_id);
    newScoreVec[startyPos+j]=dist;
    dm[1][startyPos+j]=dist;
    opIdVec.push_back(op_id);
  }
}

//---------------------------------------
void EditDistForVecString::setErrorModel(Score _hitCost,
                                         Score _insCost,
                                         Score _substCost,
                                         Score _delCost)
{
      // Set word-level costs
  hitCost=_hitCost;
  insCost=_insCost;
  substCost=_substCost;
  delCost=_delCost;

      // Set character-level costs
  editDistForStr.setErrorModel(hitCost,insCost,substCost,delCost);
}

//---------------------------------------
Score EditDistForVecString::processMatrixCell(const Vector<std::string>& x,
                                              const Vector<std::string>& y,
                                              const DistMatrix& dm,
                                              int i,
                                              int j,
                                              int& pred_i,
                                              int& pred_j,
                                              int& op_id)
{
  if(i!=0 && j!=0)
  {
        // Init variables
    Score min;
    Score subst_cost;
        
    pred_i=i-1;
    pred_j=j-1;
    
        // Treat substitution operation
    subst_cost=substitutionCost(x[i-1],y[j-1]);
    min = dm[i-1][j-1] + subst_cost;
        //Substitution cost is the edit distance between string
        //xVec[i-1] and yVec[j-1]
    if(x[i-1]==y[j-1])
    {
      op_id=HIT_OP;
    }
    else
    {
      op_id=SUBST_OP;       
    }
        
    if(dm[i-1][j]+deletionCost(x[i-1]) < min)
    {
      min = dm[i-1][j]+deletionCost(x[i-1]);
      pred_i=i-1;
      pred_j=j;
      op_id=DEL_OP;
    }
    if (dm[i][j-1]+insertionCost(y[j-1]) < min)
    {
      min = dm[i][j-1]+insertionCost(y[j-1]);
      pred_i=i;
      pred_j=j-1;
      op_id=INS_OP;
    }
    return min;
  }
  else
  {
    if(i==0 && j==0)
    {
      pred_i=0;
      pred_j=0;
      op_id=NONE_OP;
      return 0;
    }
    else
    {
      if(i==0)
      {
        pred_i=0;
        pred_j=j-1;
        op_id=INS_OP;
        return dm[0][j-1] + insertionCost(y[j-1]);
      }
      else
      {
        pred_i=i-1;
        pred_j=0;
        op_id=DEL_OP;
        return dm[i-1][0] + deletionCost(x[i-1]);
      }
    }
  }
}

//---------------------------------------
Score EditDistForVecString::processMatrixCellPref(const Vector<std::string>& x,
                                                  const Vector<std::string>& y,
                                                  const DistMatrix& dm,
                                                  SubstCostMap& substCostMap,
                                                  bool lastWordIsComplete,
                                                  bool usePrefDelOp,
                                                  int i,
                                                  int j,
                                                  int& pred_i,
                                                  int& pred_j,
                                                  int& op_id)
{
  if(i!=0 && j!=0)
  {
    Score min;
    Score subst_cost;
    Score ins_cost;
    Score del_cost;
        
    pred_i=i-1;
    pred_j=j-1;
    
        // Treat substitution operation
    if(j==(int)y.size() && !lastWordIsComplete)
    {
      subst_cost=cachedPrefSubstCost(x[i-1],y[j-1],substCostMap);
    }
    else
    {
      subst_cost=cachedSubstCost(x[i-1],y[j-1],substCostMap);
    }
    min = dm[i-1][j-1] + subst_cost;
        // Substitution cost is the edit distance between string
        // xVec[i-1] and yVec[j-1]
    if(x[i-1]==y[j-1] || (!lastWordIsComplete && StrProcUtils::isPrefix(y[j-1],x[i-1])))
    {
      op_id=HIT_OP;
    }
    else
    {
      op_id=SUBST_OP;       
    }

        // Treat deletion operation

        // If the last word has already been introduced, the deletion is
        // done with no cost
    if(usePrefDelOp && j==(int)y.size())
      del_cost=0;
    else
      del_cost=deletionCost(x[i-1]);

    if(dm[i-1][j]+del_cost< min)
    {
      min = dm[i-1][j]+del_cost;
      pred_i=i-1;
      pred_j=j;
      if(del_cost==0) op_id=PREF_DEL_OP;
      else op_id=DEL_OP;
    }

        // Treat insertion operation
    ins_cost=insertionCost(y[j-1]);
    if (dm[i][j-1]+ins_cost < min)
    {
      min = dm[i][j-1]+ins_cost;
      pred_i=i;
      pred_j=j-1;
      op_id=INS_OP;
    }
    return min;
  }
  else
  {
    if(i==0 && j==0)
    {
      pred_i=0;
      pred_j=0;
      op_id=NONE_OP;
      return 0;
    }
    else
    {
      if(i==0)
      {
        pred_i=0;
        pred_j=j-1;
        op_id=INS_OP;
        return dm[0][j-1] + insertionCost(y[j-1]);
      }
      else
      {
        pred_i=i-1;
        pred_j=0;
        op_id=DEL_OP;
        return dm[i-1][0] + deletionCost(x[i-1]);
      }
    }
  }
}

//---------------------------------------
Score EditDistForVecString::cachedPrefSubstCost(std::string xWord,
                                                std::string yWord,
                                                SubstCostMap& substCostMap)
{
  std::string xWordAdapted=xWord+" pref";
  SubstCostMap::const_iterator scmConstIter=substCostMap.find(make_pair(xWordAdapted,yWord));
  if(scmConstIter!=substCostMap.end())
    return scmConstIter->second;
  else
  {
    Score subst_cost=prefSubstitutionCost(xWord,yWord);
    substCostMap[make_pair(xWordAdapted,yWord)]=subst_cost;
    return subst_cost;
  }
}

//---------------------------------------
Score EditDistForVecString::cachedSubstCost(std::string xWord,
                                            std::string yWord,
                                            SubstCostMap& substCostMap)
{
  SubstCostMap::const_iterator scmConstIter=substCostMap.find(make_pair(xWord,yWord));
  if(scmConstIter!=substCostMap.end())
    return scmConstIter->second;
  else
  {
    Score subst_cost=substitutionCost(xWord,yWord);
    substCostMap[make_pair(xWord,yWord)]=subst_cost;
    return subst_cost;
  }
}

//---------------------------------------
void EditDistForVecString::obtainOperationsPref(const Vector<std::string>& x,
                                                const Vector<std::string>& y,
                                                const DistMatrix& dm,
                                                bool lastWordIsComplete,
                                                bool usePrefDelOp,
                                                int i,
                                                int j,
                                                Vector<unsigned int> &opsWordLevel,
                                                Vector<unsigned int> &opsCharLevel,
                                                Vector<Score>& opCosts)
{
      // Init variables
  Vector<unsigned int> vuiaux;
  Vector<Score> vscraux;
  int op_id;

      // Trace back edit distance path
  while(i>0 || j>0)
  {
        // Obtain next word level operation
    SubstCostMap substCostMap;
    Score dist=processMatrixCellPref(x,y,dm,substCostMap,lastWordIsComplete,usePrefDelOp,i,j,i,j,op_id);
    if(op_id!=PREF_DEL_OP)
    {
      vuiaux.push_back(op_id);
      vscraux.push_back(dist-dm[i][j]);
    }
    
        // Check whether to calculate char level operations
    if(j+1==(int)y.size() && !lastWordIsComplete)
    {
      if(op_id==HIT_OP)
      {
#ifdef EDIT_DIST_FAST_ED_VECSTR
        opsCharLevel.clear();
        for(unsigned int k=0;k<y.back().size();++k)
          opsCharLevel.push_back(HIT_OP);
#else
        editDistForStr.calculateEditDistPrefixOps(x[i],y.back(),opsCharLevel);
#endif
      }
    }
  }

      // Fill opsWordLevel and opCosts
  opsWordLevel.clear();
  opCosts.clear();
  for(unsigned int i=0;i<vuiaux.size();++i)
  {
    opsWordLevel.push_back(vuiaux[vuiaux.size()-1-i]);
    opCosts.push_back(vscraux[vscraux.size()-1-i]);
  }  
}

//---------------------------------------
EditDistForVecString::~EditDistForVecString(void)
{
}
