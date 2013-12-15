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
/* Module: PhraseExtractor                                          */
/*                                                                  */
/* Definitions file: PhraseExtractor.cc                             */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "PhraseExtractor.h"
#include <Trie.h>

//--------------- Constants ------------------------------------------
#define DEFAULT_RANDOM_SEED          31415
#define NUM_RAND_WALKS              100000
#define SENT_LENGTH_LIMIT_EXACT_SRF    100

//--------------- Function definitions

//-------------------------
PhraseExtractor::PhraseExtractor(void)
{
}

//-------------------------
void PhraseExtractor::extractConsistentPhrases(PhraseExtractParameters phePars,
                                               const Vector<string> &ns,
                                               const Vector<string> &t,
                                               const WordAligMatrix &alig,
                                               Vector<PhrasePair>& outvph)
{
  BpSet bpSet;
  extractConsistentPhrasesOch(phePars,ns,t,alig,outvph,bpSet);
}

//-------------------------
void PhraseExtractor::extractConsistentPhrasesOch(PhraseExtractParameters phePars,
                                                  const Vector<string> &ns,
                                                  const Vector<string> &t,
                                                  const WordAligMatrix &alig,
                                                  Vector<PhrasePair>& outvph,
                                                  BpSet& bpSet)
{
 if(t.size()>=MAX_SENTENCE_LENGTH || ns.size()-1>=MAX_SENTENCE_LENGTH)
 {
   cerr<<"Warning! the sentences are too long."<<endl;
 }
 else
 {
       // Init data members
   PositionIndex tlen=t.size();
   PositionIndex slen=ns.size()-1;
   PositionIndex maxTrgPhraseLength=phePars.maxTrgPhraseLength;
   bpSet.clear();
   
       // Extract set of consistent phrase pairs
   for(PositionIndex i1=1;i1<=slen;++i1)
   {
     for(PositionIndex i2=i1;i2<=slen;++i2)
     {
           // Obtain SP
       Vector<PositionIndex> SP;
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
//        cerr<<"------"<<endl;
//        cerr<<i1<<" "<<i2<<endl;
//        cerr<<"SP: ";
//        for(unsigned int k=0;k<SP.size();++k) cerr<<SP[k]<<" ";
//        cerr<<endl;
//        cerr<<"consec: "<<consec<<endl;
//        cerr<<"******"<<endl;
       
       if(consec)
       {
             // SP only contains consecutive positions
         PositionIndex j1=SP.front();
         PositionIndex j2=SP.back();

             // Obtain TP
         Vector<PositionIndex> TP;
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

//          cerr<<j1<<" "<<j2<<endl;
//          cerr<<"TP: ";
//          for(unsigned int k=0;k<TP.size();++k) cerr<<TP[k]<<" ";
//          cerr<<endl;
//          cerr<<"++++++"<<endl;
         
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
               {
                 outvph.push_back(ppair);
                 bpSet.incrPair(i1,i2,jp,jpp,1.0);	           
               }

//                cerr<<jp<<" "<<jpp<<endl;
//                for(unsigned int k=0;k<ppair.s_.size();++k)
//                  cerr<<ppair.s_[k]<<" ";
//                cerr<<endl;

//                for(unsigned int k=0;k<ppair.t_.size();++k)
//                  cerr<<ppair.t_[k]<<" ";
//                cerr<<endl;

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
double PhraseExtractor::segmBasedExtraction(PhraseExtractParameters phePars,
                                            const Vector<string> &ns,
                                            const Vector<string> &t,
                                            const WordAligMatrix &alig,
                                            Vector<PhrasePair>& outvph,
                                            int verbose/*=0*/)
{
 if(t.size()>=MAX_SENTENCE_LENGTH || ns.size()-1>=MAX_SENTENCE_LENGTH)
 {
   cerr<<"Warning! the sentences are too long."<<endl;
   return 0;
 }
 else
 {
   BpSet bpSet;
   BpSet C;
   extractConsistentPhrasesOch(phePars,
                               ns,
                               t,
                               alig,
                               outvph,
                               bpSet);

   double logNumSegms;
   if(t.size()<=SENT_LENGTH_LIMIT_EXACT_SRF &&
      ns.size()-1<=SENT_LENGTH_LIMIT_EXACT_SRF)
   {
//   logNumSegms=srfPhraseExtract(ns.size(),t.size(),bpSet,C);
     logNumSegms=srfPhraseExtractDp(ns.size(),t.size(),bpSet,C,verbose);
   }
   else
   {
     logNumSegms=approxSrfPhraseExtract(ns.size(),t.size(),bpSet,C,verbose);
   }
   
   obtainPhrPairVecFromBpSet(ns,t,C,outvph,logNumSegms);
   
   return logNumSegms;
 }
}

//-------------------------
double PhraseExtractor::srfPhraseExtract(PositionIndex nslen,
                                         PositionIndex tlen,
                                         const BpSet& bpSet,
                                         BpSet& C)
{
  Bitset<MAX_SENTENCE_LENGTH> SP((unsigned int)0);
  for(unsigned int x=1;x<=nslen-1;++x) SP.set(x,1);
  Bitset<MAX_SENTENCE_LENGTH> TP((unsigned int)0);
  for(unsigned int y=1;y<=tlen;++y) TP.set(y,1);
  C.clear();
  double logNumSegms=srfPhraseExtractRec(nslen,tlen,bpSet,SP,TP,C);
  return logNumSegms;
}

//-------------------------
double PhraseExtractor::srfPhraseExtractRec(PositionIndex nslen,
                                            PositionIndex tlen,
                                            const BpSet& bpSet,
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
          double logNumSegms=srfPhraseExtractRec(nslen,tlen,bpSet,SP_AUX,TP_AUX,C);
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
double PhraseExtractor::srfPhraseExtractDp(PositionIndex nslen,
                                           PositionIndex tlen,
                                           const BpSet& bpSet,
                                           BpSet& C,
                                           int verbose/*=false*/)
{
  SrfNodeInfoMap sniMap;
  fillSrfNodeInfoMap(nslen,tlen,bpSet,sniMap);
  if(verbose)
    cerr<<"Num. equiv. classes: "<<sniMap.size()<<" ; Equiv. classes with leafs: "<<sniMap.numNodesWithLeafs()<<endl;
  
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
double PhraseExtractor::approxSrfPhraseExtract(PositionIndex nslen,
                                               PositionIndex tlen,
                                               const BpSet& bpSet,
                                               BpSet& C,
                                               int verbose/*=false*/)
{
      // Set random seed
  srand(DEFAULT_RANDOM_SEED);

  if(verbose)
    cerr<<"Performing approximate srf phrase extract"<<endl;
  
      // Obtain bisegmentation length info
  if(verbose)
    cerr<<"Obtaining bisegmentation length info..."<<endl;
  SrfNodeInfoMap sniMap;
  fillSrfNodeInfoMap(nslen,tlen,bpSet,sniMap,false);
  if(verbose)
    cerr<<"#Equivalence classes: "<<sniMap.size()<<endl;
  
      // Execute random walks
  if(verbose)
    cerr<<"Executing random walks..."<<endl;
  double logNumSegms;
  logNumSegms=bisegmRandWalk(nslen,tlen,bpSet,sniMap,C);
  
  return logNumSegms;
}

//-------------------------
void PhraseExtractor::fillSrfNodeInfoMap(PositionIndex nslen,
                                         PositionIndex tlen,
                                         const BpSet& bpSet,
                                         SrfNodeInfoMap& sniMap,
                                         bool calcCSet/*=true*/)
{
  Bitset<MAX_SENTENCE_LENGTH> SP((unsigned int)0);
  for(unsigned int x=1;x<=nslen-1;++x) SP.set(x,1);
  Bitset<MAX_SENTENCE_LENGTH> TP((unsigned int)0);
  for(unsigned int y=1;y<=tlen;++y) TP.set(y,1);
  sniMap.clear();
  
  fillSrfNodeInfoMapRec(nslen,tlen,bpSet,SP,TP,sniMap,calcCSet);
}

//-------------------------
void PhraseExtractor::fillSrfNodeInfoMapRec(PositionIndex nslen,
                                            PositionIndex tlen,
                                            const BpSet& bpSet,
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
            fillSrfNodeInfoMapRec(nslen,
                                  tlen,
                                  bpSet,
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
double PhraseExtractor::bisegmRandWalk(PositionIndex nslen,
                                       PositionIndex tlen,
                                       const BpSet& bpSet,
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
    bool success=bisegmRandWalkRec(nslen,tlen,bpSet,SP,TP,sb,sniMap,result);
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
bool PhraseExtractor::bisegmRandWalkRec(PositionIndex nslen,
                                        PositionIndex tlen,
                                        const BpSet& bpSet,
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
    Vector<pair<SrfNodeKey,SrfNodeInfo> > nextSegmStepVec;
    Vector<SrfBisegm> nextSbVec;
    Vector<Bitset<MAX_SENTENCE_LENGTH> > srcPosVec;
    Vector<Bitset<MAX_SENTENCE_LENGTH> > trgPosVec;
    
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
            nextSegmStepVec.push_back(make_pair(newSnk,sni));
                // Store new sb object
            SrfBisegm newSb;
            newSb.srcCuts=sb.srcCuts;
            newSb.srcCuts.push_back(x);
            newSb.trgCuts=sb.trgCuts;
            newSb.trgCuts.push_back(make_pair(y1,y2));

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
      return bisegmRandWalkRec(nslen,
                               tlen,
                               bpSet,
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
void PhraseExtractor::obtainPhrPairVecFromBpSet(const Vector<string> &ns,
                                                const Vector<string> &t,
                                                const BpSet& bpSet,
                                                Vector<PhrasePair>& outvph,
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
double PhraseExtractor::gen01RandNum(void)
{
  return (double)rand()/RAND_MAX;
}

//-------------------------
PhraseExtractor::~PhraseExtractor(void)
{
}
