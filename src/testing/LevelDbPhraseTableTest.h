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
/* Module: LevelDbPhraseTableTest                                   */
/*                                                                  */
/* Prototypes file: LevelDbPhraseTableTest.h                        */
/*                                                                  */
/* Description: Declares the LevelDbPhraseTableTest class           */
/*              implementing unit tests for the LevelDbPhraseTable  */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

/**
 * @file LevelDbPhraseTableTest.h
 *
 * @brief Declares the LevelDbPhraseTable class implementing unit tests
 * for the LevelDbPhraseTable class.
 */

#ifndef _LevelDbPhraseTableTest_h
#define _LevelDbPhraseTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "LevelDbPhraseTable.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- LevelDbPhraseTable template class

/**
 * @brief Class implementing tests for LevelDbPhraseTable.
 */

class LevelDbPhraseTableTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( LevelDbPhraseTableTest );
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
  CPPUNIT_TEST( testLoadingLevelDb );
  CPPUNIT_TEST( testLoadedDataCorrectness );
  CPPUNIT_TEST( testSubkeys );
  CPPUNIT_TEST( test32bitRange );
  CPPUNIT_TEST( testByteMax );
  CPPUNIT_TEST_SUITE_END();

 private:
  LevelDbPhraseTable *tab;
  string dbName = "/tmp/thot_leveldb_unit_test";
  
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
  void testLoadingLevelDb();
  void testLoadedDataCorrectness();
  void testSubkeys();
  void test32bitRange();
  void testByteMax();
};

#endif