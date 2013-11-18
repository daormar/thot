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
/* Module: anjiMatrix                                               */
/*                                                                  */
/* Definitions file: anjiMatrix.cc                                  */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "anjiMatrix.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- anjiMatrix class function definitions

//-------------------------
anjiMatrix::anjiMatrix(void)
{
  anji_maxnsize=UNRESTRICTED_ANJI_SIZE;
  anji_pointer=0;
}

//-------------------------
void anjiMatrix::set_maxnsize(unsigned int _anji_maxnsize)
{
  clear();
  anji_maxnsize=_anji_maxnsize;
}

//-------------------------
unsigned int anjiMatrix::get_maxnsize(void)
{
  return anji_maxnsize;
}

//-------------------------
unsigned int anjiMatrix::n_size(void)
{
  return anji.size();
}

//-------------------------
unsigned int anjiMatrix::nj_size(unsigned int n)
{
  return anji[n].size();
}

//-------------------------
unsigned int anjiMatrix::nji_size(unsigned int n,
                                  unsigned int j)
{
  return anji[n][j].size();
}

//-------------------------
bool anjiMatrix::load(const char* prefFileName)
{
      // Clear data structures
  clear();

      // Load information from files
  bool retVal;
  std::string anjiFile=prefFileName;
  anjiFile=anjiFile+".anji";
  retVal=load_anji_values(anjiFile.c_str());
  if(retVal==ERROR) return ERROR;

  std::string maxnsizeDataFile=prefFileName;
  maxnsizeDataFile=maxnsizeDataFile+".msinfo";
  retVal=load_maxnsize_data(maxnsizeDataFile.c_str());
  if(retVal==ERROR)
  {
    cerr<<"Maximum size for anji is set to "<<UNRESTRICTED_ANJI_SIZE<<" (unrestricted size)."<<endl;
    anji_maxnsize=UNRESTRICTED_ANJI_SIZE;
  }
  return OK;
}

//-------------------------
bool anjiMatrix::load_anji_values(const char* anjiFile)
{
  cerr<<"Loading file with anji values from "<<anjiFile<<endl;

      // Try to open file  
  ifstream inF (anjiFile, ios::in | ios::binary);
  if (!inF)
  {
    cerr<<"File with anji values "<<anjiFile<<" does not exist.\n";
    return ERROR;    
  }
  else
  {
        // Read register
    bool end=false;
    while(!end)
    {
      unsigned int n;
      unsigned int j;
      unsigned int i;
      float f;
      if(inF.read((char*)&n,sizeof(unsigned int)))
      {
        inF.read((char*)&j,sizeof(unsigned int));
        inF.read((char*)&i,sizeof(unsigned int));
        inF.read((char*)&f,sizeof(float));
        set(n,j,i,f);
      }
      else end=true;
    }
    return OK;
  }
}

//-------------------------
bool anjiMatrix::load_maxnsize_data(const char* maxnsizeDataFile)
{
  awkInputStream awk;

      // Try to open file  
  if(awk.open(maxnsizeDataFile)==ERROR)
  {
    cerr<<"Error in file with anji maximum size data, file "<<maxnsizeDataFile<<" does not exist.\n";
    return ERROR;
  }  
  else
  {
        // Read values
    cerr<<"Reading anji maximum size data from file: "<<maxnsizeDataFile<<endl;
    awk.getline();
    anji_maxnsize=atoi(awk.dollar(1).c_str());
    awk.getline();
    anji_pointer=atoi(awk.dollar(1).c_str());
    
    while(awk.getline())
    {
      if(awk.NF==2)
      {
        unsigned int np=atoi(awk.dollar(1).c_str());
        unsigned int n=atoi(awk.dollar(2).c_str());
        
        update_np_to_n_vector(np,make_pair(true,n));
        update_n_to_np_vector(n,make_pair(true,np));
      }
    }
  }
  return OK;     
}

//-------------------------
bool anjiMatrix::print(const char* prefFileName)
{
  bool retVal;
  std::string anjiFile=prefFileName;
  anjiFile=anjiFile+".anji";
  retVal=print_anji_values(anjiFile.c_str());
  if(retVal==ERROR) return ERROR;

  if(anji_maxnsize!=UNRESTRICTED_ANJI_SIZE)
  {
    std::string maxnsizeDataFile=prefFileName;
    maxnsizeDataFile=maxnsizeDataFile+".msinfo";
    retVal=print_maxnsize_data(maxnsizeDataFile.c_str());
    if(retVal==ERROR) return ERROR;
  }
  
  return OK;
}

//-------------------------
bool anjiMatrix::print_anji_values(const char* anjiFile)
{
  ofstream outF;
  outF.open(anjiFile,ios::out|ios::binary);
  if(!outF)
  {
    cerr<<"Error while printing anji file."<<endl;
    return ERROR;
  }
  else
  {    
        // print file with anji values
    for(unsigned int n=0;n<anji.size();++n)
    {
      for(unsigned int j=0;j<anji[n].size();++j)
      {
        for(unsigned int i=0;i<anji[n][j].size();++i)
        {
          outF.write((char*)&n,sizeof(unsigned int));
          outF.write((char*)&j,sizeof(unsigned int));
          outF.write((char*)&i,sizeof(unsigned int));
          outF.write((char*)&anji[n][j][i],sizeof(float));
        }
      }
    }
    return OK;
  }
}

//-------------------------   
bool anjiMatrix::print_maxnsize_data(const char* maxnsizeDataFile)
{
  ofstream outF;
  outF.open(maxnsizeDataFile,ios::out);
  if(!outF)
  {
    cerr<<"Error while printing file with anji maximum size data."<<endl;
    return ERROR;
  }
  else
  {
        // Print maximum size for anji
    outF<<anji_maxnsize<<endl;
    outF<<anji_pointer<<endl;
    
        // Print np to n vector
    for(unsigned int np=0;np<np_to_n_vector.size();++np)
    {
      if(np_to_n_vector[np].first)
        outF<<np<<" "<<np_to_n_vector[np].second<<endl;
    }
    return OK;
  }  
}

//-------------------------   
void anjiMatrix::set(unsigned int n,
                     unsigned int j,
                     unsigned int i,
                     float f)
{
  if(anji_maxnsize>0)
  {
    unsigned int np;
    map_n_in_matrix(n,np);
  
        // Grow in the dimension of np if necessary
    while(anji.size()<=np)
    {
      Vector<Vector<float> > aji;
      anji.push_back(aji);
    }

        // Grow in the dimension of j if necessary
    while(anji[np].size()<=j)
    {
      Vector<float> ai;
      anji[np].push_back(ai);
    }

        // Grow in the dimension of i if necessary
    while(anji[np][j].size()<=i)
    {
      anji[np][j].push_back(INVALID_ANJI_VAL);
    }
        // Set value
    anji[np][j][i]=f;
  }
}

//-------------------------   
float anjiMatrix::get(unsigned int n,
                      unsigned int j,
                      unsigned int i)
{
  unsigned int np;
  if(!n_is_mapped_in_matrix(n,np))
    return INVALID_ANJI_VAL;
  
      // Check boundaries
  if(anji.size()<=np) return INVALID_ANJI_VAL;
  if(anji[np].size()<=j) return INVALID_ANJI_VAL;
  if(anji[np][j].size()<=i) return INVALID_ANJI_VAL;
      // anji[np][j][i] is defined
  return anji[np][j][i];
}

//-------------------------   
float anjiMatrix::get_invp(unsigned int n,
                           unsigned int j,
                           unsigned int i)
{
  float f=get(n,j,i);
  if(f==INVALID_ANJI_VAL) return 0;
  else return f;
}

//-------------------------   
float anjiMatrix::get_invlogp(unsigned int n,
                              unsigned int j,
                              unsigned int i)
{
  float f=get(n,j,i);
  if(f==INVALID_ANJI_VAL) return SMALL_LG_NUM;
  else return f;
}

//-------------------------   
bool anjiMatrix::n_is_mapped_in_matrix(unsigned int n,
                                       unsigned int &np)
{
  if(anji_maxnsize==UNRESTRICTED_ANJI_SIZE)
  {
        // Size of anji is not restricted
    if(n<anji.size())
    {
      np=n;
      return true;
    }
    else
      return false;
  }
  else
  {
        // Size of anji is restricted
    pair<bool,unsigned int> pbui=read_n_to_np_vector(n);
    np=pbui.second;
    return pbui.first;
  }
}

//-------------------------   
void anjiMatrix::map_n_in_matrix(unsigned int n,
                                 unsigned int &np)
{
  if(anji_maxnsize==UNRESTRICTED_ANJI_SIZE)
  {
        // Size of anji is not restricted
    np=n;
  }
  else
  {
        // Size of anji is restricted
    if(!n_is_mapped_in_matrix(n,np))
    {
          // n is not mapped in anji
      
          // Assign index to n
      np=anji_pointer;
      ++anji_pointer;
      if(anji_pointer>=anji_maxnsize)
        anji_pointer=0;

          // Update info for old index
      pair<bool,unsigned int> pbui=read_np_to_n_vector(np);
      if(pbui.first)
      {
            // np'th entry of anji was in use

            // Update old n to np correspondence
        update_n_to_np_vector(pbui.second,make_pair(false,0));
            // Clear anji entry for old index
        anji[np].clear();
      }
      
          // Update np to n mapping
      update_np_to_n_vector(np,make_pair(true,n));
      
          // Update n to np mapping
      update_n_to_np_vector(n,make_pair(true,np));
    }
  }
}

//-------------------------
pair<bool,unsigned int> anjiMatrix::read_np_to_n_vector(unsigned int np)
{
  if(np<np_to_n_vector.size())
  {
    return np_to_n_vector[np];
  }
  else return make_pair(false,0);
}

//-------------------------
pair<bool,unsigned int> anjiMatrix::read_n_to_np_vector(unsigned int n)
{
  if(n<n_to_np_vector.size())
  {
    return n_to_np_vector[n];
  }
  else return make_pair(false,0);  
}

//-------------------------
void anjiMatrix::update_np_to_n_vector(unsigned int np,
                                       pair<bool,unsigned int> pbui)
{
      // grow np_to_n_vector
  while(np>=np_to_n_vector.size())
  {
    np_to_n_vector.push_back(make_pair(false,0));
  }
  np_to_n_vector[np]=pbui;
}

//-------------------------
void anjiMatrix::update_n_to_np_vector(unsigned int n,
                                       pair<bool,unsigned int> pbui)
{
      // grow n_to_np_vector
  while(n>=n_to_np_vector.size())
  {
    n_to_np_vector.push_back(make_pair(false,0));
  }
  n_to_np_vector[n]=pbui;
}

//-------------------------
void anjiMatrix::clear(void)
{
  anji_pointer=0;
  anji.clear();
  np_to_n_vector.clear();
  n_to_np_vector.clear();
}
