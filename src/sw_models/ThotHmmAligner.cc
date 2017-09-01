/*
thot package for statistical machine translation
Copyright (C) 2013 Germ\'an Sanchis, Daniel Ortiz-Mart\'inez
 
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

//--------------- Include files --------------------------------------

#include "ThotHmmAligner.h"

//--------------- ThotHmmAligner class function definitions

//-------------------------
void ThotHmmAligner::align(const vector<string> &source,
                        const vector<string> &target,
                        vector< vector<float> > &alignments)
{
  WordAligMatrix w;
  if (source.size() == 0 || target.size() == 0) {
    LOG(INFO) << "WARNING: ThotHmmAligner received an empty source or target sentence!!" << std::endl << "WARNING: Returning empty alignment matrix!" << std::endl;
    alignments.resize(0);
    return;
  }
  aligModel.obtainBestAlignment( aligModel.strVectorToSrcIndexVector(source), aligModel.strVectorToTrgIndexVector(target), w );
  
  alignments.resize(source.size());
  for (size_t s=0; s<source.size(); ++s) {
	alignments[s].resize(target.size());
	for (size_t t=0; t<target.size(); ++t) {
	  alignments[s][t] = w.getValue(s,t);
	}
  }
}

//-------------------------
int ThotHmmAligner::init(char* filesPrefix)
{
  if (aligModel.load(filesPrefix) == 0) { // 0 means THOT_OK
    LOG(INFO) << "Alignment model with prefix " << filesPrefix << "was loaded successfully!" << std::endl;
  } else {
    LOG(THOT_ERROR) << "Unable to open alignment model with prefix " << filesPrefix << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
