#ifndef _BaseMiraScorer_h
#define _BaseMiraScorer_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */


#include "myVector.h"
#include <string>


/**
 * @brief Base class to implement MIRA scorers.
 */
class BaseMiraScorer
{
public:
    // Functions to manage background corpus
  virtual void resetBackgroundCorpus()=0;
  virtual void updateBackgroundCorpus(const Vector<unsigned int>& stats)=0;

    // Score for sentence with background corpus stats
  virtual void sentBackgroundScore(const std::string& candidate,
                                   const std::string& reference,
                                   double& score,
                                   Vector<unsigned int>& stats)=0;

    // Score for corpus
  virtual void Score(const Vector<std::string>& candidates,
                     const Vector<std::string>& references,
                     double& score)=0;

    // Score for sentence
  virtual void sentScore(const std::string& candidate,
                         const std::string& reference,
                         double& score)=0;

    // Destructor
  virtual ~BaseMiraScorer(){};
};

#endif
