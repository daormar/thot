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
 * @file WgHandler.h
 *
 * @brief Declares the WgHandler class. This class is used to associate
 * a set of source sentences with a set of wordgraphs.
 */

#ifndef _WgHandler_h
#define _WgHandler_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <WordGraph.h>
#include "AwkInputStream.h"

//--------------- Constants ------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- WgHandler class

/**
 * @brief The WgHandler class implements a word graph for being
 * used in stack decoding.
 */

class WgHandler
{
 public:

      // Constructor
  WgHandler(void);

      // Basic functions
  std::string pathAssociatedToSentence(const std::vector<std::string>& strVec,
                                       bool& found)const;

      // size related functions
  bool empty(void)const;
  size_t size(void)const;

      // Functions to load sentence-wordgraph info
  bool load(const char * filename);

      // Functions to print sentence-wordgraph info
  bool print(const char* filename)const;
  void print(std::ostream &outS)const;

      // clear() function
  void clear(void);

  ~WgHandler();
  
 protected:
  typedef std::string WgInfo;
  typedef std::map<std::vector<std::string>,WgInfo> SentToWgInfoMap;
  
  SentToWgInfoMap sentToWgInfoMap;
};

#endif
