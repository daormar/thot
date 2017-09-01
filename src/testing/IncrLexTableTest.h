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
/* Module: IncrLexTableTest                                         */
/*                                                                  */
/* Prototypes file: IncrLexTableTest.h                              */
/*                                                                  */
/* Description: Declares the IncrLexTableTest class implementing    */
/*              unit tests for the IncrLexTable class.              */
/*                                                                  */
/********************************************************************/

/**
 * @file IncrLexTableTest.h
 *
 * @brief Declares the IncrLexTableTest class implementing unit tests
 * for the IncrLexTable class.
 */

#ifndef _IncrLexTableTest_h
#define _IncrLexTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrLexTableTest.h"
#include "IncrLexTable.h"

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrLexLevelDbTable template class

/**
 * @brief Class implementing tests for IncrLexLevelDbTable.
 */

class IncrLexTableTest: public _incrLexTableTest
{
    CPPUNIT_TEST_SUITE( IncrLexTableTest );
    CPPUNIT_TEST( testGetSetLexDenom );
    CPPUNIT_TEST( testGetSetLexNumer );
    CPPUNIT_TEST( testGetTransForTarget );
    CPPUNIT_TEST( testSetLexNumerDenom );
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp();
        void tearDown();

};

#endif