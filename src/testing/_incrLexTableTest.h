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
/* Module: _incrLexTableTest                                        */
/*                                                                  */
/* Prototypes file: _incrLexTableTest.h                             */
/*                                                                  */
/* Description: Declares the _incrLexTableTest abstract class       */
/*              implementing unit tests for the incremental         */
/*              lexical table classes.  */
/*                                                                  */
/********************************************************************/

/**
 * @file _incrLexTableTest.h
 *
 * @brief Declares the _incrLexTable abstract class implementing
 * unit tests for the incremental lexical table classes.
 */

#ifndef __incrLexTableTest_h
#define __incrLexTableTest_h

//--------------- Include files --------------------------------------

#include "ErrorDefs.h"
#include "MathDefs.h"

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "_incrLexTable.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------

//--------------- typedefs -------------------------------------------

//--------------- Classes --------------------------------------------

//--------------- IncrLexLevelDbTable template class

/**
 * @brief Class implementing tests for IncrLexLevelDbTable.
 */

class _incrLexTableTest: public CppUnit::TestFixture
{
    protected:
        _incrLexTable *tab;

    public:
        virtual void setUp() = 0;
        virtual void tearDown() = 0;

        void testGetSetLexDenom();
        void testGetSetLexNumer();
        void testGetTransForTarget();
        void testSetLexNumerDenom();

};

#endif