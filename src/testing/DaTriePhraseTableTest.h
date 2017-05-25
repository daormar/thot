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
/* Module: DaTriePhraseTableTest                                    */
/*                                                                  */
/* Prototypes file: DaTriePhraseTableTest.h                         */
/*                                                                  */
/* Description: Declares the DaTriePhraseTableTest class            */
/*              implementing unit tests for the DaTriePhraseTable   */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

/**
 * @file DaTriePhraseTableTest.h
 *
 * @brief Declares the DaTriePhraseTable class implementing unit tests
 * for the DaTriePhraseTable class.
 */

#ifndef _DaTriePhraseTableTest_h
#define _DaTriePhraseTableTest_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "DaTriePhraseTable.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- DaTriePhraseTable template class

/**
 * @brief Class implementing tests for DaTriePhraseTable.
 */

class DaTriePhraseTableTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( DaTriePhraseTableTest );
  CPPUNIT_TEST( testAddTableEntry );
  CPPUNIT_TEST( testIncCountsOfEntry );
  CPPUNIT_TEST( testStoreAndRestore );
  CPPUNIT_TEST( testGetEntriesForTarget );
  CPPUNIT_TEST( testRetrievingSubphrase );
  CPPUNIT_TEST( testRetrieveNonLeafPhrase );
  //CPPUNIT_TEST( testGetEntriesForSource );
  CPPUNIT_TEST( testRetrievingEntriesWithCountEqualZero );
  CPPUNIT_TEST( testGetNbestForTrg );
  CPPUNIT_TEST( testAddSrcTrgInfo );
  CPPUNIT_TEST( testIteratorsLoop );
  CPPUNIT_TEST( testIteratorsOperatorsPlusPlusStar );
  CPPUNIT_TEST( testIteratorsOperatorsEqualNotEqual );
  CPPUNIT_TEST( testSize );
  CPPUNIT_TEST( testSavingAndRestoringTrie );
  CPPUNIT_TEST( testMmap );
  CPPUNIT_TEST( testSubkeys );
  CPPUNIT_TEST( test32bitRange );
  CPPUNIT_TEST_SUITE_END();

 private:
  DaTriePhraseTable *tab;
  
  Vector<WordIndex> getVector(string phrase);

 public:
  void setUp();
  void tearDown();

  void testAddTableEntry();
  void testIncCountsOfEntry();
  void testStoreAndRestore();
  void testGetEntriesForTarget();
  void testRetrievingSubphrase();
  void testRetrieveNonLeafPhrase();
  void testGetEntriesForSource();
  void testRetrievingEntriesWithCountEqualZero();
  void testGetNbestForTrg();
  void testAddSrcTrgInfo();
  void testIteratorsLoop();
  void testIteratorsOperatorsPlusPlusStar();
  void testIteratorsOperatorsEqualNotEqual();
  void testSize();
  void testSavingAndRestoringTrie();
  void testMmap();
  void testSubkeys();
  void test32bitRange();
};

#endif
