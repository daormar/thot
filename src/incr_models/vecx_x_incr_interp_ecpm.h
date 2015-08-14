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
/* Module: vecx_x_incr_interp_ecpm                                  */
/*                                                                  */
/* Prototype file: vecx_x_incr_interp_ecpm.h                        */
/*                                                                  */
/* Description: Class to manage incremental interpolated encoded    */
/*              conditional probability models p(x|Vector<x>).      */
/*                                                                  */
/********************************************************************/

#ifndef _vecx_x_incr_interp_ecpm
#define _vecx_x_incr_interp_ecpm

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrInterpEncCondProbModel.h"
#include "awkInputStream.h"
#include "vecx_x_incr_ecpm.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- function declarations ------------------------------


//--------------- Classes --------------------------------------------

//--------------- vecx_x_incr_interp_ecpm class

template<class HX,class X,class SRC_INFO,class SRCTRG_INFO>
class vecx_x_incr_interp_ecpm: public _incrInterpEncCondProbModel<Vector<HX>,HX,Vector<X>,X,SRC_INFO,SRCTRG_INFO>
{
 public:

  typedef typename _incrInterpEncCondProbModel<Vector<HX>,HX,Vector<X>,X,SRC_INFO,SRCTRG_INFO>::SrcTableNode SrcTableNode;
  typedef typename _incrInterpEncCondProbModel<Vector<HX>,HX,Vector<X>,X,SRC_INFO,SRCTRG_INFO>::TrgTableNode TrgTableNode;

      // Constructor
  vecx_x_incr_interp_ecpm():_incrInterpEncCondProbModel<Vector<HX>,HX,Vector<X>,X,SRC_INFO,SRCTRG_INFO>()
  {
    this->encPtr=new vecx_x_incr_enc<HX,X>;
  }

      // Functions to load and print the model
  bool load(const char *fileName);
  bool print(const char *fileName);
      // Prints the interpolated in different files, using "fileName" as
      // prefix

  ostream& print(ostream &outS);
      // Prints the model pointed by modelIndex on the given output stream

      // Destructor
  ~vecx_x_incr_interp_ecpm();
   
 protected:

  Vector<std::string> modelFileNameVec;

  virtual void setTablePtr(void);
      // Sets the "tablePtr" pointer. This function is called internally
      // by the "load" function.  It allows to select the class of the
      // probability table encapsulated by the model.  Important note:
      // data types must match (SRCDATA, TRGDATA and SRCTRG_INFO) For
      // Interpolated models, this functions actually set the class of a
      // probability model instead of a probability table.
};

//--------------- Template function definitions

//---------------
template<class HX,class X,class SRC_INFO,class SRCTRG_INFO>
void vecx_x_incr_interp_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>::setTablePtr(void)
{
  vecx_x_incr_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>* vxxiPtr=new vecx_x_incr_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>;
  vxxiPtr->setEncodingPtr(this->encPtr);
  this->modelPtrVec[this->modelIndex]=vxxiPtr;
}

//---------------
template<class HX,class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_interp_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>::load(const char *fileName)
{
  awkInputStream awk;
  
  if(awk.open(fileName)==ERROR)
  {
    cerr<<"Error while loading model file "<<fileName<<endl;
    return ERROR;
  }
  else
  {
    cerr<<"Loading model file "<<fileName<<endl;

    this->release();
    while(awk.getln())
    {
      if(awk.NF==2)
      {
        bool ret;
        BaseIncrEncCondProbModel<Vector<HX>,HX,Vector<X>,X,SRC_INFO,SRCTRG_INFO>* biecmPtr;
        
        this->modelPtrVec.push_back(biecmPtr);
        this->modelIndex=this->modelPtrVec.size()-1;
        setTablePtr();
        this->modelFileNameVec.push_back(awk.dollar(2));
        ret=this->modelPtrVec[this->modelIndex]->load(this->modelFileNameVec[this->modelIndex].c_str());
        if(!ret) return false;
        this->weights.push_back(atof(awk.dollar(1).c_str()));
      }
    }
  }
  return OK;
}

//--------------
template<class HX,class X,class SRC_INFO,class SRCTRG_INFO>
bool vecx_x_incr_interp_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>::print(const char *fileName)
{
  ofstream outF;

  outF.open(fileName,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing model to file."<<endl;
    return ERROR;
  }
  else
  {
    for(unsigned int i=0;i<this->modelPtrVec.size();++i)
    {
      char outFileName[1024];

      sprintf(outFileName,"%s.%d",fileName,i);
      outF<<this->weights[i]<<" "<<outFileName<<endl;
      this->modelPtrVec[i]->print(outFileName);
    }
    outF.close();	
    return OK;
  }
}

//--------------
template<class HX,class X,class SRC_INFO,class SRCTRG_INFO>
ostream& vecx_x_incr_interp_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>::print(ostream &outS)
{
  if(this->modelIndex<this->modelPtrVec.size())
  {
    this->modelPtrVec[this->modelIndex]->print(outS);
  }
  else return outS;
}

//---------------
template<class HX,class X,class SRC_INFO,class SRCTRG_INFO>
vecx_x_incr_interp_ecpm<HX,X,SRC_INFO,SRCTRG_INFO>::~vecx_x_incr_interp_ecpm()
{
  delete this->encPtr;
}

#endif
