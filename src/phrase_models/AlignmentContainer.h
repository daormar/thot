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
/* Module: AlignmentContainer                                       */
/*                                                                  */
/* Prototype file: AlignmentContainer                               */
/*                                                                  */
/* Description: Defines the AlignmentContainer class, for storing   */
/*              phrase alignments and doing operations (like        */
/*              symmetrization) over them.                          */
/*                                                                  */
/********************************************************************/

#ifndef _AlignmentContainer
#define _AlignmentContainer

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "AlignmentExtractor.h"
#include "PhraseDefs.h"
#include "AligInfo.h"
#include <printAligFuncs.h>
#include "AwkInputStream.h"
#include <map>
#include <vector>

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- Classes
class AlignmentContainer;

//--------------- function declarations ------------------------------

std::ostream& operator << (std::ostream &outS,const AlignmentContainer &ac);

//--------------- AlignmentContainer class
class AlignmentContainer 
{
 public:
			
	AlignmentContainer(void);
	bool extractAlignmentsFromGIZAFile(const char *_GizaAligFileName,
                                       bool transpose=0);
        // Stores the alignments contained in a GIZA++ alignment file
	bool join(const char *_GizaAligFileName,
              bool transpose=0);
        // joins the alignment matrixes given in the GIZA file with
        // those contained in the map aligCont
	bool intersect(const char *_GizaAligFileName,
                   bool transpose=0);
        // intersects the alignment matrixes
    bool sum(const char *_GizaAligFileName,
             bool transpose=0);
        // Obtains the sum of the alignment matrixes
	bool symmetr1(const char *_GizaAligFileName,
                  bool transpose=0);
	bool symmetr2(const char *_GizaAligFileName,
                  bool transpose=0);
	bool growDiagFinal(const char *_GizaAligFileName,
                       bool transpose=0);
	std::vector<unsigned int>
      vecString2VecUnsigInt(std::vector<std::string> vStr,
                            std::map<std::string,unsigned int> & vocab,
                            std::vector<std::string> & vocabInv)const;
	std::vector<std::string> vecUnsigInt2VecString(std::vector<unsigned int> vInt,
                                              const std::vector<std::string> & vocabInv)const;
	void clear(void);
	bool printNoCompact(std::ostream &outS);
    bool printNoCompact(FILE *file);
	friend std::ostream& operator << (std::ostream &outS,const AlignmentContainer &ac);
    void printCompact(FILE *file);

 protected:

    std::map<std::vector<unsigned int>,std::vector<AligInfo>,VecUnsignedIntSortCriterion> aligCont;
	std::map<std::string,unsigned int> sVocab;
	std::map<std::string,unsigned int> tVocab;
	std::vector<std::string> sVocabInv;
	std::vector<std::string> tVocabInv;
    char GizaAligFileName[256];	
	unsigned long numAlignments;
};

#endif
