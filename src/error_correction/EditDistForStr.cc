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
/* Module: EditDistForStr                                           */
/*                                                                  */
/* Definitions file: EditDistForStr.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "EditDistForStr.h"

//--------------- Classes --------------------------------------------


//--------------- EditDist class method definitions

EditDistForStr::EditDistForStr(void):_editDist<std::string>()
{
}

//---------------------------------------
Score EditDistForStr::calculateEditDistPrefix(const std::string& x,
                                              const std::string& y,
                                              int verbose)
{
  std::vector<unsigned int> ops;

  return calculateEditDistPrefixOps(x,y,ops,verbose);
}

//---------------------------------------
Score EditDistForStr::calculateEditDistPrefixOps(const std::string& x,
                                                 const std::string& y,
                                                 std::vector<unsigned int>& ops,
                                                 int verbose)
{
  return calculateEditDistPrefixOpsAux(x,y,USE_PREF_DEL_OP,ops,verbose);
}

//---------------------------------------
Score EditDistForStr::calculateEditDistPrefixOpsNoPrefDel(const std::string& x,
                                                          const std::string& y,
                                                          std::vector<unsigned int>& ops,
                                                          int verbose)
{
  return calculateEditDistPrefixOpsAux(x,y,DONT_USE_PREF_DEL_OP,ops,verbose);
}

//---------------------------------------
Score EditDistForStr::calculateEditDistPrefixOpsAux(const std::string& x,
                                                    const std::string& y,
                                                    bool usePrefDelOp,
                                                    std::vector<unsigned int>& ops,
                                                    int verbose)
{
  int pred_i;
  int pred_j;
  int op_id;

  DistMatrix dm;
  initDm(x,y,dm);

      // Fill distance matrix
  for (unsigned int i=0; i<=x.size(); i++)
  {
    for (unsigned int j=0; j<=y.size(); j++) 
    {
      dm[i][j]=processMatrixCellPref(x,y,dm,usePrefDelOp,i,j,pred_i,pred_j,op_id);
    }
  }
      // Retrieve operations
  obtainOperationsPref(x,y,dm,usePrefDelOp,x.size(),y.size(),ops);

  if(verbose) printDistMatrix(x,y,dm,std::cerr);
  return dm[x.size()][y.size()];	
}

//---------------------------------------
Score EditDistForStr::processMatrixCell(const std::string& x,
                                        const std::string& y,
                                        const DistMatrix& dm,
                                        int i,
                                        int j,
                                        int& pred_i,
                                        int& pred_j,
                                        int& op_id)
{
  if(i!=0 && j!=0)
  {
    Score min;
    Score ins_cost;
    Score del_cost;
    pred_i=i-1;
    pred_j=j-1;
        
        // Treat substitution operation
    if(x[i-1]==y[j-1])
    {
      min = dm[i-1][j-1] + hitCost;
      op_id=HIT_OP;
    }
    else
    {
      min = dm[i-1][j-1] + substitutionCost(x[i-1],y[j-1]);
      op_id=SUBST_OP;       
    }
        
        // Treat deletion operation
    del_cost=deletionCost(x[i-1]);
    if (dm[i-1][j]+del_cost < min)
    {
      min = dm[i-1][j]+del_cost;
      pred_i=i-1;
      pred_j=j;
      op_id=DEL_OP;
    }

        // Treat insertion operation
    ins_cost=insertionCost(y[j-1]);
    if (dm[i][j-1]+ ins_cost< min)
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
Score EditDistForStr::processMatrixCellPref(const std::string& x,
                                            const std::string& y,
                                            const DistMatrix& dm,
                                            bool usePrefDelOp,
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
    Score ins_cost;
    Score del_cost;
    pred_i=i-1;
    pred_j=j-1;

        // Treat substitution operation
    if(x[i-1]==y[j-1])
    {
      min = dm[i-1][j-1] + hitCost;
      op_id=HIT_OP;
    }
    else
    {
      min = dm[i-1][j-1] + substitutionCost(x[i-1],y[j-1]);
      op_id=SUBST_OP;       
    }
        
        // Treat deletion operation
    if(usePrefDelOp && j==(int)y.size())
      del_cost=0;
    else
      del_cost=deletionCost(x[i-1]);
    if (dm[i-1][j]+del_cost < min)
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
void EditDistForStr::obtainOperationsPref(const std::string& x,
                                          const std::string& y,
                                          const DistMatrix& dm,
                                          bool usePrefDelOp,
                                          int i,
                                          int j,
                                          std::vector<unsigned int> &opsCharLevel)
{  
  std::vector<unsigned int> vuiaux;
  int op_id;

      // Trace back edit distance path
  while(i>0 || j>0)
  {
    processMatrixCellPref(x,y,dm,usePrefDelOp,i,j,i,j,op_id);
    if(op_id!=PREF_DEL_OP)
      vuiaux.push_back(op_id);
  }

      // Fill opsWordLevel
  opsCharLevel.clear();

  for(unsigned int i=0;i<vuiaux.size();++i)
  {
    opsCharLevel.push_back(vuiaux[vuiaux.size()-1-i]);
  }  
}

//---------------------------------------
EditDistForStr::~EditDistForStr(void)
{
}
