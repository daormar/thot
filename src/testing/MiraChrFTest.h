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
/* Module: MiraChrFTest                                             */
/*                                                                  */
/* Prototypes file: MiraChrFTest.h                                  */
/*                                                                  */
/* Description: Declares the MiraChrFTest class implementing        */
/*              unit tests for the MiraChfr class.                  */
/*                                                                  */
/********************************************************************/

/**
 * @file MiraChrFTest.h
 *
 * @brief Declares the MiraChrFTest class implementing unit tests
 * for the KbMiraLlWu class.
 */

#ifndef _MiraChrFTest_h
#define _MiraChrFTest_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <math.h>
#include "stack_dec/MiraChrF.h"
#include <cppunit/extensions/HelperMacros.h>

class MiraChrFTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( MiraChrFTest );
    CPPUNIT_TEST( testSentenceLevel );
    CPPUNIT_TEST( testCorpusLevel );
    CPPUNIT_TEST_SUITE_END();

    private:
        MiraChrF *chrf_metric;
        Vector<std::string> system_sentences;
        Vector<std::string> reference_sentences;

    public:
        void setUp();
        void tearDown();

        void testSentenceLevel();
        void testCorpusLevel();
};

#endif
