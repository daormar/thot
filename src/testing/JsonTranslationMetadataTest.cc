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
/* Definitions file: JsonTranslationMetadataTest.cc                 */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "JsonTranslationMetadataTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( JsonTranslationMetadataTest );

//--------------- JsonTranslationMetadataTest class functions
//

//---------------------------------------
void JsonTranslationMetadataTest::setUp(void)
{
    metadata = new JsonTranslationMetadata<PhrScoreInfo>();
}

//---------------------------------------
void JsonTranslationMetadataTest::tearDown(void)
{
    delete metadata;
}

//---------------------------------------
void JsonTranslationMetadataTest::testSafeRecallingObtainTransConstraints(void)
{
    metadata->clear();

    // Call
    metadata->obtainTransConstraints(getJsonString(), 0);
    std::vector<std::string> srcSentVec1 = metadata->getSrcSentVec();

    // Call again without cleaning
    metadata->obtainTransConstraints(getJsonString(), 0);
    std::vector<std::string> srcSentVec2 = metadata->getSrcSentVec();

    CPPUNIT_ASSERT( srcSentVec1 == srcSentVec2);
}

//---------------------------------------
void JsonTranslationMetadataTest::testGetSrcSentVec(void)
{
    // Prepare expected answer
    const std::string expectedSrcSent = "First and only T-shirt with logo 22.9cm 2x5";

    metadata->clear();
    metadata->obtainTransConstraints(getJsonString(), 0);

    std::vector<std::string> srcSentVec = metadata->getSrcSentVec();

    CPPUNIT_ASSERT_EQUAL(8,  (int) srcSentVec.size());
    CPPUNIT_ASSERT_EQUAL(expectedSrcSent, StrProcUtils::stringVectorToString(srcSentVec));
}

//---------------------------------------
void JsonTranslationMetadataTest::testGetTransForSrcPhr(void)
{
    metadata->obtainTransConstraints(getJsonString(), 0);
    std::vector<std::string> translation = metadata->getTransForSrcPhr(std::make_pair(1, 1));

    std::vector<std::string> expectedTranslation;
    expectedTranslation.push_back("premier");

    CPPUNIT_ASSERT(expectedTranslation == translation);
}

//---------------------------------------
void JsonTranslationMetadataTest::testGetConstrainedSrcPhrases(void)
{
    // Prepare expected data structure
    std::set<std::pair<PositionIndex,PositionIndex> > expectedConstraints;
    expectedConstraints.insert(std::make_pair(1, 1));
    expectedConstraints.insert(std::make_pair(4, 5));
    expectedConstraints.insert(std::make_pair(6, 6));
    expectedConstraints.insert(std::make_pair(7, 7));
    expectedConstraints.insert(std::make_pair(8, 8));

    // Obtain constraints for a given JSON
    metadata->obtainTransConstraints(getJsonString(), 0);
    std::set<std::pair<PositionIndex,PositionIndex> > constraints = metadata->getConstrainedSrcPhrases();

    CPPUNIT_ASSERT(expectedConstraints == constraints);
}

//---------------------------------------
void JsonTranslationMetadataTest::testSrcPhrAffectedByConstraint(void)
{
    bool isConstrained;
    // Extract constraints
    metadata->obtainTransConstraints(getJsonString(), 0);

    isConstrained = metadata->srcPhrAffectedByConstraint(std::make_pair(1, 1));
    CPPUNIT_ASSERT_EQUAL(true, isConstrained);

    isConstrained = metadata->srcPhrAffectedByConstraint(std::make_pair(1, 2));
    CPPUNIT_ASSERT_EQUAL(true, isConstrained);

    isConstrained = metadata->srcPhrAffectedByConstraint(std::make_pair(2, 2));
    CPPUNIT_ASSERT_EQUAL(false, isConstrained);

    isConstrained = metadata->srcPhrAffectedByConstraint(std::make_pair(5, 7));
    CPPUNIT_ASSERT_EQUAL(true, isConstrained);

    isConstrained = metadata->srcPhrAffectedByConstraint(std::make_pair(6, 8));
    CPPUNIT_ASSERT_EQUAL(true, isConstrained);
}

//---------------------------------------
void JsonTranslationMetadataTest::testTranslationSatisfiesSrcPhrConstraints(void)
{
  testTranslationSatisfyingSrcPhrConstraints();
  testTranslationViolatingSrcSegmentationConstraints();
  testTranslationViolatingTrgSegmentationConstraints();
  testTranslationViolatingWordSelectionConstraints();
}

//---------------------------------------
void JsonTranslationMetadataTest::testTranslationSatisfyingSrcPhrConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getJsonString(), 0);

    // Prepare parameters
    SourceSegmentation sourceSegmentation;
    sourceSegmentation.push_back(std::make_pair(1, 1));
    sourceSegmentation.push_back(std::make_pair(2, 3));
    sourceSegmentation.push_back(std::make_pair(4, 5));
    sourceSegmentation.push_back(std::make_pair(6, 6));
    sourceSegmentation.push_back(std::make_pair(7, 7));
    sourceSegmentation.push_back(std::make_pair(8, 8));

    std::vector<PositionIndex> targetSegmentCuts;
    targetSegmentCuts.push_back(1);
    targetSegmentCuts.push_back(3);
    targetSegmentCuts.push_back(5);
    targetSegmentCuts.push_back(6);
    targetSegmentCuts.push_back(7);
    targetSegmentCuts.push_back(8);

    std::vector<std::string> targetWordVec;
    targetWordVec.push_back("premier");
    targetWordVec.push_back("et");
    targetWordVec.push_back("Only");
    targetWordVec.push_back("t-shirt");
    targetWordVec.push_back("avec");
    targetWordVec.push_back("Logo");
    targetWordVec.push_back("22.9cm");
    targetWordVec.push_back("2x5");

    // Valid translation - respects all constraints
    bool isSatisfied = metadata->translationSatisfiesConstraints(sourceSegmentation, targetSegmentCuts, targetWordVec);
    CPPUNIT_ASSERT_EQUAL(true, isSatisfied);
}

//---------------------------------------
void JsonTranslationMetadataTest::testTranslationViolatingSrcSegmentationConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getJsonString(), 0);

    // Prepare parameters
    SourceSegmentation sourceSegmentation;
    sourceSegmentation.push_back(std::make_pair(1, 2));
    sourceSegmentation.push_back(std::make_pair(3, 3));
    sourceSegmentation.push_back(std::make_pair(4, 5));
    sourceSegmentation.push_back(std::make_pair(6, 6));
    sourceSegmentation.push_back(std::make_pair(7, 7));
    sourceSegmentation.push_back(std::make_pair(8, 8));

    std::vector<PositionIndex> targetSegmentCuts;
    targetSegmentCuts.push_back(1);
    targetSegmentCuts.push_back(3);
    targetSegmentCuts.push_back(5);
    targetSegmentCuts.push_back(6);
    targetSegmentCuts.push_back(7);
    targetSegmentCuts.push_back(8);

    std::vector<std::string> targetWordVec;
    targetWordVec.push_back("premier");
    targetWordVec.push_back("et");
    targetWordVec.push_back("Only");
    targetWordVec.push_back("t-shirt");
    targetWordVec.push_back("avec");
    targetWordVec.push_back("Logo");
    targetWordVec.push_back("22.9cm");
    targetWordVec.push_back("2x5");

    // Valid translation - respects all constraints
    bool isSatisfied = metadata->translationSatisfiesConstraints(sourceSegmentation, targetSegmentCuts, targetWordVec);
    CPPUNIT_ASSERT_EQUAL(false, isSatisfied);
}

//---------------------------------------
void JsonTranslationMetadataTest::testTranslationViolatingTrgSegmentationConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getJsonString(), 0);

    // Prepare parameters
    SourceSegmentation sourceSegmentation;
    sourceSegmentation.push_back(std::make_pair(1, 1));
    sourceSegmentation.push_back(std::make_pair(2, 3));
    sourceSegmentation.push_back(std::make_pair(4, 5));
    sourceSegmentation.push_back(std::make_pair(6, 6));
    sourceSegmentation.push_back(std::make_pair(7, 7));
    sourceSegmentation.push_back(std::make_pair(8, 8));

    std::vector<PositionIndex> targetSegmentCuts;
    targetSegmentCuts.push_back(2);
    targetSegmentCuts.push_back(3);
    targetSegmentCuts.push_back(5);
    targetSegmentCuts.push_back(6);
    targetSegmentCuts.push_back(7);
    targetSegmentCuts.push_back(8);

    std::vector<std::string> targetWordVec;
    targetWordVec.push_back("premier");
    targetWordVec.push_back("et");
    targetWordVec.push_back("Only");
    targetWordVec.push_back("t-shirt");
    targetWordVec.push_back("avec");
    targetWordVec.push_back("Logo");
    targetWordVec.push_back("22.9cm");
    targetWordVec.push_back("2x5");

    // Valid translation - respects all constraints
    bool isSatisfied = metadata->translationSatisfiesConstraints(sourceSegmentation, targetSegmentCuts, targetWordVec);
    CPPUNIT_ASSERT_EQUAL(false, isSatisfied);
}

//---------------------------------------
void JsonTranslationMetadataTest::testTranslationViolatingWordSelectionConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getJsonString(), 0);

    // Prepare parameters
    SourceSegmentation sourceSegmentation;
    sourceSegmentation.push_back(std::make_pair(1, 1));
    sourceSegmentation.push_back(std::make_pair(2, 3));
    sourceSegmentation.push_back(std::make_pair(4, 5));
    sourceSegmentation.push_back(std::make_pair(6, 6));
    sourceSegmentation.push_back(std::make_pair(7, 7));
    sourceSegmentation.push_back(std::make_pair(8, 8));

    std::vector<PositionIndex> targetSegmentCuts;
    targetSegmentCuts.push_back(1);
    targetSegmentCuts.push_back(3);
    targetSegmentCuts.push_back(5);
    targetSegmentCuts.push_back(6);
    targetSegmentCuts.push_back(7);
    targetSegmentCuts.push_back(8);

    std::vector<std::string> targetWordVec;
    targetWordVec.push_back("premier");
    targetWordVec.push_back("XXXXXXXX");
    targetWordVec.push_back("Only");
    targetWordVec.push_back("t-shirt");
    targetWordVec.push_back("avec");
    targetWordVec.push_back("Logo");
    targetWordVec.push_back("22.9cm");
    targetWordVec.push_back("2x5");

    // Valid translation - respects all constraints
    bool isSatisfied = metadata->translationSatisfiesConstraints(sourceSegmentation, targetSegmentCuts, targetWordVec);
    CPPUNIT_ASSERT_EQUAL(true, isSatisfied);
}

//---------------------------------------
std::string JsonTranslationMetadataTest::getJsonString(void)
{
  return "{"
    "	\"src_title\" : {"
    "		\"preprocessed\" : \"First and only T-shirt with logo 22.9cm 2x5\","
    "		\"original\" : \"First and only T-shirt with logo 9\\\" 2x5 718$/L\""
    "	},"
    "	\"tex_segmentation\" : ["
    "		{ \"translation\" : \"premier\", \"original\" : \"First\" },"
    "		{ \"translation\" : \"\", \"original\" : \"and only\" },"
    "		{ \"translation\" : \"<should-not-appear>\", \"original\" : \" \" },"
    "		{ \"translation\" : \"t-shirt avec\", \"original\" : \"T-shirt with\" },"
    "		{ \"translation\" : \"Logo\", \"original\" : \"logo\" },"
    "		{ \"translation\" : \"22.9cm\", \"original\" : \"22.9cm\" },"
    "		{ \"translation\" : \"2x5\", \"original\" : \"2x5\" }"
    "	]"
    "}";
}
