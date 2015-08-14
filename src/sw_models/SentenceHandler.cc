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
/* Module: SentenceHandler                                          */
/*                                                                  */
/* Definitions file: SentenceHandler.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "SentenceHandler.h"

//--------------- Global variables -----------------------------------


//--------------- Function declarations 


//--------------- Constants


//--------------- Classes --------------------------------------------


//--------------- SentenceHandler class function definitions

//-------------------------
SentenceHandler::SentenceHandler(void)
{
}

//-------------------------
bool SentenceHandler::readSentencePairs(const char *srcFileName,
                                        const char *trgFileName,
                                        const char *sentCountsFile,
                                        pair<unsigned int,unsigned int>& sentRange)
{
      // Clear sentence handler
 cerr<<"Warning: the previous content of the sentence handler will be removed after calling the readSentencePairs() function"<<endl;
 clear();

      // Define variables
 unsigned int i;
 awkInputStream awkSrc;
 awkInputStream awkTrg;
 awkInputStream awkSrcTrgC;
 Vector<std::string> srcSentStr;
 Vector<std::string> trgSentStr;

     // Fill first field of sentRange
 sentRange.first=sentPairCont.size();

     // Open source file
 if(awkSrc.open(srcFileName)==ERROR)
 {
   cerr<<"Error in source language file: "<<srcFileName<<endl;
   return ERROR;
 }
 else
 {
       // Open target file
   if(awkTrg.open(trgFileName)==ERROR)
   {
     cerr<<"Error in target language file: "<<trgFileName<<endl;
     return ERROR;
   }
   else
   {
         // Open file with sentence counts
     bool countFileExists;
     if(strlen(sentCountsFile)==0)
     {
           // sentCountsFile is empty
       countFileExists=false;
     }
     else
     {
           // sentCountsFile is not empty
       if(awkSrcTrgC.open(sentCountsFile)==ERROR)
       {
         cerr<<"File with sentence counts "<<sentCountsFile<<" does not exist"<<endl;
         countFileExists=false;
       }
       else
         countFileExists=true;
     }
     
         // Read sentence pairs
     cerr<<"Reading sentence pairs from files: "<<srcFileName<<" and "<<trgFileName<<endl;
     if(countFileExists) cerr<<"Reading sentence pair counts from file "<<sentCountsFile<<endl;
     
     pair<unsigned int,unsigned int> sentRangeAux;

     Count nsPairs=0;
     Count runningSrcWords=0;
     Count runningTrgWords=0;
     
     while(awkSrc.getln())
     {
       srcSentStr.clear();
       trgSentStr.clear();
					   
       awkTrg.getln();
       for(i=1;i<=awkSrc.NF;++i) 
       {
         srcSentStr.push_back(awkSrc.dollar(i));
       }
       for(i=1;i<=awkTrg.NF;++i)
       {
         trgSentStr.push_back(awkTrg.dollar(i));
       }
       if(countFileExists)
       {
         awkSrcTrgC.getln();
         Count c=atof(awkSrcTrgC.dollar(1).c_str());
         addSentPair(srcSentStr,trgSentStr,c,sentRangeAux);
             // Update statistics
         nsPairs+=c;
         runningSrcWords+=c*(double)srcSentStr.size();
         runningTrgWords+=c*(double)trgSentStr.size();
       }
       else
       {
         addSentPair(srcSentStr,trgSentStr,1,sentRangeAux);
             // Update statistics
         nsPairs+=1;
         runningSrcWords+=srcSentStr.size();
         runningTrgWords+=trgSentStr.size();
       }
     }
         // Print statistics
     cerr<<"#Sentence pairs: "<<nsPairs<<endl;
     cerr<<"#Running source words: "<<runningSrcWords<<endl;
     cerr<<"#Running target words: "<<runningTrgWords<<endl;
   }
       // Fill second field of sentRange
   sentRange.second=sentPairCont.size()-1;

   return OK;	
 }
}

//-------------------------
void SentenceHandler::addSentPair(Vector<std::string> srcSentStr,
                                  Vector<std::string> trgSentStr,
                                  Count c,
                                  pair<unsigned int,unsigned int>& sentRange)
{
      // Fill sentRange information
  sentRange.first=sentPairCont.size();
  sentRange.second=sentRange.first;
      // add to sentPairCont
  sentPairCont.push_back(make_pair(srcSentStr,trgSentStr));
      // add to sentPairCount
  sentPairCount.push_back(c);

      // Display warnings if sentences are empty
  if(srcSentStr.empty())
    cerr<<"Warning: source sentence "<<sentRange.first<<" is empty"<<endl;
  if(trgSentStr.empty())
    cerr<<"Warning: target sentence "<<sentRange.first<<" is empty"<<endl;
}

//-------------------------
unsigned int SentenceHandler::numSentPairs(void)
{
  return sentPairCont.size();
}

//-------------------------
int SentenceHandler::nthSentPair(unsigned int n,
                                 Vector<std::string>& srcSentStr,
                                 Vector<std::string>& trgSentStr,
                                 Count& c)
{
  if(n>=numSentPairs())
    return ERROR;
  else
  {
    srcSentStr=sentPairCont[n].first;

    trgSentStr=sentPairCont[n].second;
    
    c=sentPairCount[n];
    
    return OK;
  }
}

//-------------------------
int SentenceHandler::getSrcSent(unsigned int n,
                                Vector<std::string>& srcSentStr)
{
  if(n>=numSentPairs())
    return ERROR;
  else
  {
    srcSentStr=sentPairCont[n].first;

    return OK;
  }
  
}

//-------------------------
int SentenceHandler::getTrgSent(unsigned int n,
                                Vector<std::string>& trgSentStr)
{
  if(n>=numSentPairs())
    return ERROR;
  else
  {
    trgSentStr=sentPairCont[n].second;

    return OK;
  }  
}

//-------------------------
int SentenceHandler::getCount(unsigned int n,
                              Count& c)
{
  if(n>=numSentPairs())
    return ERROR;
  else
  {
    c=sentPairCount[n];
    
    return OK;
  }  
}

//-------------------------
bool SentenceHandler::printSentPairs(const char *srcSentFile,
                                     const char *trgSentFile,
                                     const char *sentCountsFile)
{
  ofstream srcOutF;
  ofstream trgOutF;
  ofstream countsOutF;

      // Open file with source sentences
  srcOutF.open(srcSentFile,ios::out);
  if(!srcOutF)
  {
    cerr<<"Error while printing file with source sentences."<<endl;
    return ERROR;
  }

      // Open file with target sentences
  trgOutF.open(trgSentFile,ios::out);
  if(!trgOutF)
  {
    cerr<<"Error while printing file with target sentences."<<endl;
    return ERROR;
  }

      // Open file with sentence counts
  countsOutF.open(sentCountsFile,ios::out);
  if(!countsOutF)
  {
    cerr<<"Error while printing file with sentence counts."<<endl;
    return ERROR;
  }

      // Print sentence pair with its count
  for(unsigned int i=0;i<sentPairCont.size();++i)
  {
        // print source sentence
    for(unsigned int j=0;j<sentPairCont[i].first.size();++j)
    {
      srcOutF<<sentPairCont[i].first[j];
      if(j<sentPairCont[i].first.size()-1) srcOutF<<" ";
    }
    srcOutF<<endl;
    
        // print target sentence
    for(unsigned int j=0;j<sentPairCont[i].second.size();++j)
    {
      trgOutF<<sentPairCont[i].second[j];
      if(j<sentPairCont[i].second.size()-1) trgOutF<<" ";
    }
    trgOutF<<endl;

        // print count
    countsOutF<<sentPairCount[i]<<endl;
  }
      // Close output streams
  srcOutF.close();
  trgOutF.close();
  countsOutF.close();
  
  return OK;
}

//-------------------------
void SentenceHandler::clear(void)
{
  sentPairCont.clear();
  sentPairCount.clear();
}
