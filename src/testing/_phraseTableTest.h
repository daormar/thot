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
/* Module: _phraseTableTest                                         */
/*                                                                  */
/* Prototypes file: _phraseTableTest.h                              */
/*                                                                  */
/* Description: Declares the _phraseTableTest abstract class        */
/*              implementing common unit tests for the phrase       */
/*              table classes.                                      */
/*                                                                  */
/********************************************************************/

/**
 * @file _phraseTableTest.h
 *
 * @brief Declares the _phraseTableTest abstract class implementing
 * common unit tests for the phrase table classes.
 */

#ifndef __phraseTableTest_h
#define __phraseTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "BasePhraseTable.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

//--------------- _phraseTable template class

/**
 * @brief Abstract class implementing common tests for phrase tables.
 */

class _phraseTableTest: public CppUnit::TestFixture
{
    protected:
        BasePhraseTable *tab;

        std::vector<WordIndex> getVector(string phrase);

    public:
        virtual void setUp() = 0;
        virtual void tearDown() = 0;

        void testStoreAndRestore();
        void testAddTableEntry();
        void testIncCountsOfEntry();
        void testGetEntriesForTarget();
        void testRetrievingSubphrase();
        void testRetrieveNonLeafPhrase();
        void testGetEntriesForSource();
        void testRetrievingEntriesWithCountEqualZero();
        void testGetNbestForTrg();
        void testAddSrcTrgInfo();
        //void testIteratorsLoop();
        //void testIteratorsOperatorsPlusPlusStar();
        //void testIteratorsOperatorsEqualNotEqual();
        void testSize();
        void testSubkeys();
        void test32bitRange();
        void testByteMax();
};

#endif