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
 
#ifndef _PhraseModelPars_h
#define _PhraseModelPars_h

//--------------- Constants ------------------------------------------

#define DEFAULT_PTS_WEIGHT 1.0
#define DEFAULT_PST_WEIGHT 0.000001

//--------------- PhraseModelPars struct

struct PhraseModelPars
{
  std::string srcTrainVocabFileName;
  std::string trgTrainVocabFileName;
  std::string readTablePrefix;
  std::vector<double> ptsWeightVec;
  std::vector<double> pstWeightVec;
  double trgSegmLenWeight;
  double srcJumpWeight;
  double srcSegmLenWeight;

      // Constructor
  PhraseModelPars(void)
    {
      trgSegmLenWeight=1.0;
      srcJumpWeight=0.01;
      srcSegmLenWeight=1.0;
    };
};

#endif
