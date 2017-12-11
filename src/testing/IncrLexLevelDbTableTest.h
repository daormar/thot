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
/* Module: IncrLexLevelDbTableTest                                  */
/*                                                                  */
/* Prototypes file: IncrLexLevelDbTableTest.h                       */
/*                                                                  */
/* Description: Declares the IncrLexLevelDbTableTest class          */
/*              implementing unit tests for the IncrLexLevelDbTable */
/*              class.                                              */
/*                                                                  */
/********************************************************************/

/**
 * @file IncrLexLevelDbTableTest.h
 *
 * @brief Declares the IncrLexLevelDbTable class implementing unit tests
 * for the IncrLexLevelDbTable class.
 */

#ifndef _IncrLexLevelDbTableTest_h
#define _IncrLexLevelDbTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrLexTableTest.h"
#include "IncrLexLevelDbTable.h"

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- IncrLexLevelDbTable template class

/**
 * @brief Class implementing tests for IncrLexLevelDbTable.
 */

class IncrLexLevelDbTableTest: public _incrLexTableTest
{
  CPPUNIT_TEST_SUITE( IncrLexLevelDbTableTest );
  CPPUNIT_TEST( testGetSetLexDenom );
  CPPUNIT_TEST( testGetSetLexNumer );
  CPPUNIT_TEST( testGetTransForTarget );
  CPPUNIT_TEST( testSetLexNumerDenom );
  CPPUNIT_TEST( testLoadFromBinFile );
  CPPUNIT_TEST( testLoadFromLevelDb );
  CPPUNIT_TEST_SUITE_END();

 private:
  std::string dbNamePrefix = "/tmp/thot_leveldb_unit_test";

 public:
  void setUp();
  void tearDown();

  void testLoadFromBinFile();
  void testLoadFromLevelDb();

};

#endif