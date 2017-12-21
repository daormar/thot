/*
thot package for statistical machine translation
Copyright (C) 2017 Daniel Ortiz-Mart\'inez

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
/* Definitions file: TranslationMetadataTest.cc                     */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "TranslationMetadataTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TranslationMetadataTest );

//--------------- TranslationMetadataTest class functions
//

//---------------------------------------
void TranslationMetadataTest::setUp()
{
    metadata = new TranslationMetadata();
}

//---------------------------------------
void TranslationMetadataTest::tearDown()
{
    delete metadata;
}

//---------------------------------------
void TranslationMetadataTest::testSafeRecallingObtainTransConstraints()
{
    metadata->clear();

    // Call
    metadata->obtainTransConstraints(getXmlString(), 0);
    std::vector<std::string> srcSentVec1 = metadata->getSrcSentVec();

    // Call again without cleaning
    metadata->obtainTransConstraints(getXmlString(), 0);
    std::vector<std::string> srcSentVec2 = metadata->getSrcSentVec();

    CPPUNIT_ASSERT( srcSentVec1 == srcSentVec2);
}

//---------------------------------------
void TranslationMetadataTest::testGetSrcSentVec()
{
    // Prepare expected answer
    const std::string expectedSrcSent = "First and only T-shirt with logo 22.9cm 2x5";

    metadata->clear();
    metadata->obtainTransConstraints(getXmlString(), 0);

    std::vector<std::string> srcSentVec = metadata->getSrcSentVec();

    CPPUNIT_ASSERT_EQUAL((int) StrProcUtils::stringToStringVector(expectedSrcSent).size(),  (int) srcSentVec.size());
    CPPUNIT_ASSERT_EQUAL(expectedSrcSent, StrProcUtils::stringVectorToString(srcSentVec));
}

//---------------------------------------
void TranslationMetadataTest::testGetTransForSrcPhr()
{
    metadata->obtainTransConstraints(getXmlString(), 0);
    std::vector<std::string> translation = metadata->getTransForSrcPhr(std::make_pair(1, 1));

    std::vector<std::string> expectedTranslation;
    expectedTranslation.push_back("premier");

    CPPUNIT_ASSERT(expectedTranslation == translation);
}

//---------------------------------------
void TranslationMetadataTest::testGetConstrainedSrcPhrases()
{
    // Prepare expected data structure
    std::set<std::pair<PositionIndex,PositionIndex> > expectedConstraints;
    expectedConstraints.insert(std::make_pair(1, 1));
    expectedConstraints.insert(std::make_pair(4, 5));
    expectedConstraints.insert(std::make_pair(6, 6));
    expectedConstraints.insert(std::make_pair(7, 7));
    expectedConstraints.insert(std::make_pair(8, 8));

    // Obtain constraints for a given xml string
    metadata->obtainTransConstraints(getXmlString(), 0);
    std::set<std::pair<PositionIndex,PositionIndex> > constraints = metadata->getConstrainedSrcPhrases();

    CPPUNIT_ASSERT(expectedConstraints == constraints);
}

//---------------------------------------
void TranslationMetadataTest::testSrcPhrAffectedByConstraint()
{
    bool isConstrained;
    // Extract constraints
    metadata->obtainTransConstraints(getXmlString(), 0);

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
void TranslationMetadataTest::testTranslationSatisfiesSrcPhrConstraints(void)
{
  testTranslationSatisfyingSrcPhrConstraints();
  testTranslationViolatingSrcSegmentationConstraints();
  testTranslationViolatingTrgSegmentationConstraints();
  testTranslationViolatingWordSelectionConstraints();
}

//---------------------------------------
void TranslationMetadataTest::testTranslationSatisfyingSrcPhrConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getXmlString(), 0);

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
void TranslationMetadataTest::testTranslationViolatingSrcSegmentationConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getXmlString(), 0);

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
void TranslationMetadataTest::testTranslationViolatingTrgSegmentationConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getXmlString(), 0);

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
void TranslationMetadataTest::testTranslationViolatingWordSelectionConstraints(void)
{
    // Extract constraints
    metadata->obtainTransConstraints(getXmlString(), 0);

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
std::string TranslationMetadataTest::getXmlString(void)
{
  return "<phr_pair_annot><src_segm>First</src_segm><trg_segm>premier</trg_segm></phr_pair_annot> and only <phr_pair_annot><src_segm>T-shirt with</src_segm><trg_segm>t-shirt avec</trg_segm></phr_pair_annot> <phr_pair_annot><src_segm>logo</src_segm><trg_segm>Logo</trg_segm></phr_pair_annot> <phr_pair_annot><src_segm>22.9cm</src_segm><trg_segm>22.9cm</trg_segm></phr_pair_annot> <phr_pair_annot><src_segm>2x5</src_segm><trg_segm>2x5</trg_segm></phr_pair_annot>";
}
