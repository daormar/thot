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

/**
 * @file thot_wg_proc.cc
 * 
 * @brief Processes a wordgraph stored in a file.
 */

//--------------- Include files ---------------------------------------

#include <options.h>
#include <ctimer.h>
#include <set>
#include <string>
#include <ErrorDefs.h>
#include "thot_wg_proc_pars.h"
#include "WordGraph.h"

//--------------- Constants -------------------------------------------


//--------------- Function Declarations -------------------------------

int processParameters(thot_wg_proc_pars pars);
int printNbList(const std::vector<std::pair<std::string,float> >& compWeights,
                const std::vector<std::pair<Score,std::string> >& nblist,
                const std::vector<std::vector<Score> >& scoreCompsVec,
                const std::vector<NbSearchHyp>& hypList,
                std::string nbListFile);
int process_bp_par(const WordGraph& wordGraph,
                   thot_wg_proc_pars pars);
int handleParameters(int argc,
                     char *argv[],
                     thot_wg_proc_pars& pars);
int takeParameters(int argc,
                   const std::vector<std::string>& argv_stl,
                   thot_wg_proc_pars& pars);
int checkParameters(thot_wg_proc_pars& pars);
void printParameters(thot_wg_proc_pars pars);
void printUsage(void);
void version(void);

//--------------- Constants -------------------------------------------

#define WGWEIGHT                0
#define ECMWEIGHT               1
#define DEFAULT_N_VALUE         1
#define DEFAULT_ECMWEIGHT       1
#define DEFAULT_WGWEIGHT        1

//--------------- Global variables ------------------------------------


//--------------- Function Definitions --------------------------------


//---------------
int main(int argc,char *argv[])
{
  thot_wg_proc_pars pars;
    
  if(handleParameters(argc,argv,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    return processParameters(pars);
  }
}

//---------------
int processParameters(thot_wg_proc_pars pars)
{
  bool ret;
  WordGraph wordGraph;

      // Load word-graph
  ret=wordGraph.load(pars.w_str.c_str());
  if(ret==THOT_ERROR) return THOT_ERROR;
  
  if(pars.wgp_given)
  {
        // Prune word-graph
    WordGraph wgAux=wordGraph;
    unsigned int numPrunedAndNotPrunedArcs=wgAux.getNumberOfPrunedAndNonPrunedArcs();

        // Get initial time
    std::cerr<<"Pruning word-graph..."<<std::endl;
    double total_time=0,elapsed_ant,elapsed,ucpu,scpu;  
    ctimer(&elapsed_ant,&ucpu,&scpu);

        // Prune word graph
    unsigned int numPrunedArcs=wgAux.prune(pars.pruningThreshold);
    
        // Get final time
    ctimer(&elapsed,&ucpu,&scpu);
        // Obtain total time
    total_time+=elapsed-elapsed_ant;
    std::cerr<<"done, processing time: "<<total_time<<" seconds"<<std::endl;

    std::cerr<<"Initial number of arcs: "<<numPrunedAndNotPrunedArcs<<std::endl;
    std::cerr.precision(3);
    std::cerr<<numPrunedArcs<<" arcs were pruned ("<<100*((float)numPrunedArcs/numPrunedAndNotPrunedArcs)<<" %)"<<std::endl;
        // Print word-graph
    std::string wgOutFile=pars.o_str;
    wgOutFile=wgOutFile+".wgp";
    ret=wgAux.print(wgOutFile.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;
  }

  if(pars.bp_given)
  {
        // Obtain best-path from hypStateIndex
    ret=process_bp_par(wordGraph,pars);
    if(ret==THOT_ERROR) return THOT_ERROR;
  }

      // Print n-best list
  if(pars.n_given)
  {
    if(pars.wgp_given)
    {
          // Prune word-graph
      WordGraph wgAux=wordGraph;
      wgAux.prune(pars.pruningThreshold);

          // Obtain component weights
      std::vector<std::pair<std::string,float> > compWeights;
      wgAux.getCompWeights(compWeights);

          // Obtain n-best list
      std::vector<std::pair<Score,std::string> > nblist;
      std::vector<NbSearchHyp> hypList;
      std::vector<std::vector<Score> > scoreCompsVec;
      wgAux.obtainNbestList(pars.nbListLen,nblist,hypList,scoreCompsVec,pars.v_given);
      
          // Print file
      std::string nbListFile=pars.o_str;
      nbListFile=nbListFile+".nbl_pruned";
      ret=printNbList(compWeights,nblist,scoreCompsVec,hypList,nbListFile);
      if(ret==THOT_ERROR) return THOT_ERROR;
    }
    else
    {
          // Obtain component weights
      std::vector<std::pair<std::string,float> > compWeights;
      wordGraph.getCompWeights(compWeights);

          // Obtain n-best list
      std::vector<std::pair<Score,std::string> > nblist;
      std::vector<NbSearchHyp> hypList;
      std::vector<std::vector<Score> > scoreCompsVec;
      wordGraph.obtainNbestList(pars.nbListLen,nblist,hypList,scoreCompsVec,pars.v_given);

          // Print file
      std::string nbListFile=pars.o_str;
      nbListFile=nbListFile+".nbl";
      ret=printNbList(compWeights,nblist,scoreCompsVec,hypList,nbListFile);
      if(ret==THOT_ERROR) return THOT_ERROR;
    }
  }

      // Obtain and print new word-graph with arcs topologically ordered
  if(pars.t_given)
  {
    WordGraph wgAux=wordGraph;
    wgAux.orderArcsTopol();
    std::string wgArcsTopOrdFile=pars.o_str;
    wgArcsTopOrdFile=wgArcsTopOrdFile+".wg_arcs_top_order";
    ret=wgAux.print(wgArcsTopOrdFile.c_str());
    if(ret==THOT_ERROR) return THOT_ERROR;
  }

      // Obtain and print new word-graph composed of useful states
  if(pars.u_given)
  {
    WordGraph wgAux=wordGraph;
    if(pars.wgp_given)
    {
          // Prune word-graph
      wgAux.prune(pars.pruningThreshold);

          // Obtain useful states
      wgAux.obtainWgComposedOfUsefulStates();

          // Print word-graph
      std::string wgUsefulOutFile=pars.o_str;
      wgUsefulOutFile=wgUsefulOutFile+".wg_useful_pruned";
      ret=wgAux.print(wgUsefulOutFile.c_str());
      if(ret==THOT_ERROR) return THOT_ERROR;
    }
    else
    {
          // Get initial time
      std::cerr<<"Obtaining useful states..."<<std::endl;
      double total_time=0,elapsed_ant,elapsed,ucpu,scpu;  
      ctimer(&elapsed_ant,&ucpu,&scpu);
        
          // Obtain useful states
      wgAux.obtainWgComposedOfUsefulStates();

          // Get final time
      ctimer(&elapsed,&ucpu,&scpu);
          // Obtain total time
      total_time+=elapsed-elapsed_ant;
      std::cerr<<"done, processing time: "<<total_time<<" seconds"<<std::endl;
 
          // Print word-graph
      std::string wgUsefulOutFile=pars.o_str;
      wgUsefulOutFile=wgUsefulOutFile+".wg_useful";
      ret=wgAux.print(wgUsefulOutFile.c_str());
      if(ret==THOT_ERROR) return THOT_ERROR;
    }
  }

  return THOT_OK;
}

//---------------
int printNbList(const std::vector<std::pair<std::string,float> >& compWeights,
                const std::vector<std::pair<Score,std::string> >& nblist,
                const std::vector<std::vector<Score> >& scoreCompsVec,
                const std::vector<NbSearchHyp>& hypList,
                std::string nbListFile)
{
  std::ofstream outS;
  outS.open(nbListFile.c_str(),std::ios::trunc);
  if(!outS)
  {
    std::cerr<<"Error while printing n-best list file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {    
        // Print component weights as header if they were given
    if(!compWeights.empty())
    {
      outS<<"# ";
      for(unsigned int i=0;i<compWeights.size();++i)
      {
        outS<<compWeights[i].first<<" "<<compWeights[i].second;
        if(i!=compWeights.size()-1) outS<<" , ";
      }
      outS<<std::endl;
    }

        // Iterate over n-best list elements
    for(unsigned int i=0;i<nblist.size();++i)
    {
      outS<<nblist[i].first<<" |||";

          // Print score components if given
      if(i<scoreCompsVec.size())
      {
        for(unsigned int j=0;j<scoreCompsVec[i].size();++j)
          outS<<" "<<scoreCompsVec[i][j];
        outS<<" |||";
      }

          // Print hypothesis information if given
      if(i<hypList.size())
      {
        for(unsigned int j=0;j<hypList[i].size();++j)
          outS<<" "<<hypList[i][j];
        outS<<" |||";        
      }

          // Print translation
      outS<<" "<<nblist[i].second<<std::endl;
    }
    outS.close();
    return THOT_OK;
  }
}

//---------------
int process_bp_par(const WordGraph& wordGraph,
                   thot_wg_proc_pars pars)
{
  std::vector<WordGraphArc> arcVec;
  std::set<WordGraphArcId> emptyWgArcIdSet;

      // Obtain best path
  Score bestScore=wordGraph.bestPathFromFinalStateToIdxWeights(pars.hypStateIndex,
                                                               emptyWgArcIdSet,
                                                               pars.compWeights,
                                                               arcVec);
    
      // Print results
  std::ofstream outS;
  std::string bpFile=pars.o_str;
  bpFile=bpFile+".bp";
  
  outS.open(bpFile.c_str(),std::ios::out);
  if(!outS)
  {
    std::cerr<<"Error while printing bp file."<<std::endl;
    return THOT_ERROR;
  }
  else
  {
        // Print parameters
    outS<<"-bp "<<pars.hypStateIndex;
    for(unsigned int i=0;i<pars.compWeights.size();++i)
      outS<<" "<<pars.compWeights[i];
    outS<<std::endl;
    
        // Print best path
    std::string str;
    for(unsigned int i=0;i<arcVec.size();++i)
    {
      unsigned int r=arcVec.size()-i-1;
      outS<<arcVec[r].predStateIndex<<" -> "<<arcVec[r].succStateIndex;
      if(i!=0) str=str+" ";
      for(unsigned int j=0;j<arcVec[r].words.size();++j)
      { 
       str=str+arcVec[r].words[j];
       if(j!=arcVec[r].words.size()-1) str=str+" ";
        outS<<" "<<arcVec[r].words[j];
      }
      outS<<std::endl;
    }
        // Print best score
    outS<<"Score: "<<bestScore<<std::endl;
        // Print sentence
    outS<<str<<std::endl;
    
    return THOT_OK;
  }  
}

//---------------
int handleParameters(int argc,
                     char *argv[],
                     thot_wg_proc_pars& pars)
{
  if(argc==1 || readOption(argc,argv,"--version")!=-1)
  {
    version();
    return THOT_ERROR;
  }
  if(readOption(argc,argv,"--help")!=-1)
  {
    printUsage();
    return THOT_ERROR;   
  }

  std::vector<std::string> argv_stl=argv2argv_stl(argc,argv);
  if(takeParameters(argc,argv_stl,pars)==THOT_ERROR)
  {
    return THOT_ERROR;
  }
  else
  {
    if(checkParameters(pars)==THOT_OK)
    {
      printParameters(pars);
      return THOT_OK;
    }
    else
    {
      return THOT_ERROR;
    }
  }
}

//---------------
int takeParameters(int argc,
                   const std::vector<std::string>& argv_stl,
                   thot_wg_proc_pars& pars)
{
  int i=1;
  unsigned int matched;
  
  while(i<argc)
  {
    matched=0;
    
        // -w parameter
    if(argv_stl[i]=="-w" && !matched)
    {
      pars.w_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -s parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.w_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -wgp parameter
    if(argv_stl[i]=="-wgp" && !matched)
    {
      pars.wgp_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -wgp parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.pruningThreshold=atof(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -bp parameter
    if(argv_stl[i]=="-bp" && !matched)
    {
      pars.bp_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -bp parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        ++matched;
        bool end=false;
        std::vector<float> floatVec;
        while(!end)
        {
          if(i==argc-1 || isOption(argv_stl[i+1].c_str()))
            end=true;
          else
          {
            floatVec.push_back(atof(argv_stl[i+1].c_str()));
            ++i;
          }
        }
            // Set index of hypothesis state
        pars.hypStateIndex=(HypStateIndex) floatVec[0];
            // Set weights of the components if they were given
        for(unsigned int i=1;i<floatVec.size();++i)
          pars.compWeights.push_back(floatVec[i]);
      }
    }

        // -o parameter
    if(argv_stl[i]=="-o" && !matched)
    {
      pars.o_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -o parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.o_str=argv_stl[i+1];
        ++matched;
        ++i;
      }
    }

        // -n parameter
    if(argv_stl[i]=="-n" && !matched)
    {
      pars.n_given=true;
      if(i==argc-1)
      {
        std::cerr<<"Error: no value for -n parameter."<<std::endl;
        return THOT_ERROR;
      }
      else
      {
        pars.nbListLen=atoi(argv_stl[i+1].c_str());
        ++matched;
        ++i;
      }
    }

        // -u parameter
    if(argv_stl[i]=="-u" && !matched)
    {
      pars.u_given=true;
      ++matched;
    }

        // -t parameter
    if(argv_stl[i]=="-t" && !matched)
    {
      pars.t_given=true;
      ++matched;
    }

        // -v parameter
    if(argv_stl[i]=="-v" && !matched)
    {
      pars.v_given=true;
      ++matched;
    }

        // -v1 parameter
    if(argv_stl[i]=="-v1" && !matched)
    {
      pars.v1_given=true;
      ++matched;
    }

        // Check if current parameter is not valid
    if(matched==0)
    {
      std::cerr<<"Error: parameter "<<argv_stl[i]<<" not valid."<<std::endl;
      return THOT_ERROR;
    }
    ++i;
  }
  return THOT_OK;
}

//---------------
int checkParameters(thot_wg_proc_pars& pars)
{
  if(!pars.w_given)
  {
    std::cerr<<"Error: -w parameter not given!"<<std::endl;
    return THOT_ERROR;
  }

  if(!pars.o_given)
  {
    std::cerr<<"Error: -o parameter not given!"<<std::endl;
    return THOT_ERROR;
  }
  
  return THOT_OK;
}

//---------------
void printParameters(thot_wg_proc_pars pars)
{
  std::cerr<<"File with word-graph: "<<pars.w_str<<std::endl;

  if(pars.wgp_given)
    std::cerr<<"-wgp: "<<pars.pruningThreshold<<std::endl;

  if(pars.bp_given)
  {
    std::cerr<<"-bp: "<<pars.hypStateIndex;
    for(unsigned int i=0;i<pars.compWeights.size();++i)
      std::cerr<<" "<<pars.compWeights[i];
    std::cerr<<std::endl;
  }

  if(pars.u_given)
    std::cerr<<"-u"<<std::endl;

  if(pars.t_given)
    std::cerr<<"-t"<<std::endl;

  std::cerr<<"-o: "<<pars.o_str<<std::endl;
}

//---------------
void printUsage(void)
{
  std::cerr<<"Usage: thot_wg_proc        -w <string>\n";
  std::cerr<<"                           [-bp <int> [<float1> ... <floatn>] ]\n";
  std::cerr<<"                           [-wgp <float>] [-n <int> [-y] ] [-u] [-t]\n";
  std::cerr<<"                           -o <string>\n";
  std::cerr<<"                           [-v|-v1] [--help] [--version]\n\n";
  std::cerr<<"-w <string>                File with word-graph to be loaded.\n";
  std::cerr<<"-bp <int> [<float1> ... <floatn>]\n";
  std::cerr<<"                           Obtain best-path from state <int> to a final\n";
  std::cerr<<"                           state. Optionally, a float vector containing the\n";
  std::cerr<<"                           weights of the score components can be given.\n";
  std::cerr<<"-wgp <float>               Prune word-graph using the given threshold.\n";
  std::cerr<<"                           Threshold=0 -> no pruning is performed.\n";
  std::cerr<<"                           Threshold=1 -> only the best arc arriving to each\n";
  std::cerr<<"                                          state is retained.\n";
  std::cerr<<"                           If not given, the number of arcs is not\n";
  std::cerr<<"                           restricted.\n";
  std::cerr<<"                           NOTE: arcs must be topologically ordered.\n";
  std::cerr<<"-n <int>                   Print n-best list of length <int>.\n";
  std::cerr<<"                           NOTE: -n and -wgp options can be combined\n";
  std::cerr<<"-y                         Incorporate hypothesis information when printing\n";
  std::cerr<<"                           n-best list (should be combined with -n option).\n";
  std::cerr<<"-u                         Print word-graph composed of useful states.\n";
  std::cerr<<"                           NOTE: -u and -wgp options can be combined\n";
  std::cerr<<"-t                         Print word-graph with arcs topologically ordered.\n";
  std::cerr<<"-o <string>                Set prefix for output files.\n";
  std::cerr<<"-v | -v1                   Verbose modes.\n";
  std::cerr<<"--help                     Display this help and exit.\n";
  std::cerr<<"--version                  Output version information and exit.\n";
}

//---------------
void version(void)
{
  std::cerr<<"thot_wg_proc is part of the thot package "<<std::endl;
  std::cerr<<"thot version "<<THOT_VERSION<<std::endl;
  std::cerr<<"thot is GNU software written by Daniel Ortiz"<<std::endl;
}
