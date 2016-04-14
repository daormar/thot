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
/* Module: KbMiraLlWuTest                                           */
/*                                                                  */
/* Prototypes file: KbMiraLlWuTest.h                                */
/*                                                                  */
/* Description: Declares the KbMiraLlWuTest class implementing      */
/*              unit tests for the KbMiraLlWu class.                */
/*                                                                  */
/********************************************************************/

/**
 * @file KbMiraLlWuTest.h
 *
 * @brief Declares the KbMiraLlWuTest class implementing unit tests
 * for the KbMiraLlWu class.
 */

#ifndef _KbMiraLlWuTest_h
#define _KbMiraLlWuTest_h

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include "KbMiraLlWu.h"
#include <cppunit/extensions/HelperMacros.h>

//--------------- Constants ------------------------------------------


//--------------- typedefs -------------------------------------------


//--------------- Classes --------------------------------------------

//--------------- KbMiraLlWuTest template class

/**
 * @brief Class implementing tests for KbMiraLlWu.
 */

class KbMiraLlWuTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( KbMiraLlWuTest );
  CPPUNIT_TEST( testOnlineUpdate );
  CPPUNIT_TEST( testFixedCorpusUpdate );
  CPPUNIT_TEST_SUITE_END();

 private:
  KbMiraLlWu *updater;

 public:
  void setUp();
  void tearDown();

  void testOnlineUpdate();
  void testFixedCorpusUpdate();
};

#endif
