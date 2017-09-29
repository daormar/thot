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
 * @brief Declares the LevelDbPhraseTableTest class implementing unit
 * tests for the LevelDbPhraseTable class.
 */

#ifndef _LevelDbPhraseTableTest_h
#define _LevelDbPhraseTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_phraseTableTest.h"
#include "LevelDbPhraseTable.h"

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- LevelDbPhraseTable template class

/**
 * @brief Class implementing tests for LevelDbPhraseTable.
 */

class LevelDbPhraseTableTest: public _phraseTableTest
{
  CPPUNIT_TEST_SUITE( LevelDbPhraseTableTest );
  CPPUNIT_TEST( testKeyVectorConversion );
  CPPUNIT_TEST( testAddTableEntry );
  CPPUNIT_TEST( testIncCountsOfEntry );
  CPPUNIT_TEST( testStoreAndRestore );
  CPPUNIT_TEST( testGetEntriesForTarget );
  CPPUNIT_TEST( testRetrievingSubphrase );
  CPPUNIT_TEST( testRetrieveNonLeafPhrase );
  //CPPUNIT_TEST( testGetEntriesForSource );  // Not implemented yet functionality for LevelDB
  CPPUNIT_TEST( testRetrievingEntriesWithCountEqualZero );
  CPPUNIT_TEST( testGetNbestForTrg );
  CPPUNIT_TEST( testAddSrcTrgInfo );
  CPPUNIT_TEST( testIteratorsLoop );
  CPPUNIT_TEST( testPSrcGivenTrg );
  //CPPUNIT_TEST( testPTrgGivenSrc );  // Not implemented yet functionality for LevelDB
  CPPUNIT_TEST( testIteratorsOperatorsPlusPlusStar );
  CPPUNIT_TEST( testIteratorsOperatorsEqualNotEqual );
  CPPUNIT_TEST( testAddingSameSrcAndTrg );
  CPPUNIT_TEST( testSize );
  CPPUNIT_TEST( testLoadingLevelDb );
  CPPUNIT_TEST( testLoadedDataCorrectness );
  CPPUNIT_TEST( testSubkeys );
  CPPUNIT_TEST( test32bitRange );
  CPPUNIT_TEST( testByteMax );
  CPPUNIT_TEST( testByteMin );
  CPPUNIT_TEST_SUITE_END();

 private:
  string dbName = "/tmp/thot_leveldb_unit_test";
  LevelDbPhraseTable* tabLdb;  // Used for avoiding casting - should point on the same object like tab in parent class

 public:
  void setUp();
  void tearDown();

  void testKeyVectorConversion();
  void testAddSrcTrgInfo();
  void testIteratorsLoop();
  void testIteratorsOperatorsPlusPlusStar();
  void testIteratorsOperatorsEqualNotEqual();
  void testLoadingLevelDb();
  void testLoadedDataCorrectness();
};

#endif
