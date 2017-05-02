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
  CPPUNIT_TEST( testIncCountsOfEntry );
  CPPUNIT_TEST( testStoreAndRestore );
  CPPUNIT_TEST( testMock );
  CPPUNIT_TEST_SUITE_END();

 private:
  DaTriePhraseTable *tab;
  
  Vector<WordIndex> getVector(string phrase);

 public:
  void setUp();
  void tearDown();

  void testIncCountsOfEntry();
  void testStoreAndRestore();

  void testMock();
};

#endif
