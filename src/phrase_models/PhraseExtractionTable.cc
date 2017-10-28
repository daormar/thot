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
/* Module: PhraseExtractionTable                                    */
/*                                                                  */
/* Definitions file: PhraseExtractionTable.cc                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhraseExtractionTable.h"
#include <Trie.h>

//--------------- Constants ------------------------------------------
#define DEFAULT_RANDOM_SEED          31415
#define NUM_RAND_WALKS              100000
#define SENT_LENGTH_LIMIT_EXACT_SRF    100

//--------------- Function definitions

//-------------------------
PhraseExtractionTable::PhraseExtractionTable(void)
{
  pruneWasApplied=false;
  numSegmentationPrunings=0;

      // Make room for pecMatrix
  PhraseExtractionCell pec;
  std::vector<PhraseExtractionCell> pecVec;
  pecVec.insert(pecVec.begin(),MAX_SENTENCE_LENGTH,pec);
  pecMatrix.insert(pecMatrix.begin(),MAX_SENTENCE_LENGTH,pecVec);

      // Make room for iterMatrix
  iterator iter;
  std::vector<iterator> iterVec;
  iterVec.insert(iterVec.begin(),MAX_SENTENCE_LENGTH,iter);
  iterMatrix.insert(iterMatrix.begin(),MAX_SENTENCE_LENGTH,iterVec);
}

//-------------------------
PhraseExtractionTable& PhraseExtractionTable::operator=(const PhraseExtractionTable &right)
{
  unsigned int i,j;
 
  for(i=0;i<tlen;++i)
    for(j=0;j<tlen;++j)
    {
      pecMatrix[i][j]=right.pecMatrix[i][j];
    }
  pruneWasApplied=right.pruneWasApplied;
  numSegmentationPrunings=right.numSegmentationPrunings;
  return *this;
}

//-------------------------
PhraseExtractionCell& PhraseExtractionTable::cell(unsigned int x, unsigned int y)
{
  if(x<tlen && y<tlen) return pecMatrix[x][y];	
  else return pecMatrix[0][0];	
}

//-------------------------
void PhraseExtractionTable::extractConsistentPhrases(PhraseExtractParameters phePars,
                                                     const std::vector<std::string> &_ns,
                                                     const std::vector<std::string> &_t,
                                                     const WordAligMatrix &_alig,
                                                     std::vector<PhrasePair>& outvph)
{
#ifdef USE_OCH_PHRASE_EXTRACT
  extractConsistentPhrasesOch(phePars,_ns,_t,_alig,outvph);
#else
  extractConsistentPhrasesOld(phePars,_ns,_t,_alig,outvph);
#endif
}

//-------------------------
void PhraseExtractionTable::extractConsistentPhrasesOld(PhraseExtractParameters phePars,
                                                        const std::vector<std::string> &_ns,
                                                        const std::vector<std::string> &_t,
                                                        const WordAligMatrix &_alig,
                                                        std::vector<PhrasePair>& outvph)
{ 
 if(_t.size()>=MAX_SENTENCE_LENGTH || _ns.size()-1>=MAX_SENTENCE_LENGTH)
 {
   std::cerr<<"Warning! the sentences are too long."<<std::endl;
 }
 else
 {
   ns=_ns;
   t=_t;
   alig=_alig;	
   tlen=t.size(); nslen=ns.size();
   maxTrgPhraseLength=phePars.maxTrgPhraseLength;
   if(!phePars.constraintSrcLen) maxSrcPhraseLength=MAX_SENTENCE_LENGTH;
   else maxSrcPhraseLength=maxTrgPhraseLength;
   countSpurious=phePars.countSpurious;
   monotone=phePars.monotone;
   
   obtainConsistentPhrases();
   createVectorWithConsPhrases(outvph);
 }
}

//-------------------------
void PhraseExtractionTable::extractConsistentPhrasesOch(PhraseExtractParameters phePars,
                                                        const std::vector<std::string> &_ns,
                                                        const std::vector<std::string> &_t,
                                                        const WordAligMatrix &_alig,
                                                        std::vector<PhrasePair>& outvph)
{
 if(_t.size()>=MAX_SENTENCE_LENGTH || _ns.size()-1>=MAX_SENTENCE_LENGTH)
 {
   std::cerr<<"Warning! the sentences are too long."<<std::endl;
 }
 else
 {
       // Init data members
   ns=_ns;
   t=_t;
   alig=_alig;	
   PositionIndex tlen=t.size();
   PositionIndex slen=ns.size()-1;
   maxTrgPhraseLength=phePars.maxTrgPhraseLength;

       // Extract set of consistent phrase pairs
   for(PositionIndex i1=1;i1<=slen;++i1)
   {
     for(PositionIndex i2=i1;i2<=slen;++i2)
     {
           // Obtain SP
       std::vector<PositionIndex> SP;
       for(unsigned int j=1;j<=tlen;++j)
       {
         for(unsigned int k=i1;k<=i2;++k)
         {
           if(alig.getValue(k-1,j-1))
           {
             SP.push_back(j);
             break;
           }
         }
       }
       
           // Check if SP only contains consecutive positions
       bool consec=true;
       if(SP.empty()) consec=false;
       else
       {
         if(SP.size()>1)
         {
           for(unsigned int k=0;k<SP.size()-1;++k)
           {
             if(SP[k]!=SP[k+1]-1)
             {
               for(PositionIndex jp=SP[k]+1;jp<SP[k+1];++jp)
               {
                 if(!alig.jAligned(jp))
                 {
                   consec=false;
                   break;
                 }
               }
               if(!consec) break;
             }
           }
         }
       }
//        std::cerr<<"------"<<std::endl;
//        std::cerr<<i1<<" "<<i2<<std::endl;
//        std::cerr<<"SP: ";
//        for(unsigned int k=0;k<SP.size();++k) std::cerr<<SP[k]<<" ";
//        std::cerr<<std::endl;
//        std::cerr<<"consec: "<<consec<<std::endl;
//        std::cerr<<"******"<<std::endl;
       
       if(consec)
       {
             // SP only contains consecutive positions
         PositionIndex j1=SP.front();
         PositionIndex j2=SP.back();

             // Obtain TP
         std::vector<PositionIndex> TP;
         for(unsigned int i=1;i<=slen;++i)
         {
           for(unsigned int k=j1;k<=j2;++k)
           {
             if(alig.getValue(i-1,k-1))
             {
               TP.push_back(i);
               break;
             }
           }
         }

//          std::cerr<<j1<<" "<<j2<<std::endl;
//          std::cerr<<"TP: ";
//          for(unsigned int k=0;k<TP.size();++k) std::cerr<<TP[k]<<" ";
//          std::cerr<<std::endl;
//          std::cerr<<"++++++"<<std::endl;
         
             // Check if TP is in [i1,..,i2]
         if(!TP.empty() && TP.front()>=i1 && TP.back()<=i2)
         {
           PositionIndex jp=j1;
           while(jp==j1 || (jp> 0 && !alig.jAligned(jp-1)))
           {
             PositionIndex jpp=j2;
             while(jpp==j2 || (jpp<=tlen && !alig.jAligned(jpp-1)))
             {
               PhrasePair ppair;
               ppair.weight=1;
               for(PositionIndex k=i1;k<=i2;++k)
                 ppair.s_.push_back(ns[k]);
               for(PositionIndex k=jp;k<=jpp;++k)
                 ppair.t_.push_back(t[k-1]);
               if(ppair.t_.size()<=(unsigned int)maxTrgPhraseLength)
                 outvph.push_back(ppair);

//                std::cerr<<jp<<" "<<jpp<<std::endl;
//                for(unsigned int k=0;k<ppair.s_.size();++k)
//                  std::cerr<<ppair.s_[k]<<" ";
//                std::cerr<<std::endl;

//                for(unsigned int k=0;k<ppair.t_.size();++k)
//                  std::cerr<<ppair.t_[k]<<" ";
//                std::cerr<<std::endl;

               jpp+=1;
             }
             jp-=1;
           }
         }
       }
     }
   }
 }  
}

//-------------------------
double PhraseExtractionTable::segmBasedExtraction(PhraseExtractParameters phePars,
                                                  const std::vector<std::string> &_ns,
                                                  const std::vector<std::string> &_t,
                                                  const WordAligMatrix &_alig,
                                                  std::vector<PhrasePair>& outvph,
                                                  int verbose/*=0*/)
{
 if(_t.size()>=MAX_SENTENCE_LENGTH || _ns.size()-1>=MAX_SENTENCE_LENGTH)
 {
   std::cerr<<"Warning! the sentences are too long."<<std::endl;
   return 0;
 }
 else
 {
   ns=_ns;
   t=_t;
   alig=_alig;	
   tlen=t.size(); nslen=ns.size();
   maxTrgPhraseLength=phePars.maxTrgPhraseLength;
   if(!phePars.constraintSrcLen) maxSrcPhraseLength=MAX_SENTENCE_LENGTH;
   else maxSrcPhraseLength=maxTrgPhraseLength;
   countSpurious=phePars.countSpurious;
   monotone=phePars.monotone;
   
   obtainConsistentPhrases();
   BpSet bpSet;
   obtainBpSet(bpSet);
   BpSet C;

   double logNumSegms;
   if(t.size()<=SENT_LENGTH_LIMIT_EXACT_SRF &&
      ns.size()-1<=SENT_LENGTH_LIMIT_EXACT_SRF)
   {
//   logNumSegms=srfPhraseExtract(bpSet,C);
     logNumSegms=srfPhraseExtractDp(bpSet,C,verbose);
   }
   else
   {
     logNumSegms=approxSrfPhraseExtract(bpSet,C,verbose);
   }
   
   obtainPhrPairVecFromBpSet(C,outvph,logNumSegms);
   
   return logNumSegms;
 }
}

//-------------------------
double PhraseExtractionTable::srfPhraseExtract(const BpSet& bpSet,
                                                     BpSet& C)
{
  Bitset<MAX_SENTENCE_LENGTH> SP((unsigned int)0);
  for(unsigned int x=1;x<=nslen-1;++x) SP.set(x,1);
  Bitset<MAX_SENTENCE_LENGTH> TP((unsigned int)0);
  for(unsigned int y=1;y<=tlen;++y) TP.set(y,1);
  C.clear();
  double logNumSegms=srfPhraseExtractRec(bpSet,SP,TP,C);
  return logNumSegms;
}

//-------------------------
double PhraseExtractionTable::srfPhraseExtractRec(const BpSet& bpSet,
                                                  const Bitset<MAX_SENTENCE_LENGTH>& SP,
                                                  const Bitset<MAX_SENTENCE_LENGTH>& TP,
                                                  BpSet& C)
{
  Bitset<MAX_SENTENCE_LENGTH> b((unsigned int) 0);
  if(SP==b)
  {
        // All source positions have been covered
    if(TP==b) return 0;
    else return SMALL_LG_NUM;
  }
  else
  {
        // There are source positions to be covered
    float totalLogNumSegms=SMALL_LG_NUM;
    unsigned int x1=MAX_SENTENCE_LENGTH+1;
    unsigned int x2=0;

        // Obtain min and max source positions to be covered
    for(unsigned int k=1;k<=nslen-1;++k)
    {
      if(SP.test(k))
      {
        if(x1>k) x1=k;
        if(x2<k) x2=k;
      }
    }

        // Iterate over all possible source segments to be covered
    for(unsigned int x=x1;x<=x2;++x)
    {
          // Obtain target segments for source segments contained in BP
      BpSetInfoCont trgPhrases;
      bpSet.retrieveTrgPhrasesGivenSrc(x1,x,trgPhrases);
      for(BpSetInfoCont::iterator bsicIter=trgPhrases.begin();bsicIter!=trgPhrases.end();++bsicIter)
      {
            // Mark source positions as covered
        Bitset<MAX_SENTENCE_LENGTH> SP_AUX=SP;
        for(unsigned int k=x1;k<=x;++k) SP_AUX.reset(k);
            // Initialize y1 and y2
        PositionIndex y1=bsicIter->tpair.first;
        PositionIndex y2=bsicIter->tpair.second;
            // Mark target positions as covered
        Bitset<MAX_SENTENCE_LENGTH> TP_AUX=TP;
        bool segmOk=true;
        for(unsigned int k=y1;k<=y2;++k)
        {
          if(TP_AUX.test(k)==0)
          {
            segmOk=false;
            break;
          }
          TP_AUX.reset(k);
        }
        if(segmOk)
        {
              // Recursive invocation
          double logNumSegms=srfPhraseExtractRec(bpSet,SP_AUX,TP_AUX,C);
              // Revise counts
          if(logNumSegms>=0)
          {
            C.incrPair(x1,x,y1,y2,logNumSegms);
            totalLogNumSegms=MathFuncs::lns_sumlog(totalLogNumSegms,logNumSegms);
          }
        }
      }
    }
    return totalLogNumSegms;
  }
}

//-------------------------
double PhraseExtractionTable::srfPhraseExtractDp(const BpSet& bpSet,
                                                 BpSet& C,
                                                 int verbose/*=false*/)
{
  SrfNodeInfoMap sniMap;
  fillSrfNodeInfoMap(bpSet,sniMap);
  if(verbose)
    std::cerr<<"Num. equiv. classes: "<<sniMap.size()<<" ; Equiv. classes with leafs: "<<sniMap.numNodesWithLeafs()<<std::endl;
  
  Bitset<MAX_SENTENCE_LENGTH> SP((unsigned int)0);
  for(unsigned int x=1;x<=nslen-1;++x) SP.set(x,1);
  Bitset<MAX_SENTENCE_LENGTH> TP((unsigned int)0);
  for(unsigned int y=1;y<=tlen;++y) TP.set(y,1);
  SrfNodeKey snk;
  snk.srcCuts=SP;
  snk.trgCuts=TP;
  bool found;
  SrfNodeInfo sni=sniMap.find(snk,found);
  C=sni.C;
  return sni.estLogNumLeafs;
}

//-------------------------
double PhraseExtractionTable::approxSrfPhraseExtract(const BpSet& bpSet,
                                                     BpSet& C,
                                                     int verbose/*=false*/)
{
      // Set random seed
  srand(DEFAULT_RANDOM_SEED);

  if(verbose)
    std::cerr<<"Performing approximate srf phrase extract"<<std::endl;
  
      // Obtain bisegmentation length info
  if(verbose)
    std::cerr<<"Obtaining bisegmentation length info..."<<std::endl;
  SrfNodeInfoMap sniMap;
  fillSrfNodeInfoMap(bpSet,sniMap,false);
  if(verbose)
    std::cerr<<"#Equivalence classes: "<<sniMap.size()<<std::endl;
  
      // Execute random walks
  if(verbose)
    std::cerr<<"Executing random walks..."<<std::endl;
  double logNumSegms;
  logNumSegms=bisegmRandWalk(bpSet,sniMap,C);
  
  return logNumSegms;
}

//-------------------------
void PhraseExtractionTable::fillSrfNodeInfoMap(const BpSet& bpSet,
                                               SrfNodeInfoMap& sniMap,
                                               bool calcCSet/*=true*/)
{
  Bitset<MAX_SENTENCE_LENGTH> SP((unsigned int)0);
  for(unsigned int x=1;x<=nslen-1;++x) SP.set(x,1);
  Bitset<MAX_SENTENCE_LENGTH> TP((unsigned int)0);
  for(unsigned int y=1;y<=tlen;++y) TP.set(y,1);
  sniMap.clear();
  
  fillSrfNodeInfoMapRec(bpSet,SP,TP,sniMap,calcCSet);
}

//-------------------------
void PhraseExtractionTable::fillSrfNodeInfoMapRec(const BpSet& bpSet,
                                                  const Bitset<MAX_SENTENCE_LENGTH>& SP,
                                                  const Bitset<MAX_SENTENCE_LENGTH>& TP,
                                                  SrfNodeInfoMap& sniMap,
                                                  bool calcCSet/*=true*/)
{
  Bitset<MAX_SENTENCE_LENGTH> b((unsigned int) 0);
  if(SP==b)
  {
        // All source positions have been covered
    if(TP==b)
    {
      SrfNodeInfo sni;
      sni.estLogNumLeafs=0;
      SrfNodeKey snk;
      snk.srcCuts=SP;
      snk.trgCuts=TP;
      sniMap.insert(snk,sni);
    }
    else
    {
      SrfNodeInfo sni;
      sni.estLogNumLeafs=SMALL_LG_NUM;
      SrfNodeKey snk;
      snk.srcCuts=SP;
      snk.trgCuts=TP;
      sniMap.insert(snk,sni);
    }
  }
  else
  {
        // Initialize info for current key
    SrfNodeInfo sni;
    sni.estLogNumLeafs=SMALL_LG_NUM;
    SrfNodeKey snk;
    snk.srcCuts=SP;
    snk.trgCuts=TP;
    sniMap.insert(snk,sni);

        // There are source positions to be covered
    unsigned int x1=MAX_SENTENCE_LENGTH+1;
    unsigned int x2=0;

        // Obtain min and max source positions to be covered
    for(unsigned int k=1;k<=nslen-1;++k)
    {
      if(SP.test(k))
      {
        if(x1>k) x1=k;
        if(x2<k) x2=k;
      }
    }

        // Iterate over all possible source segments to be covered
    for(unsigned int x=x1;x<=x2;++x)
    {
          // Obtain target segments for source segments contained in BP
      BpSetInfoCont trgPhrases;
      bpSet.retrieveTrgPhrasesGivenSrc(x1,x,trgPhrases);

      for(BpSetInfoCont::iterator bsicIter=trgPhrases.begin();bsicIter!=trgPhrases.end();++bsicIter)
      {
            // Mark source positions as covered
        Bitset<MAX_SENTENCE_LENGTH> SP_AUX=SP;
        for(unsigned int k=x1;k<=x;++k) SP_AUX.reset(k);
            // Initialize y1 and y2
        PositionIndex y1=bsicIter->tpair.first;
        PositionIndex y2=bsicIter->tpair.second;
            // Mark target positions as covered
        Bitset<MAX_SENTENCE_LENGTH> TP_AUX=TP;
        bool segmOk=true;
        for(unsigned int k=y1;k<=y2;++k)
        {
          if(TP_AUX.test(k)==0)
          {
            segmOk=false;
            break;
          }
          TP_AUX.reset(k);
        }
        if(segmOk)
        {
               // Create new snk object
          SrfNodeKey newSnk;
          newSnk.srcCuts=SP_AUX;
          newSnk.trgCuts=TP_AUX;
             
              // Check if the new node has already been explored
          bool found;
          SrfNodeInfo sniNewSnk=sniMap.find(newSnk,found);

          if(!found)
          {
                // Equivalence class not present
              
                // Recursive call
            fillSrfNodeInfoMapRec(bpSet,
                                  SP_AUX,
                                  TP_AUX,
                                  sniMap);
              
                // Obtain info for the new node
            sniNewSnk=sniMap.find(newSnk,found);

            if(sniNewSnk.estLogNumLeafs>=0)
            {
                  // Update info
              SrfNodeInfo sni=sniMap.find(snk,found);

                  // Update number of leafs info
              sni.estLogNumLeafs=MathFuncs::lns_sumlog(sni.estLogNumLeafs,sniNewSnk.estLogNumLeafs);
                
                  // Update phrase counts info
              if(calcCSet)
              {
                sni.C.incrPair(x1,x,y1,y2,sniNewSnk.estLogNumLeafs);
                sni.C.obtainUnion(sniNewSnk.C);
              }
                
                  // Insert info
              sniMap.insert(snk,sni);
            }
          }
          else
          {
                // Equivalence class present
            if(sniNewSnk.estLogNumLeafs>=0)
            {              
                  // Update info
              SrfNodeInfo sni=sniMap.find(snk,found);

                  // Update number of leafs info
              sni.estLogNumLeafs=MathFuncs::lns_sumlog(sni.estLogNumLeafs,sniNewSnk.estLogNumLeafs);
                
                  // Update phrase counts info
              if(calcCSet)
              {
                sni.C.incrPair(x1,x,y1,y2,sniNewSnk.estLogNumLeafs);
                sni.C.obtainUnion(sniNewSnk.C);
              }

                  // Insert info
              sniMap.insert(snk,sni);
            }
          }
        }
      }
    }    
  }
}

//-------------------------
double PhraseExtractionTable::bisegmRandWalk(const BpSet& bpSet,
                                             const SrfNodeInfoMap& sniMap,
                                             BpSet& C)
{
  C.clear();
  Bitset<MAX_SENTENCE_LENGTH> SP((unsigned int)0);
  for(unsigned int x=1;x<=nslen-1;++x) SP.set(x,1);
  Bitset<MAX_SENTENCE_LENGTH> TP((unsigned int)0);
  for(unsigned int y=1;y<=tlen;++y) TP.set(y,1);
  SrfBisegm sb;
  SrfBisegm result;

  unsigned int numSuccess=0;
  for(unsigned int n=0;n<NUM_RAND_WALKS;++n)
  {
    bool success=bisegmRandWalkRec(bpSet,SP,TP,sb,sniMap,result);
    if(success)
    {
      ++numSuccess;
      for(unsigned int i=0;i<result.srcCuts.size();++i)
      {
        PositionIndex x1;
        if(i==0) x1=1;
        else x1=result.srcCuts[i-1]+1;
        PositionIndex x2=result.srcCuts[i];
        PositionIndex y1=result.trgCuts[i].first;
        PositionIndex y2=result.trgCuts[i].second;
        C.incrPair(x1,x2,y1,y2,0);
      }
    }
  }
  return log((double)numSuccess);
}

//-------------------------
bool PhraseExtractionTable::bisegmRandWalkRec(const BpSet& bpSet,
                                              const Bitset<MAX_SENTENCE_LENGTH>& SP,
                                              const Bitset<MAX_SENTENCE_LENGTH>& TP,
                                              const SrfBisegm& sb,
                                              const SrfNodeInfoMap& sniMap,
                                              SrfBisegm& result)
{
  Bitset<MAX_SENTENCE_LENGTH> b((unsigned int) 0);
  if(SP==b)
  {
        // All source positions have been covered
    if(TP==b)
    {
      result=sb;
      return true;
    }
    else return false;
  }
  else
  {
        // There are source positions to be covered
    unsigned int x1=MAX_SENTENCE_LENGTH+1;
    unsigned int x2=0;

        // Obtain min and max source positions to be covered
    for(unsigned int k=1;k<=nslen-1;++k)
    {
      if(SP.test(k))
      {
        if(x1>k) x1=k;
        if(x2<k) x2=k;
      }
    }

        // Iterate over all possible source segments to be covered
    std::vector<std::pair<SrfNodeKey,SrfNodeInfo> > nextSegmStepVec;
    std::vector<SrfBisegm> nextSbVec;
    std::vector<Bitset<MAX_SENTENCE_LENGTH> > srcPosVec;
    std::vector<Bitset<MAX_SENTENCE_LENGTH> > trgPosVec;
    
    for(unsigned int x=x1;x<=x2;++x)
    {
          // Obtain target segments for source segments contained in BP
      BpSetInfoCont trgPhrases;
      bpSet.retrieveTrgPhrasesGivenSrc(x1,x,trgPhrases);
      for(BpSetInfoCont::iterator bsicIter=trgPhrases.begin();bsicIter!=trgPhrases.end();++bsicIter)
      {
            // Mark source positions as covered
        Bitset<MAX_SENTENCE_LENGTH> SP_AUX=SP;
        for(unsigned int k=x1;k<=x;++k) SP_AUX.reset(k);
            // Initialize y1 and y2
        PositionIndex y1=bsicIter->tpair.first;
        PositionIndex y2=bsicIter->tpair.second;
            // Mark target positions as covered
        Bitset<MAX_SENTENCE_LENGTH> TP_AUX=TP;
        bool segmOk=true;
        for(unsigned int k=y1;k<=y2;++k)
        {
          if(TP_AUX.test(k)==0)
          {
            segmOk=false;
            break;
          }
          TP_AUX.reset(k);
        }
        if(segmOk)
        {
              // Create new snk object
          SrfNodeKey newSnk;
          newSnk.srcCuts=SP_AUX;
          newSnk.trgCuts=TP_AUX;

              // Store info about new node
          bool found;
          SrfNodeInfo sni=sniMap.find(newSnk,found);
          if(sni.estLogNumLeafs==UNINIT_NUM_LEAFS)
            sni.estLogNumLeafs=SMALL_LG_NUM;
          if(sni.estLogNumLeafs>=0)
          {
                // Store set of source positions
            srcPosVec.push_back(SP_AUX);
                // Store set of target positions
            trgPosVec.push_back(TP_AUX);
                // Store new snk object
            nextSegmStepVec.push_back(std::make_pair(newSnk,sni));
                // Store new sb object
            SrfBisegm newSb;
            newSb.srcCuts=sb.srcCuts;
            newSb.srcCuts.push_back(x);
            newSb.trgCuts=sb.trgCuts;
            newSb.trgCuts.push_back(std::make_pair(y1,y2));

            nextSbVec.push_back(newSb);
          }
        }
      }
    }
        // Randomly select next segmentation step
    unsigned int numNodes=nextSegmStepVec.size();
    if(numNodes>0)
    {
          // There is one or more candidate segmentation steps

          // Obtain sum of estimated number of leafs
      double estLogNumLeafsSum=SMALL_LG_NUM;
      for(unsigned int i=0;i<nextSegmStepVec.size();++i)
      {
        double estLogNumLeafsForNode=nextSegmStepVec[i].second.estLogNumLeafs;
        if(estLogNumLeafsForNode>=0)
          estLogNumLeafsSum=MathFuncs::lns_sumlog(estLogNumLeafsSum,estLogNumLeafsForNode);
      }

          // Return false if there are no estimated leafs
      if(estLogNumLeafsSum==SMALL_LG_NUM) return false;
      
          // Determine random index
      double zerooneRandNum=gen01RandNum();
      unsigned int rand_idx=1;
      double acumEstLogNumLeafs=SMALL_LG_NUM;
      for(unsigned int i=0;i<nextSegmStepVec.size();++i)
      {
        acumEstLogNumLeafs=MathFuncs::lns_sumlog(acumEstLogNumLeafs,nextSegmStepVec[i].second.estLogNumLeafs);
        double acumWeight=exp(acumEstLogNumLeafs-estLogNumLeafsSum);
        if(zerooneRandNum<=acumWeight)
        {
          rand_idx=i;
          break;
        }
      }
      
          // Recursive call
      return bisegmRandWalkRec(bpSet,
                               srcPosVec[rand_idx],
                               trgPosVec[rand_idx],
                               nextSbVec[rand_idx],
                               sniMap,
                               result);
    }
    else
    {
      return false;
    }
  }
}

//-------------------------
void PhraseExtractionTable::obtainPhrPairVecFromBpSet(const BpSet& bpSet,
                                                      std::vector<PhrasePair>& outvph,
                                                      double logNumSegms/*=1*/)
{
  outvph.clear();
  PositionIndex x1max=bpSet.getx1Max();
  for(PositionIndex x1=1;x1<=x1max;++x1)
  {
    PositionIndex x2max=bpSet.getx2Max(x1);
    for(PositionIndex x2=1;x2<=x2max;++x2)
    {
      BpSetInfoCont bsic;
      bpSet.retrieveTrgPhrasesGivenSrc(x1,x2,bsic);
      for(BpSetInfoCont::iterator bsicIter=bsic.begin();bsicIter!=bsic.end();++bsicIter)
      {
        PhrasePair pp;
        for(unsigned int i=x1;i<=x2;++i)
          pp.s_.push_back(ns[i]);
        for(unsigned int j=bsicIter->tpair.first;j<=bsicIter->tpair.second;++j)
          pp.t_.push_back(t[j-1]);
        pp.weight=exp(bsicIter->lc-logNumSegms);
        outvph.push_back(pp);
      }
    }
  }
}

//-------------------------
double PhraseExtractionTable::gen01RandNum(void)
{
  return (double)rand()/RAND_MAX;
}

//-------------------------
void PhraseExtractionTable::obtainConsistentPhrases(void)
{
 unsigned int i,leftmost_i,rightmost_i,trgPhraseLength;
 CellAlignment calig,calig_aux;
	
 zFertBitset=zeroFertBitset(alig);
 spurBitset=spuriousWordsBitset(alig);
 
 clear();
 
 // Fill the extraction table (explore each cell of the table)
 for(unsigned int y=0;y<tlen;++y)
 {
   for(unsigned int x=tlen-y-1;x<tlen;++x)
   {
         //std::cerr<<"- "<<x<<" "<<y<<" "<<pecMatrix[x][y].cellAligs.size()<<std::endl;
         // Obtain the length of the source phrase
     if(countSpurious) trgPhraseLength=trgPhraseLengthInCell(x,y);
     else trgPhraseLength=trgPhraseLengthInCellNonSpurious(x,y,spurBitset);
     
     if(trgPhraseLength<=(unsigned int)maxTrgPhraseLength)
     {
       if(x==tlen-y-1)
       {//process extern cell
         calig.coverage.reset();
         rightmost_i=0; leftmost_i=nslen;
         for(i=0;i<nslen-1;++i) 
           if(alig.getValue(i,y)!=0)
           {
             calig.coverage.set(i+1);
             if(leftmost_i>i+1)leftmost_i=i+1;
             if(rightmost_i<i+1)rightmost_i=i+1;
           }
				  						 
         for(i=1;i<nslen;++i) 
           if(zFertBitset.test(i)==1 && (i> leftmost_i && i<rightmost_i)) calig.coverage.set(i);
                         
         if(validCoverageForCell(calig.coverage,x,y) && trgPhraseLengthInCellNonSpurious(x,y,spurBitset)>0) 
           calig.validPhrase=true;
         else calig.validPhrase=false;
         pecMatrix[x][y].cellAligs.push_back(calig);	
         calig_aux=calig;
				 
             // Add zero fertility words at the left of the coverage vector
         i=leftmost_i; 
         if(i>0 && leftmost_i!=nslen)
         {
           --i; 
           while(i>0 && zFertBitset.test(i)==1)
           {
             calig.coverage.set(i);
             --i;
           }
           if(zFertBitset.test(i+1))
           {
             if(validCoverageForCell(calig.coverage,x,y))
               calig.validPhrase=true;
             else calig.validPhrase=false;
             pecMatrix[x][y].cellAligs.push_back(calig);
           }
         }
                       
             // Add zero fertility words at the right of the coverage vector
         i=rightmost_i;		
         if(i<nslen-1 && rightmost_i!=0)
         {
           calig=calig_aux; 
           ++i;
           while(i<nslen && zFertBitset.test(i)==1)
           {
             calig.coverage.set(i);
             ++i;
           }
           if(zFertBitset.test(i-1))
           {
             if(validCoverageForCell(calig.coverage,x,y))
               calig.validPhrase=true;
             else calig.validPhrase=false;
             pecMatrix[x][y].cellAligs.push_back(calig);
           }
         }
                          
       }
       else
       {// Process inner cell
         for(unsigned int z=tlen-x-1;z<y;++z) 
         {
               // Combine complementary cells   
           for(unsigned int a=0;a<pecMatrix[x][z].cellAligs.size();++a)
             for(unsigned int b=0;b<pecMatrix[tlen-2-z][y].cellAligs.size();++b)
             {
               calig.coverage=pecMatrix[x][z].cellAligs[a].coverage;
               calig.coverage |= pecMatrix[tlen-2-z][y].cellAligs[b].coverage; 
				
               if(validCoverageForCell(calig.coverage,x,y)) 
               {
                 calig.validPhrase=true;
               }
               else calig.validPhrase=false;	
               if(!existCellAlig(pecMatrix[x][y].cellAligs,calig))
                 pecMatrix[x][y].cellAligs.push_back(calig);	 
             }
         }
       }  
     } 
   }
 }
}

//-------------------------
void PhraseExtractionTable::obtainBpSet(BpSet& bpSet)
{
  spurBitset=spuriousWordsBitset(alig);

  // Create the set of bilingual phrases
  for(unsigned int y=0;y<tlen;++y)
  {
    for(unsigned int x=tlen-y-1;x<tlen;++x)
    {
      for(unsigned int a=0;a<pecMatrix[x][y].cellAligs.size();++a)
      {
        if(pecMatrix[x][y].cellAligs[a].validPhrase)
        {
          unsigned int x1=MAX_SENTENCE_LENGTH+1;
          unsigned int x2=0;
          unsigned int y1=tlen-x;
          unsigned int y2=y+1;
          Count c=0;
          unsigned int numWordsAligned=0;
          float weight=0;
                    
          for(unsigned int i=1;i<=nslen;++i)
          {
            if(pecMatrix[x][y].cellAligs[a].coverage.test(i)) 
            {
              if(x1>i) x1=i;
              if(x2<i) x2=i;
              for(unsigned int z=tlen-x-1;z<=y;++z)
              {
                weight+=alig.getValue(i-1,z);
                if(alig.getValue(i-1,z)>0) ++numWordsAligned;	
              }
            }
          }
          c=(float) weight/numWordsAligned;	
      				   
          bpSet.incrPair(x1,x2,y1,y2,c);	           
        }
      }
    }
  }
}

//-------------------------
void PhraseExtractionTable::createVectorWithConsPhrases(std::vector<PhrasePair>& consistentPhrases)
{
  unsigned int x,y,i,a,z,numWordsAligned;
  PhrasePair phPair;
  std::vector<std::string> t_,s_;
  consistentPhrases.clear();
  spurBitset=spuriousWordsBitset(alig);

  // Create a vector with the consistent phrases
  for(y=0;y<tlen;++y)
  {
    for(x=tlen-y-1;x<tlen;++x)
    {
      for(a=0;a<pecMatrix[x][y].cellAligs.size();++a)
      {
        if(pecMatrix[x][y].cellAligs[a].validPhrase)
        {
          t_.clear(); s_.clear();
          phPair.weight=0; numWordsAligned=0;	  
          for(z=tlen-x-1;z<=y;++z) t_.push_back(t[z]);
          for(i=1;i<=nslen;++i)
            if(pecMatrix[x][y].cellAligs[a].coverage.test(i)) 
            {
              s_.push_back(ns[i]);
              for(z=tlen-x-1;z<=y;++z)
              {
                phPair.weight+=alig.getValue(i-1,z);
                if(alig.getValue(i-1,z)>0) ++numWordsAligned;	
              }   		
            }	                   
          phPair.t_=t_;
          phPair.s_=s_;
          phPair.weight=(float) phPair.weight/numWordsAligned;	
      				   
          consistentPhrases.push_back(phPair);	           
        }
      }
    }
  }
}

//-------------------------
Bisegm& PhraseExtractionTable::obtainPossibleSegmentations(PhraseExtractParameters phePars,
                                                           const std::vector<std::string> &_ns,
                                                           const std::vector<std::string> &_t,
                                                           const WordAligMatrix &_alig)
{
  if(_t.size()>=MAX_SENTENCE_LENGTH || _ns.size()>=MAX_SENTENCE_LENGTH)
  {
    std::cerr<<"Warning! the sentences are too long."<<std::endl;
    clear();
  }
  else
  {
    ns=_ns;
    t=_t;
    alig=_alig;	
    tlen=t.size(); nslen=ns.size();
    maxTrgPhraseLength=phePars.maxTrgPhraseLength;
    if(!phePars.constraintSrcLen) maxSrcPhraseLength=MAX_SENTENCE_LENGTH;
    else maxSrcPhraseLength=maxTrgPhraseLength;
    countSpurious=phePars.countSpurious;
    monotone=phePars.monotone;

    maxNumbOfCombsInTable=phePars.maxNumbOfCombsInTable;	

    obtainConsistentPhrases();
    getSegmentationsForEachCellFast();
  }
  
  return pecMatrix[tlen-1][tlen-1].combs;
}

//-------------------------
void PhraseExtractionTable::getSegmentationsForEachCell(void)
{
 unsigned int x,y,z,a,k,numCells,trgPhraseLength;
 std::vector<CellID> comb;	
 CellID cid;
 Bitset<MAX_SENTENCE_LENGTH> zFertBitset,spurBitset;	
 float diag_prune_factor=1;
 Bisegm::const_iterator bsIter;
 
 tlen=t.size(); nslen=ns.size();
 numCells=(tlen*tlen+tlen)/2;	
 zFertBitset=zeroFertBitset(alig);
 spurBitset=spuriousWordsBitset(alig);	
 pruneWasApplied=false;	
  
 for(y=0;y<tlen;++y)
 {
   for(x=tlen-y-1;x<tlen;++x)           
   {	
     for(a=0;a<pecMatrix[x][y].cellAligs.size();++a)
     {
	   if(pecMatrix[x][y].cellAligs[a].validPhrase) 
       {
         comb.clear();
         cid.x=x; cid.y=y; cid.z=a;	
         comb.push_back(cid);
		  	 
         pecMatrix[x][y].combs.insert(comb);	 	
       }
     }
     if(x!=tlen-y-1)
     {            
       diag_prune_factor=(float) (tlen-getDiagNumber(x,y)+1)/getDiagNumber(x,y);    
       for(z=tlen-x-1;z<y;++z) 	  
       {         
         for(a=0;a<pecMatrix[x][z].cellAligs.size();++a)
         {
           if(pecMatrix[x][z].cellAligs[a].validPhrase) 
           {
             cid.x=x; cid.y=z; cid.z=a;	 
				   	  
             for(bsIter=pecMatrix[tlen-2-z][y].combs.begin();bsIter!=pecMatrix[tlen-2-z][y].combs.end();++bsIter)
             {
               comb.clear();
               comb.push_back(cid);				 
					   
               for(k=0;k<bsIter->size();++k)  
               {
                 comb.push_back((*bsIter)[k]);	
               }
               
               if(countSpurious) trgPhraseLength=maxTrgPhraseLengthInComb(comb);
               else trgPhraseLength=maxTrgPhraseLengthInCombNonSpurious(comb,spurBitset);
						 
               if(validSegmentationForCell(comb,zFertBitset,x,y) &&
                  trgPhraseLength<=(unsigned int)maxTrgPhraseLength &&
                  pecMatrix[x][y].combs.size()<(float)diag_prune_factor*(maxNumbOfCombsInTable/numCells)) 
               {
                 pecMatrix[x][y].combs.insert(comb); 	
               }
               else
               {
                 if(pecMatrix[x][y].combs.size()>=(float)diag_prune_factor*(maxNumbOfCombsInTable/numCells))
                   pruneWasApplied=true;
               }
             }
           }
         }
       }
     }
   }
 }
 if(pruneWasApplied) ++numSegmentationPrunings;	  	
}

//-------------------------
void PhraseExtractionTable::getSegmentationsForEachCellFast(void)
{
 unsigned int x,y,z,a,k,trgPhraseLength;
 std::vector<CellID> comb;	
 CellID cid;
 Bitset<MAX_SENTENCE_LENGTH> zFertBitset,spurBitset;	
 float row_prune_factor=1;
 Bisegm::const_iterator bsIter;
 
 tlen=t.size(); nslen=ns.size();

 zFertBitset=zeroFertBitset(alig);
 spurBitset=spuriousWordsBitset(alig);	
 pruneWasApplied=false;	
 
 for(y=0;y<tlen;++y)
 {
   for(x=tlen-y-1;x<tlen;++x)           
   {	
     for(a=0;a<pecMatrix[x][y].cellAligs.size();++a)
     {
	   if(pecMatrix[x][y].cellAligs[a].validPhrase) 
       {
         comb.clear();
         cid.x=x; cid.y=y; cid.z=a;	
         comb.push_back(cid);
         pecMatrix[x][y].combs.insert(comb);
       }
     }
     if(x!=tlen-y-1 && y==tlen-1)
     {           
       row_prune_factor=(tlen*tlen+tlen)/2;
       row_prune_factor=(float) row_prune_factor/tlen;
       row_prune_factor=(float) x/row_prune_factor;    
       for(z=tlen-x-1;z<y;++z) 	  
       {          
         for(a=0;a<pecMatrix[x][z].cellAligs.size();++a)
         {
           if(pecMatrix[x][z].cellAligs[a].validPhrase) 
           {
             cid.x=x; cid.y=z; cid.z=a;	 

             for(bsIter=pecMatrix[tlen-2-z][y].combs.begin();bsIter!=pecMatrix[tlen-2-z][y].combs.end();++bsIter)
             {
               comb.clear();
               comb.push_back(cid);				 

               for(k=0;k<bsIter->size();++k)
               {
                 comb.push_back((*bsIter)[k]);	
               }

               if(countSpurious) trgPhraseLength=maxTrgPhraseLengthInComb(comb);
               else trgPhraseLength=maxTrgPhraseLengthInCombNonSpurious(comb,spurBitset);

               if(validSegmentationForCell(comb,zFertBitset,x,y) &&
                  trgPhraseLength<=(unsigned int)maxTrgPhraseLength &&
                  pecMatrix[x][y].combs.size()<(float)row_prune_factor*(maxNumbOfCombsInTable/tlen)) 
               {
                 pecMatrix[x][y].combs.insert(comb);
               }
               else
               {
                 if(pecMatrix[x][y].combs.size()>=(float)row_prune_factor*(maxNumbOfCombsInTable/tlen))
                   pruneWasApplied=true;
               }
             }
           }
         }
         if(x==tlen-1) pecMatrix[tlen-2-z][y].combs.clear();  
       }
     }
   }
 }
 if(pruneWasApplied) ++numSegmentationPrunings;
}

//-------------------------
bool PhraseExtractionTable::validCoverageForCell(Bitset<MAX_SENTENCE_LENGTH>& c,
                                                 unsigned int x,
                                                 unsigned int y)
{
 unsigned int i,j,leftMost_i;
 int numAlig;
 leftMost_i=nslen;
 numAlig=0;
 
 //If last cell, verify that all words were covered	
 if(x==tlen-1 && y==tlen-1)
 {
   for(i=1;i<nslen;++i)
   {
     if(c.test(i)==0) return false;
     else ++numAlig;
   }
   if(numAlig>maxSrcPhraseLength) return false;
 }
 else
 { //Obtain leftmost source (e) position covered	  
   for(i=1;i<nslen;++i) 
   {
     if(c.test(i)==1 && leftMost_i>i) leftMost_i=i;
         //if exist a gap in the coverage return false
     if(numAlig>0 && c.test(i)==1 && c.test(i-1)==0) return false;
     if(c.test(i)==1) ++numAlig;	    
   }
       //At least one source position must be covered
   if(numAlig==0 || numAlig>maxSrcPhraseLength) return false;
 }
  
 for(j=0;j<tlen;++j)
 {//For each target pos (fj) out of the cell
   if(!sourcePosInCell(j,x,y)) 
   {
     for(i=0;i<nslen-1;++i) 
       if(alig.getValue(i,j)!=0)
       {//If the target pos is aligned return false
         if(c.test(i+1)==1){return false;}
         else
         {
           if(monotone)
           {
             if(i+1<leftMost_i)
             {
               if(j>rightMostPosInCell(x,y)) return false;
             }
             else
             {
               if(j<leftMostPosInCell(x,y)) return false;
             }
           }
         }
       }
   }
 } 
   
 return true;
}

//-------------------------
std::vector<PhrasePair> PhraseExtractionTable::getPhrasesFromSegmentation(std::vector<CellID>& comb)
{
 unsigned int b;
 std::vector<std::string> s_;
 CellID cid;
 PhrasePair phPair;	
 std::vector<PhrasePair> segmentation;
	
 for(b=0;b<comb.size();++b)
 {
   cid=comb[b];
   phPair=getPhrasePairFromCellID(cid);	
   segmentation.push_back(phPair);
 }
 return segmentation;	
}

//-------------------------
PhrasePair PhraseExtractionTable::getPhrasePairFromCellID(CellID& cid)
{
 unsigned int z,i,numWordsAligned;
 std::vector<std::string> t_,s_;
 PhrasePair phPair;	
	
 t_.clear(); s_.clear();
 phPair.weight=0; numWordsAligned=0;
 
 for(z=tlen-cid.x-1;z<=cid.y;++z) t_.push_back(t[z]);
 for(i=1;i<=nslen;++i)
   if(pecMatrix[cid.x][cid.y].cellAligs[cid.z].coverage.test(i)) 
   {
     s_.push_back(ns[i]);
     for(z=tlen-cid.x-1;z<=cid.y;++z)
     {
       phPair.weight+=alig.getValue(i-1,z);
       if(alig.getValue(i-1,z)>0) ++numWordsAligned;	
     }
   }	
 phPair.t_=t_;	 
 phPair.s_=s_;	
                      
 phPair.weight=(float) phPair.weight/numWordsAligned;
						
 return phPair;
}

//-------------------------
bool PhraseExtractionTable::validSegmentationForCell(const std::vector<CellID> &cidVec,
                                                     Bitset<MAX_SENTENCE_LENGTH>& zFertBits,
                                                     unsigned int x,
                                                     unsigned int y,
                                                     unsigned int first)
{
 unsigned int a,i,left,right;
 Bitset<MAX_SENTENCE_LENGTH> coverageOR,coverageXOR,coverage_aux,coverage;
 CellID cid;
	
	
 for(a=first;a<cidVec.size();++a)
 {
   cid=cidVec[a];
	 
   coverage_aux=pecMatrix[cid.x][cid.y].cellAligs[cid.z].coverage;
   coverage_aux&=zFertBits;   	 
   coverageOR |=coverage_aux;
   coverageXOR ^=coverage_aux;   	 
   coverage |=pecMatrix[cid.x][cid.y].cellAligs[cid.z].coverage;	 
 }
 
 if(coverageOR!=coverageXOR) return false;
	 
 for(i=1;i<nslen;++i)
 {
   if(x==tlen-1 && y==tlen-1 && coverage.test(i)==0) return false;
   if(coverage.test(i)==0 && zFertBits.test(i)==1)
   {
     left=i; 
     while(i<nslen && coverage.test(i)==0 && zFertBits.test(i)==1) {++i;}
     right=i-1;	   
     if(left==1)
     {
       if(coverage.test(right+1)==1) return false;
     }
     else
     {
       if(right==nslen-1)
       {
         if(coverage.test(left-1)==1) return false;
       }
       else
       {
         if(coverage.test(left-1)==1 && coverage.test(right+1)==1) return false;
       }
     }
   }
 }
 
 return true; 
}

//-------------------------
Bitset<MAX_SENTENCE_LENGTH> PhraseExtractionTable::zeroFertBitset(WordAligMatrix &waMatrix)
{
 Bitset<MAX_SENTENCE_LENGTH> sol;
 unsigned int i,j;	
 	
 sol.set();
    
 for(i=0;i<waMatrix.get_I();++i)	
  for(j=0;j<waMatrix.get_J();++j)
	 if(waMatrix.getValue(i,j)!=0) sol.reset(i+1);
 sol.reset(0);

 return sol;	
}

//-------------------------
Bitset<MAX_SENTENCE_LENGTH> PhraseExtractionTable::spuriousWordsBitset(WordAligMatrix &waMatrix)
{
 Bitset<MAX_SENTENCE_LENGTH> sol;
 unsigned int i,j;	
 	
 sol.set();
 for(i=0;i<waMatrix.get_I();++i)	
  for(j=0;j<waMatrix.get_J();++j)
	 if(waMatrix.getValue(i,j)!=0) sol.reset(j);
 
 return sol;		
}

//-------------------------
bool PhraseExtractionTable::existCellAlig(const std::vector<CellAlignment> &cellAligs,CellAlignment calig)
{
 unsigned int i;
 bool found;
	
 found=false;
 for(i=0;i<cellAligs.size();++i)
 {
   if(cellAligs[i].coverage==calig.coverage)
   {
     found=true;
	 break;
   }
 }
 return found;
}
//-------------------------
bool PhraseExtractionTable::sourcePosInCell(unsigned int j,
                                            unsigned int x,
                                            unsigned int y)
{
 if(j<=y && j>=tlen-x-1) return true;
 else return false;
}

//-------------------------
unsigned int PhraseExtractionTable::leftMostPosInCell(unsigned int x,
                                                      unsigned int /*y*/)
{
 return tlen-x-1;	
}

//-------------------------
unsigned int PhraseExtractionTable::rightMostPosInCell(unsigned int /*x*/,
                                                       unsigned int y)
{
 return y;	
}
//-------------------------
unsigned int PhraseExtractionTable::trgPhraseLengthInCell(unsigned int x,
                                                          unsigned int y)
{
 return y-(tlen-x-1)+1;
}

//-------------------------
unsigned int PhraseExtractionTable::trgPhraseLengthInCellNonSpurious(unsigned int x,unsigned int y,
                                                                     Bitset<MAX_SENTENCE_LENGTH>& spurBits)
{
 unsigned int j,sol;

 sol=0;
 for(j=leftMostPosInCell(x,y);j<=rightMostPosInCell(x,y);++j)
 {
  if(!spurBits.test(j)) ++sol;	 
 }
 return sol;
}

//-------------------------
unsigned int PhraseExtractionTable::getDiagNumber(unsigned int x,
                                                  unsigned int y)
{
 if(tlen-x>tlen-y)
	 return tlen-x+1;
 else return tlen-y+1;
}

//-------------------------
unsigned int PhraseExtractionTable::trgPhraseLengthInComb(const std::vector<CellID> &cidVec)
{
 unsigned int a,l;
 
 l=0;	
 for(a=0;a<cidVec.size();++a)
 {
   l+=trgPhraseLengthInCell(cidVec[a].x,cidVec[a].y);
 }

 return l;
}

//-------------------------
unsigned int PhraseExtractionTable::maxTrgPhraseLengthInComb(const std::vector<CellID> &cidVec,
                                                             unsigned int first)
{
 unsigned int a,l,maxL;
 
 maxL=0;	
 for(a=first;a<cidVec.size();++a)
 {
   l=trgPhraseLengthInCell(cidVec[a].x,cidVec[a].y);
   if(maxL<l) maxL=l;	 
 }
	
 return maxL;
}

//-------------------------
unsigned int PhraseExtractionTable::maxTrgPhraseLengthInCombNonSpurious(const std::vector<CellID> &cidVec,
	                                                                    Bitset<MAX_SENTENCE_LENGTH>& spurBits,
                                                                        unsigned int first)
{
 unsigned int a,l,maxL;
 
 maxL=0;	
 for(a=first;a<cidVec.size();++a)
 {
   l=trgPhraseLengthInCellNonSpurious(cidVec[a].x,cidVec[a].y,spurBits);
   if(maxL<l) maxL=l;	 
 }
	
 return maxL;
}

//-------------------------
Bitset<MAX_SENTENCE_LENGTH> PhraseExtractionTable::getCoverageForCellID(CellID& cid)
{
 return pecMatrix[cid.x][cid.y].cellAligs[cid.z].coverage;
}

//-------------------------
unsigned int PhraseExtractionTable::get_nslen(void)
{
 return nslen;	
}

//-------------------------
unsigned int PhraseExtractionTable::get_tlen(void)
{
 return tlen;	
}
//-------------------------
void PhraseExtractionTable::clear(void)
{
      // Clear extraction tables
 unsigned int x,y;
	
 for(y=0;y<MAX_SENTENCE_LENGTH;++y)
 {
   for(x=0;x<MAX_SENTENCE_LENGTH;++x)
   {
     pecMatrix[x][y].cellAligs.clear();
     pecMatrix[x][y].combs.clear();
   }
 }
}

//-------------------------
PhraseExtractionTable::~PhraseExtractionTable(void)
{
      // Release memory for pecMatrix
  pecMatrix.clear();

      // Release memory for iterMatrix
  iterMatrix.clear();
}

// Iterator functions for the PhraseExtractionTable class
//---------------

PhraseExtractionTable::iterator PhraseExtractionTable::getSegmIter(PhraseExtractParameters phePars,
                                                                   const std::vector<std::string> &_ns,
                                                                   const std::vector<std::string> &_t,
                                                                   const WordAligMatrix &_alig)

{
  if(_t.size()>=MAX_SENTENCE_LENGTH || _ns.size()>=MAX_SENTENCE_LENGTH)
  {
    std::cerr<<"Warning! the sentences are too long."<<std::endl;
    clear();
  }
  else
  {
    PhraseExtractionTable::iterator iter;
    t=_t;
    ns=_ns;
    alig=_alig;	
    tlen=t.size(); nslen=ns.size();
    maxTrgPhraseLength=phePars.maxTrgPhraseLength;
    if(!phePars.constraintSrcLen) maxSrcPhraseLength=MAX_SENTENCE_LENGTH;
    else maxSrcPhraseLength=maxTrgPhraseLength;
    countSpurious=phePars.countSpurious;
    monotone=phePars.monotone;
    unsigned int x,y;

    PhraseExtractionTable::iterator::initTablePointer(this);
    
    for(y=0;y<MAX_SENTENCE_LENGTH;++y)
      for(x=0;x<MAX_SENTENCE_LENGTH;++x)
        iterMatrix[x][y].set_x_y(x,y);
    obtainConsistentPhrases();
    iterMatrix[tlen-1][tlen-1].init(tlen-1,tlen-1);
  }
  return iterMatrix[tlen-1][tlen-1];
}

//----- iterator class static member definitions

PhraseExtractionTable* PhraseExtractionTable::iterator::petPtr;
std::vector<CellID> PhraseExtractionTable::iterator::cidVec;

// iterator function definitions
//--------------------------

//-------------------------
PhraseExtractionTable::iterator::iterator(void)
{
  petPtr=NULL;
  iter=NULL;
  first=MAX_SENTENCE_LENGTH+1;
}

//-------------------------
void PhraseExtractionTable::iterator::initTablePointer(PhraseExtractionTable* _petPtr)
{
  petPtr=_petPtr;
}

//-------------------------
void PhraseExtractionTable::iterator::init(unsigned int _x,unsigned int _y)
{
  iter=NULL;
  x=_x;
  y=_y;
  i=0;
  z=0; --z;
  first=MAX_SENTENCE_LENGTH+1;
  cidVec.clear();
  
  ++(*this);
}

//-------------------------
void PhraseExtractionTable::iterator::set_x_y(unsigned int _x,unsigned int _y)
{
  x=_x;
  y=_y;
}

//-------------------------
bool PhraseExtractionTable::iterator::operator++(void)
{
  bool end=false;
  bool itOk;
  
  while(!end)
  {
    itOk=this->iterate();

    if(!itOk) end=true;
    else
    {
      unsigned int trgLen;
      if(petPtr->countSpurious) trgLen=petPtr->maxTrgPhraseLengthInComb(cidVec,first);
      else trgLen=petPtr->maxTrgPhraseLengthInCombNonSpurious(cidVec,petPtr->spurBitset,first);
    
      if(petPtr->validSegmentationForCell(cidVec,petPtr->zFertBitset,x,y,first) &&
         trgLen<=(unsigned int)petPtr->maxTrgPhraseLength)
      {
        end=true;
      }
    }
  }
  if(!itOk) return false;
  else return true;
}

//-------------------------
bool PhraseExtractionTable::iterator::iterate(void)
{
  if(!this->end())
  {
    bool searchOk;

    if(i==0)
    { // Segmentations of length equal to 1
      searchOk=searchFirstValidCell();
      if(!searchOk) // i needs to be increased
      {
        searchFirstValid_i();
      }
    }
    else
    { // Segmentations of length greater than 1
      searchOk=searchFirstValidCell();
      if(!searchOk)
      {
        z=0; --z;
        searchFirstValidCell();
        ++(*iter);
      }
      if(!searchOk && this->iter->end()) // i needs to be increased
      {
        searchFirstValid_i();
      }
    }
    // if end==true, pop the last element of cidVec
    if(this->end())
    {
      if(first<=MAX_SENTENCE_LENGTH) cidVec.pop_back();
      first=MAX_SENTENCE_LENGTH+1;
    }
    // Return true if the end has been reached
    return !(this->end());
  }
  else return false;
}

//-------------------------
bool PhraseExtractionTable::iterator::searchFirstValidCell(void)
{
  bool stop=false;
  
  while(!stop)
  { // Search the first valid cellAlig
    ++z;
    if(z>=petPtr->pecMatrix[x][y-i].cellAligs.size())
    { // There is not more segmentations
      stop=true;
    }
    else
    { // The iterator is set to the next segmentation
      if(petPtr->pecMatrix[x][y-i].cellAligs[z].validPhrase)
      {
        CellID cid;
        cid.x=x;
        cid.y=y-i;
        cid.z=z;
        if(first>MAX_SENTENCE_LENGTH)
        {
          cidVec.push_back(cid);
          first=cidVec.size()-1;
        }
        else cidVec[first]=cid;
        
        stop=true;
      }
    }
  }
  
  if(z>=petPtr->pecMatrix[x][y-i].cellAligs.size()) return false;
  else return true;
}

//-------------------------
bool PhraseExtractionTable::iterator::searchFirstValid_i(void)
{
  bool stop=false;
  bool searchOk;
  
  while(!stop)
  {
    ++i;
    z=0; --z;
    if(this->end()) stop=true;
    else
    {
      searchOk=searchFirstValidCell();
      if(searchOk)
      {
        petPtr->iterMatrix[(petPtr->tlen)-(y-i+2)][y].reset();
        this->iter=&(petPtr->iterMatrix[(petPtr->tlen)-(y-i+2)][y]);
        if(!this->iter->end()) stop=true;
      }
    }
  }

  return !(this->end());
}

//-------------------------
bool PhraseExtractionTable::iterator::end(void)const
{
  if(y<i) return true;
  else
  {
    if(y-i>=petPtr->tlen-x-1) return false;
    else return true;
  }
}

//-------------------------
void PhraseExtractionTable::iterator::reset(void)
{
  if(iter!=NULL)
  {
    iter->clear();
  }
  first=MAX_SENTENCE_LENGTH+1;
  i=0;
  z=0; --z;

  ++(*this);
}

//-------------------------
void PhraseExtractionTable::iterator::clear(void)
{
  iter=NULL;
}

//-------------------------
std::vector<CellID> PhraseExtractionTable::iterator::operator*(void)const 
{
  std::vector<CellID> vcid;
  CellID cid;

  if(!(this->end()))
  {
    cid.x=x;
    cid.y=y-i;
    cid.z=z;
    if(i==0)
    {
      vcid.push_back(cid);
    }
    else
    {
      unsigned int i;
      
      vcid=*(*iter);
      vcid.push_back(cid);
      for(i=vcid.size()-1;i>0;--i)
        vcid[i]=vcid[i-1];
      vcid[0]=cid;
    }
  }
  return vcid;
}

//-------------------------
const std::vector<CellID>& PhraseExtractionTable::iterator::getCidVec(void)const
{
  return cidVec;
}

//-------------------------
PhraseExtractionTable::iterator::~iterator()
{
  
}
