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
#include "PhrScoreInfo.h"
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
  void setUp(void);
  void tearDown(void);

  void testSafeRecallingObtainTransConstraints(void);
  void testGetSrcSentVec(void);
  void testGetTransForSrcPhr(void);
  void testGetConstrainedSrcPhrases(void);
  void testSrcPhrAffectedByConstraint(void);
  void testTranslationSatisfiesSrcPhrConstraints(void);

 private:
  TranslationMetadata<PhrScoreInfo>* metadata;

      // Input data generators
  std::string getXmlString(void);

      // Auxiliary functions to test source phrase constraints
  void testTranslationSatisfyingSrcPhrConstraints(void);
  void testTranslationViolatingSrcSegmentationConstraints(void);
  void testTranslationViolatingTrgSegmentationConstraints(void);
  void testTranslationViolatingWordSelectionConstraints(void);

};

#endif
