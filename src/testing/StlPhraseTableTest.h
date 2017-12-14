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
/* Module: StlPhraseTableTest                                       */
/*                                                                  */
/* Prototypes file: StlPhraseTableTest.h                            */
/*                                                                  */
/* Description: Declares the StlPhraseTableTest class implementing  */
/*               unit tests for the StlPhraseTable class.           */
/*                                                                  */
/********************************************************************/

/**
 * @file StlPhraseTableTest.h
 *
 * @brief Declares the StlPhraseTableTest class implementing unit tests
 * for the StlPhraseTable class.
 */

#ifndef _StlPhraseTableTest_h
#define _StlPhraseTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"
#include "MathDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_phraseTableTest.h"
#include "StlPhraseTable.h"

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

//--------------- StlPhraseTable template class

/**
 * @brief Class implementing tests for StlPhraseTable.
 */

class StlPhraseTableTest: public _phraseTableTest
{
    CPPUNIT_TEST_SUITE( StlPhraseTableTest );
    CPPUNIT_TEST( testAddTableEntry );
    CPPUNIT_TEST( testIncCountsOfEntry );
    CPPUNIT_TEST( testStoreAndRestore );
    CPPUNIT_TEST( testGetEntriesForTarget );
    CPPUNIT_TEST( testRetrievingSubphrase );
    CPPUNIT_TEST( testRetrieveNonLeafPhrase );
    CPPUNIT_TEST( testGetEntriesForSource );
    CPPUNIT_TEST( testRetrievingEntriesWithCountEqualZero );
    CPPUNIT_TEST( testGetNbestForTrg );
    CPPUNIT_TEST( testAddSrcTrgInfo );
    CPPUNIT_TEST( testPSrcGivenTrg );
    CPPUNIT_TEST( testPTrgGivenSrc );
    CPPUNIT_TEST( testIteratorsLoop );
    CPPUNIT_TEST( testIteratorsOperatorsPlusPlusStar );
    CPPUNIT_TEST( testIteratorsOperatorsEqualNotEqual );
    CPPUNIT_TEST( testAddingSameSrcAndTrg );
    CPPUNIT_TEST( testSize );
    CPPUNIT_TEST( testSubkeys );
    CPPUNIT_TEST( test32bitRange );
    CPPUNIT_TEST( testByteMax );
    CPPUNIT_TEST( testByteMin );
    CPPUNIT_TEST_SUITE_END();

    private:
        StlPhraseTable* tabStl;  // Used for avoiding casting - should point on the same object like tab in parent class

    public:
        void setUp();
        void tearDown();

        void testAddSrcTrgInfo();
        void testIteratorsLoop();
        void testIteratorsOperatorsPlusPlusStar();
        void testIteratorsOperatorsEqualNotEqual();
};

#endif 