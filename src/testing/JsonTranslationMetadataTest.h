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
/* Module: JsonTranslationMetadataTest                              */
/*                                                                  */
/* Prototypes file: JsonTranslationMetadataTest.h                   */
/*                                                                  */
/* Description: Declares the JsonTranslationMetadataTest class      */
/*              implementing unit tests for                         */
/*              the JsonTranslationMetadata class.                  */
/*                                                                  */
/********************************************************************/

/**
 * @file JsonTranslationMetadataTest.h
 *
 * @brief Declares the JsonTranslationMetadataTest class implementing
 * unit tests for the JsonTranslationMetadata class.
 */

#ifndef _JsonTranslationMetadataTest_h
#define _JsonTranslationMetadataTest_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "JsonTranslationMetadata.h"
#include "PhrScoreInfo.h"
#include "StrProcUtils.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- JsonTranslationMetadata class

/**
 * @brief Class implementing tests for JsonTranslationMetadata.
 */

class JsonTranslationMetadataTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( JsonTranslationMetadataTest );
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
  JsonTranslationMetadata<PhrScoreInfo>* metadata;

      // Input data generators
  std::string getJsonString(void);

      // Auxiliary functions to test source phrase constraints
  void testTranslationSatisfyingSrcPhrConstraints(void);
  void testTranslationViolatingSrcSegmentationConstraints(void);
  void testTranslationViolatingTrgSegmentationConstraints(void);
  void testTranslationViolatingWordSelectionConstraints(void);
};

#endif
