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
 
#include "bleu.h"

//---------------
int calc_bleu(const char* ref,
              const char* sys,
              float& bleu,
              float &bp,
              Vector<float>& bleu_n,
              int verbosity)
{
  FILE *reff;
  FILE *sysf;
  int ret;
  
  reff=fopen(ref,"r");
  if(reff==NULL)
  {
    cerr<<"Error while opening file with references: "<<ref<<endl;
    return ERROR;
  }

  sysf=fopen(sys,"r");
  if(sysf==NULL)
  {
    cerr<<"Error while opening file with translations: "<<sys<<endl;
    return ERROR;
  }

  ret=calc_bleuf(reff,sysf,bleu,bp,bleu_n,verbosity);

  fclose(reff);
  fclose(sysf);

  return ret;
}

//---------------
int calc_bleuf(FILE *reff,
               FILE *sysf,
               float& bleu,
               float &bp,
               Vector<float>& bleu_n,
               int verbosity)
{
  awkInputStream refStream;
  awkInputStream sysStream;
  unsigned int numSents=0;
  unsigned int refWords=0;
  unsigned int sysWords=0;
  Vector<float> total;
  Vector<float> precs;
  unsigned int i;
  Vector<std::string> refsen;
  Vector<std::string> syssen;
  
  bleu_n.clear();

  for(i=0;i<MAX_N;++i)
  {
    total.push_back(0);
    precs.push_back(0);
    bleu_n.push_back(0);
  }
  
      // Open files
  if(refStream.open_stream(reff)==ERROR)
  {
    cerr<<"Invalid file pointer to file with references."<<endl;
    return ERROR;
  }  
  if(sysStream.open_stream(sysf)==ERROR)
  {
    cerr<<"Invalid file pointer to file with system translations."<<endl;
    return ERROR;
  }  

  while(refStream.getline())
  {
    unsigned int prec_sent;
    unsigned int total_sent;
    
    bool ok=sysStream.getline();
    if(!ok)
    {
      cerr<<"Unexpected end of system file."<<endl;
      return ERROR;      
    }

    ++numSents;
    refWords+=refStream.NF;
    sysWords+=sysStream.NF;

    if(verbosity) cerr<<numSents<<endl;

        // extract ref sentence
    refsen.clear();
    for(i=1;i<=refStream.NF;++i)
    {
      refsen.push_back(refStream.dollar(i));
      if(verbosity)
      {
        cerr<<refsen[i-1]<<" ";
        if(i==refStream.NF) cerr<<endl;
      }
    }
        // extract translation
    syssen.clear();
    for(i=1;i<=sysStream.NF;++i)
    {
      syssen.push_back(sysStream.dollar(i));
      if(verbosity)
      {
        cerr<<syssen[i-1]<<" ";
        if(i==sysStream.NF) cerr<<endl;
      }
    }

        // calculate precisions
    for(i=1;i<=MAX_N;++i)
    {
      prec_n(refsen,syssen,i,prec_sent,total_sent);
      precs[i-1]+=prec_sent;
      total[i-1]+=total_sent;
      if(verbosity)
      {
        cerr<<prec_sent<<"|"<<precs[i-1]<<" / "<<total_sent<<"|"<<total[i-1]<<" ; ";
      }
    }
    if(verbosity) cerr<<endl;
  }

      // calculate brevity penalty
  if (sysWords<refWords)
  {
    bp=(double)exp((double)1-(double)refWords/sysWords);
  }
  else bp=1;

      // calculate bleu
  if(verbosity) cerr<<"Counts: ";
  bleu=0;
  for(i=1;i<=MAX_N;++i)
  {
    if(total[i-1]==0) bleu_n[i-1]=0;
    else bleu_n[i-1]=(double)precs[i-1]/total[i-1];
    bleu+=((double)1/MAX_N)*(double)my_log((double)bleu_n[i-1]);
    if(verbosity)
    {
      cerr<<precs[i-1]<<","<<total[i-1]<<" ; ";
      if(i==MAX_N) cerr<<endl;
    }
  }
  
  bleu=bp*(double)exp((double)bleu);
  
  if(verbosity)
  {
    cerr<<"#Sentences: "<<numSents<<endl;
    cerr<<"ref. words: "<<refWords<<endl;
    cerr<<"sys. words: "<<sysWords<<endl;
  }
  
  return OK;
}

//---------------
void prec_n(Vector<std::string> refsen,
            Vector<std::string> syssen,
            unsigned int n,
            unsigned int& prec,
            unsigned int& total)
{
  unsigned int i;
  unsigned int j;
  unsigned int reftotal;
  Vector<bool> matched;
    
  if(n>syssen.size()) total=0;
  else total=syssen.size()-n+1;

  if(n>refsen.size()) reftotal=0;
  else reftotal=refsen.size()-n+1;

  for(i=0;i<reftotal;++i)
  {
    matched.push_back(false);
  }

  prec=0;
  for(i=0;i<total;++i)
  {
    for(j=0;j<reftotal;++j)
    {
      bool match=true;
      for(unsigned int k=0;k<n;++k)
      {
        if(syssen[i+k]!=refsen[j+k])
        {
          match=false;
          break;
        }
      }
      if(match && !matched[j])
      {
        ++prec;
        matched[j]=true;
        break;
      }
    }
  }
}

//---------------
double my_log(double x)
{
  if(x==0) return -999999999;
  else return log(x);
}
