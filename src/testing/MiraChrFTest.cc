/*
thot package for statistical machine translation

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

#include "MiraChrFTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION( MiraChrFTest );

void MiraChrFTest::setUp()
{
    chrf_metric = new MiraChrF();

    system_sentences.push_back("colourless green ideas sleep furiously");
    system_sentences.push_back("");
    system_sentences.push_back("colourless green ideas sleep furiously");
    system_sentences.push_back("");
    system_sentences.push_back(".");
    system_sentences.push_back("colourless GREEN ideas sleep furiously");
    system_sentences.push_back("colourless green ideas sleep furiously");
    system_sentences.push_back("colorless greeny idea sleeps furious");
    system_sentences.push_back("áéíóúûüмариночкалучшевсех");

    reference_sentences.push_back("colourless green ideas sleep furiously");
    reference_sentences.push_back("colourless green ideas sleep furiously");
    reference_sentences.push_back("");
    reference_sentences.push_back("");
    reference_sentences.push_back(".");
    reference_sentences.push_back("Colourless green ideas sleep furiously");
    reference_sentences.push_back("colourless green ideas sleepfuriously");
    reference_sentences.push_back("colourless green ideas sleep furiously");
    reference_sentences.push_back("áéíóúûüмариночкалучшевсех");

}

//---------------------------------------
void MiraChrFTest::tearDown()
{
    delete chrf_metric;
}

// This test works with the following metric parameters defined in chrf.h:
// MAX_NGRAM_LENGTH 4
// BETA 3
// CONSIDER_WHITESPACE true

void MiraChrFTest::testSentenceLevel()
{
    double score;
    // Candidate and reference are exactly the same
    chrf_metric->sentScore(system_sentences[0], reference_sentences[0], score);
    CPPUNIT_ASSERT(score == 1.0);

    // Reference is empty, candidate is not
    chrf_metric->sentScore(system_sentences[1], reference_sentences[1], score);
    CPPUNIT_ASSERT(score == 0.0);

    // Candidate is empty, reference is not
    chrf_metric->sentScore(system_sentences[2], reference_sentences[2], score);
    CPPUNIT_ASSERT(score == 0.0);

    // Both candidate and reference are empty
    chrf_metric->sentScore(system_sentences[3], reference_sentences[3], score);
    CPPUNIT_ASSERT(score == 1.0);

    // Candidate and reference contain only one exactly matching character
    chrf_metric->sentScore(system_sentences[4], reference_sentences[4], score);
    CPPUNIT_ASSERT(score == 1.0);

    // Candidate and reference differ only in casing
    chrf_metric->sentScore(system_sentences[5], reference_sentences[5], score);
    CPPUNIT_ASSERT(score == 1.0);

    // Candidate and reference differ only in white space
    chrf_metric->sentScore(system_sentences[6], reference_sentences[6], score);
    CPPUNIT_ASSERT(floor(score*100)/100 == 0.95);

    // Candidate and reference differ in word forms
    chrf_metric->sentScore(system_sentences[7], reference_sentences[7], score);
    CPPUNIT_ASSERT(floor(score*100)/100 == 0.74);

    // Candidate and reference contain non-latin characters and characters with diacritics
    chrf_metric->sentScore(system_sentences[8], reference_sentences[8], score);
    CPPUNIT_ASSERT(score == 1.0);

}

void MiraChrFTest::testCorpusLevel()
{
    double score;
    chrf_metric->corpusScore(system_sentences, reference_sentences, score);
    CPPUNIT_ASSERT(floor(score*100)/100 == 0.74);
}