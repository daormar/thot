/*
thot package for statistical machine translation
Copyright (C) 2017 Adam Harasimowicz

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
/* Module: TranslationMetadataTest                                  */
/*                                                                  */
/* Prototypes file: TranslationMetadataTest.h                       */
/*                                                                  */
/* Description: Declares the TranslationMetadataTest class          */
/*              implementing unit tests for                         */
/*              the TranslationMetadata class.                      */
/*                                                                  */
/********************************************************************/

/**
 * @file TranslationMetadataTest.h
 *
 * @brief Declares the TranslationMetadataTest class implementing
 * unit tests for the TranslationMetadata class.
 */

#ifndef _TranslationMetadataTest_h
#define _TranslationMetadataTest_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "TranslationMetadata.h"
#include "StrProcUtils.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- TranslationMetadata class

/**
 * @brief Class implementing tests for TranslationMetadata.
 */

class TranslationMetadataTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( TranslationMetadataTest );
  CPPUNIT_TEST( testSafeRecallingObtainTransConstraints );
  CPPUNIT_TEST( testGetSrcSentVec );
  CPPUNIT_TEST( testGetTransForSrcPhr );
  CPPUNIT_TEST( testGetConstrainedSrcPhrases );
  CPPUNIT_TEST( testSrcPhrAffectedByConstraint );
  CPPUNIT_TEST( testTranslationSatisfiesSrcPhrConstraints );
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();

  void testSafeRecallingObtainTransConstraints();
  void testGetSrcSentVec();
  void testGetTransForSrcPhr();
  void testGetConstrainedSrcPhrases();
  void testSrcPhrAffectedByConstraint();
  void testTranslationSatisfiesSrcPhrConstraints();

 private:
  TranslationMetadata* metadata;
  void testTranslationSatisfyingSrcPhrConstraints();
  void testTranslationViolatingSrcSegmentationConstraints();
  void testTranslationViolatingTrgSegmentationConstraints();
  void testTranslationViolatingWordSelectionConstraints();

  const std::string xmlStr = "<phr_pair_annot><src_segm>First</src_segm><trg_segm>premier</trg_segm></phr_pair_annot> and only <phr_pair_annot><src_segm>T-shirt with</src_segm><trg_segm>t-shirt avec</trg_segm></phr_pair_annot> <phr_pair_annot><src_segm>logo</src_segm><trg_segm>Logo</trg_segm></phr_pair_annot> <phr_pair_annot><src_segm>22.9cm</src_segm><trg_segm>22.9cm</trg_segm></phr_pair_annot> <phr_pair_annot><src_segm>2x5</src_segm><trg_segm>2x5</trg_segm></phr_pair_annot>";

};

#endif
