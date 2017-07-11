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
/* Module: KenLm                                                    */
/*                                                                  */
/* Definitions file: KenLm.cc                                       */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "KenLm.h"

//--------------- Global variables -----------------------------------


//--------------- KenLm class function definitions

//-------------------------
KenLm::KenLm(void)
{
  modelPtr=NULL;
}

//-------------------------
LgProb KenLm::getNgramLgProb(WordIndex w,
                             const Vector<WordIndex>& vu)
{
      // Reverse history (required by kenlm library)
  Vector<WordIndex> rev_vu=vu;
  std::reverse(rev_vu.begin(),rev_vu.end());

      // Obtain log-prob
  lm::ngram::State out_st;
  return modelPtr->FullScoreForgotState(&*rev_vu.begin(),&*rev_vu.end(),w,out_st).prob*M_LN10;
}

//-------------------------
LgProb KenLm::getNgramLgProbStr(string s,
                                const Vector<string>& rq)
{
  WordIndex w=stringToWordIndex(s);

  Vector<WordIndex> wIdxVec;
  for(unsigned int i=0;i<rq.size();++i)
  {
    wIdxVec.push_back(stringToWordIndex(rq[i]));
  }

  return getNgramLgProb(w,wIdxVec);
}

//-------------------------
LgProb KenLm::getLgProbEnd(const Vector<WordIndex>& vu)
{
  bool found;
  return getNgramLgProb(getEosId(found),vu);
}

//-------------------------
LgProb KenLm::getLgProbEndStr(const Vector<string>& rq)
{
  Vector<WordIndex> wIdxVec;
  for(unsigned int i=0;i<rq.size();++i)
  {
    wIdxVec.push_back(stringToWordIndex(rq[i]));
  }

  return getLgProbEnd(wIdxVec);
}

//-------------------------
bool KenLm::getStateForWordSeq(const Vector<WordIndex>& wordSeq,
                               Vector<WordIndex>& state)
{
  state=wordSeq;
  return true;
}

//-------------------------
void KenLm::getStateForBeginOfSentence(Vector<WordIndex>& state)
{
  bool found;
  unsigned int ngramOrder=getNgramOrder();
  state.clear();
  
  if(ngramOrder>0)
  {
    for(unsigned int i=0;i<ngramOrder-1;++i)
      state.push_back(getBosId(found));
  }
}

//-------------------------
LgProb KenLm::getNgramLgProbGivenState(WordIndex w,
                                       Vector<WordIndex>& state)
{
  LgProb lp=getNgramLgProb(w,state);
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=w;
  return lp;
}

//-------------------------
LgProb KenLm::getNgramLgProbGivenStateStr(std::string s,
                                          Vector<WordIndex>& state)
{
  WordIndex w=stringToWordIndex(s);

  return getNgramLgProbGivenState(w,state);
}

//-------------------------
LgProb KenLm::getLgProbEndGivenState(Vector<WordIndex>& state)
{
  bool found;

  LgProb lp=getLgProbEnd(state);
  for(unsigned int i=1;i<state.size();++i) state[i-1]=state[i];
  if(state.size()>0) state[state.size()-1]=getEosId(found);
  return lp;
}

//-------------------------
bool KenLm::existSymbol(string s)const
{
  const KlmVocabulary& vocab=modelPtr->GetVocabulary();
  WordIndex w=vocab.Index(s);  
  return (w!=vocab.NotFound());
}

//-------------------------
WordIndex KenLm::addSymbol(string /*s*/)
{
  cerr<<"KenLm: warning, addSymbol() function called but not currently implemented"<<endl;
  return 0;
}

//-------------------------
unsigned int KenLm::getVocabSize(void)
{
  cerr<<"KenLm: warning, getVocabSize() function called but not currently implemented"<<endl;
  return 0;
}

//-------------------------
WordIndex KenLm::stringToWordIndex(string s)const
{
  if(modelPtr!=NULL)
  {
    const KlmVocabulary& vocab=modelPtr->GetVocabulary();
    return vocab.Index(s);
  }
  else
  {
    const KlmVocabulary emptyVocab;
    return emptyVocab.NotFound();
  }
}

//-------------------------
string KenLm::wordIndexToString(WordIndex /*w*/)const
{
  cerr<<"KenLm: warning, wordIndexToString() function called but not currently implemented"<<endl;
  return UNK_SYMBOL_STR;
}

//-------------------------
WordIndex KenLm::getBosId(bool &found)const
{
  found=true;
  const KlmVocabulary& vocab=modelPtr->GetVocabulary();
  return vocab.BeginSentence();
}

//-------------------------
WordIndex KenLm::getEosId(bool &found)const
{
  found=true;
  const KlmVocabulary& vocab=modelPtr->GetVocabulary();
  return vocab.EndSentence();  
}

//-------------------------
bool KenLm::loadVocab(const char* /*fileName*/)
{
  cerr<<"KenLm: warning, loadVocab() function called but not currently implemented"<<endl;
  return THOT_ERROR;
}

//-------------------------
bool KenLm::printVocab(const char* /*fileName*/)
{
  cerr<<"KenLm: warning, printVocab() function called but not currently implemented"<<endl;
  return THOT_ERROR;
}

//-------------------------
void KenLm::clearVocab(void)
{
  cerr<<"KenLm: warning, clearVocab() function called but not currently implemented"<<endl;
}

//-------------------------
bool KenLm::load(const char *fileName)
{
  std::string mainFileName;
  if(fileIsDescriptor(fileName,mainFileName))
  {
    std::string descFileName=fileName;
    std::string absolutizedMainFileName=absolutizeModelFileName(descFileName,mainFileName);
    return load_kenlm_file(absolutizedMainFileName.c_str());
  }
  else
  {
    return load_kenlm_file(fileName);
  }
}

//-------------------------
bool KenLm::load_kenlm_file(const char *fileName)
{
  if(modelPtr==NULL)
  {
    try
    {
          // Define loading configuration
      lm::ngram::Config config;
      config.load_method=util::LAZY;
        
          // Load model
      cerr<<"Loading kenlm model from "<<fileName<<" ..."<<endl;
      modelPtr=new KenLangModel(fileName,config);
    }
    catch(...)
    {
      cerr<<"KenLm: something went wrong while loading "<<fileName<<endl;
      return THOT_ERROR;
    }
    return THOT_OK;
  }
  else
  {
    cerr<<"KenLm: error, a language model was already loaded"<<endl;
    return THOT_ERROR;
  }
}

//-------------------------
bool KenLm::print(const char* /*fileName*/)
{
  cerr<<"KenLm: warning, print() function called but not currently implemented"<<endl;
  return THOT_ERROR;
}

//-------------------------
unsigned int KenLm::getNgramOrder(void)
{
  return modelPtr->Order();
}

//-------------------------
void KenLm::setNgramOrder(int /*_ngramOrder*/)
{
  cerr<<"KenLm: warning, setNgramOrder() function called but not currently implemented"<<endl;
}

//-------------------------
size_t KenLm::size(void)
{
  cerr<<"KenLm: warning, size() function called but not currently implemented"<<endl;
  return 0;
}

//-------------------------
void KenLm::clear(void)
{
  cerr<<"KenLm: warning, clear() function called but not currently implemented"<<endl;
}

//-------------------------
KenLm::~KenLm()
{
  if(modelPtr!=NULL)
    delete modelPtr;
}
