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
/* Module: _editDist                                                */
/*                                                                  */
/* Prototypes file: _editDist.h                                     */
/*                                                                  */
/* Description: Defines the _editDist class, a predecessor class    */
/*              for derivating clases for calculating edit distance.*/
/*                                                                  */
/********************************************************************/

#ifndef __editDist_h
#define __editDist_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BaseEditDist.h"
#include <stdlib.h>                             
#include <iostream>
#include <iomanip>
#include <vector>

//--------------- Constants ------------------------------------------


//--------------- Type definitions -----------------------------------

typedef std::vector<std::vector<Score> > DistMatrix;

//--------------- Classes --------------------------------------------


//--------------- _editDist class declaration

template<class OBJ>
class _editDist: public BaseEditDist<OBJ>
{
 public:
  
      // Constructor
  _editDist();

  void setErrorModel(Score _hitCost,
                     Score _insCost,
                     Score _substCost,
                     Score _delCost); 
      // Sets the cost of each operation (hits, insertions, deletions and
      // substitutions)

  Score calculateEditDist(const OBJ& x,
                          const OBJ& y,
                          int verbose=0);
      // Calculates edit distance between OBJ's x and y (operations
      // to transform x into y)
  
  Score calculateEditDistOps(const OBJ& x,
                             const OBJ& y,
                             unsigned int& hCount,
                             unsigned int& iCount,
                             unsigned int& sCount,
                             unsigned int& dCount,
                             int verbose=0);
      // Version of the previous function returning the number of edit
      // operations

      // destructor
  virtual ~_editDist();

	
 protected:
  
  Score hitCost;
  Score insCost;
  Score substCost;
  Score delCost;

  Score calculateEditDistDm(const OBJ& x,
                            const OBJ& y,
                            DistMatrix& dm,
                            int verbose);
      // Auxiliary function to calculate edit distance

  virtual Score processMatrixCell(const OBJ& x,
                                  const OBJ& y,
                                  const DistMatrix& dm,
                                  int i,
                                  int j,
                                  int& pred_i,
                                  int& pred_j,
                                  int& op_id)=0;
      // Basic function to calculate edit distance

      // Miscellaneous operations
  void countOperations(const OBJ& x,
                       const OBJ& y,
                       const DistMatrix& dm,
                       int i,
                       int j,
                       std::vector<unsigned int> &opsPerType,
                       std::vector<Score> &opCostsPerType);
      // After an edit distance calculation, this function counts how
      // many insertions, deletions and substitutions were necessary.

  void countOpsGivenOpVec(const std::vector<unsigned int>& ops,
                          std::vector<unsigned int> &opsPerType);
      // Count operations given vector with operations

  void obtainOpsAndOpCostsPerType(const std::vector<unsigned int>& ops,
                                  const std::vector<Score>& opCosts,
                                  std::vector<unsigned int> &opsPerType,
                                  std::vector<Score> &opCostsPerType);
      // Obtain operations and operations costs per each edit operation
      // type

  void obtainOperations(const OBJ& x,
                        const OBJ& y,
                        const DistMatrix& dm,
                        int i,
                        int j,
                        std::vector<unsigned int> &ops,
                        std::vector<Score> &opCosts);
      // After an edit distance calculation, this function obtains the
      // optimal sequence of operations.

      // Initialize edit distance matrix
  void initDm(const OBJ& x,
              const OBJ& y,
              DistMatrix& dm);
  
      // Verbose-mode related functions
  void printDistMatrix(const OBJ& x,
                       const OBJ& y,
                       const DistMatrix& dm,
                       std::ostream &outS);
};

//--------------- EditDist class function definitions

//---------------------------------------
template<class OBJ>
_editDist<OBJ>::_editDist()
{
  hitCost=0;
  insCost=1;
  substCost=1;
  delCost=1;
}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::setErrorModel(Score _hitCost,
                                   Score _insCost,
                                   Score _substCost,
                                   Score _delCost)
{
  hitCost=_hitCost;
  insCost=_insCost;
  substCost=_substCost;
  delCost=_delCost;
}

//---------------------------------------
template<class OBJ>
Score _editDist<OBJ>::calculateEditDist(const OBJ& x,
                                        const OBJ& y,
                                        int verbose)
{
  DistMatrix dm;
  return calculateEditDistDm(x,y,dm,verbose);
}

//---------------------------------------
template<class OBJ>
Score _editDist<OBJ>::calculateEditDistDm(const OBJ& x,
                                          const OBJ& y,
                                          DistMatrix& dm,
                                          int verbose)
{
  int pred_i;
  int pred_j;
  int op_id;

  initDm(x,y,dm);

  for (unsigned int i=0; i<=x.size(); i++)
  {
    for (unsigned int j=0; j<=y.size(); j++) 
    {
      dm[i][j]=processMatrixCell(x,y,dm,i,j,pred_i,pred_j,op_id);
    }
  }
  if(verbose) printDistMatrix(x,y,dm,std::cerr);
  return dm[x.size()][y.size()];	
}

//---------------------------------------
template<class OBJ>
Score _editDist<OBJ>::calculateEditDistOps(const OBJ& x,
                                           const OBJ& y,
                                           unsigned int& hCount,
                                           unsigned int& iCount,
                                           unsigned int& sCount,
                                           unsigned int& dCount,
                                           int verbose)
{
      // Calculate edit distance
  Score dist;
  DistMatrix dm;
  dist=calculateEditDistDm(x,y,dm,verbose);

      // Obtain information about operations
  std::vector<unsigned int> opsPerType;
  std::vector<Score> opCostsPerType;
  countOperations(x,y,dm,(int)x.size(),(int)y.size(),opsPerType,opCostsPerType);

      // Set values of output variables
  hCount=opsPerType[HIT_OP];
  iCount=opsPerType[INS_OP];
  sCount=opsPerType[SUBST_OP];
  dCount=opsPerType[DEL_OP];

      // If verbose, print operation costs per type
  if(verbose)
  {
    std::cerr<<"Operation costs per type: ";
    std::cerr<<HIT_OP_STR<<": "<<opCostsPerType[HIT_OP]<<" ; "<<INS_OP_STR<<": "<<opCostsPerType[INS_OP]<<" ; "<<SUBST_OP_STR<<": "<<opCostsPerType[SUBST_OP]<<" ; "<<DEL_OP_STR<<": "<<opCostsPerType[DEL_OP]<<std::endl;
  }
    
      // Return edit distance
  return dist;	
}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::countOperations(const OBJ& x,
                                     const OBJ& y,
                                     const DistMatrix& dm,
                                     int i,
                                     int j,
                                     std::vector<unsigned int> &opsPerType,
                                     std::vector<Score> &opCostsPerType)
{
  std::vector<unsigned int> ops;
  std::vector<Score> opCosts;
  
  obtainOperations(x,y,dm,i,j,ops,opCosts);
  obtainOpsAndOpCostsPerType(ops,opCosts,opsPerType,opCostsPerType);
}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::countOpsGivenOpVec(const std::vector<unsigned int>& ops,
                                        std::vector<unsigned int> &opsPerType)
{
  opsPerType.clear();
  for(unsigned int k=0;k<4;++k)
  {
    opsPerType.push_back(0);
  }
  
  
  for(unsigned int k=0;k<ops.size();++k)
  {
    switch(ops[k])
    {
      case HIT_OP: ++opsPerType[HIT_OP];
        break;
      case INS_OP: ++opsPerType[INS_OP];
        break;
      case SUBST_OP: ++opsPerType[SUBST_OP];
        break;
      case DEL_OP: ++opsPerType[DEL_OP];
        break;
    }
  }
}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::obtainOpsAndOpCostsPerType(const std::vector<unsigned int>& ops,
                                                const std::vector<Score>& opCosts,
                                                std::vector<unsigned int> &opsPerType,
                                                std::vector<Score> &opCostsPerType)
{
  opsPerType.clear();
  opCostsPerType.clear();
  for(unsigned int k=0;k<4;++k)
  {
    opsPerType.push_back(0);
    opCostsPerType.push_back(0);
  }
  
  
  for(unsigned int k=0;k<ops.size();++k)
  {
    switch(ops[k])
    {
      case HIT_OP: ++opsPerType[HIT_OP];
        opCostsPerType[HIT_OP]+=opCosts[k];
        break;
      case INS_OP: ++opsPerType[INS_OP];
        opCostsPerType[INS_OP]+=opCosts[k];
        break;
      case SUBST_OP: ++opsPerType[SUBST_OP];
        opCostsPerType[SUBST_OP]+=opCosts[k];
        break;
      case DEL_OP: ++opsPerType[DEL_OP];
        opCostsPerType[DEL_OP]+=opCosts[k];
        break;
    }
  }
}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::obtainOperations(const OBJ& x,
                                      const OBJ& y,
                                      const DistMatrix& dm,
                                      int i,
                                      int j,
                                      std::vector<unsigned int> &ops,
                                      std::vector<Score> &opCosts)
{
  std::vector<unsigned int> opsAux;
  std::vector<Score> opCostsAux;
  int op_id;
        
  while(i>0 || j>0)
  {
    Score dist=processMatrixCell(x,y,dm,i,j,i,j,op_id);
    opsAux.push_back(op_id);
    opCostsAux.push_back(dist-dm[i][j]);
  }

  ops.clear();

  for(unsigned int i=0;i<opsAux.size();++i)
  {
    ops.push_back(opsAux[opsAux.size()-1-i]);
  }

  opCosts.clear();

  for(unsigned int i=0;i<opCostsAux.size();++i)
  {
    opCosts.push_back(opCostsAux[opCostsAux.size()-1-i]);
  }

}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::initDm(const OBJ& x,
                            const OBJ& y,
                            DistMatrix& dm)
{
  unsigned int dim=std::max(x.size(),y.size());

/*   std::vector<Score> srcv; */
  
/*   for(unsigned int i=0;i<=dim;++i)  */
/*     srcv.push_back(0);  */

/*   dm.clear(); */
/*   for(unsigned int i=0;i<=dim;++i)  */
/*     dm.push_back(srcv);  */
/*   dm.insert(dm.begin(),dim+1,srcv); */

  std::vector<Score> scrv(dim+1,0);
  dm.clear();
  dm.insert(dm.begin(),dim+1,scrv);
}

//---------------------------------------
template<class OBJ>
void _editDist<OBJ>::printDistMatrix(const OBJ& x,
                                     const OBJ& y,
                                     const DistMatrix& dm,
                                     std::ostream &outS)
{
  for(unsigned int i=0;i<=x.size();++i)
  {
    for(unsigned int j=0;j<=y.size();++j)
      outS<<dm[i][j]<<" ";
    outS<<std::endl;
  }
}

//---------------------------------------
template<class OBJ>
_editDist<OBJ>::~_editDist()
{
}

#endif
