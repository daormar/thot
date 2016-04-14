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
/* Definitions file: KbMiraLlWuTest.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "KbMiraLlWuTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( KbMiraLlWuTest );

//--------------- KbMiraLlWuTest class functions
//

//---------------------------------------
void KbMiraLlWuTest::setUp()
{
  updater = new KbMiraLlWu();
}

//---------------------------------------
void KbMiraLlWuTest::tearDown()
{
  delete updater;
}

//---------------------------------------
void KbMiraLlWuTest::testFeature()
{
  //     // Set up
  // const std::string currencyFF( "FF" );
  // const double longNumber = 12345678.90123;

  //     // Process
  // Money money( longNumber, currencyFF );

  //     // Check
  // CPPUNIT_ASSERT_EQUAL( longNumber, money.getAmount() );
  // CPPUNIT_ASSERT_EQUAL( currencyFF, money.getCurrency() );
}

//---------------------------------------
void KbMiraLlWuTest::testOnlineUpdate()
{
  std::string ref = "those documents are reunidas in the following file :";
  Vector<std::string> nbest;
  nbest.push_back("these documents are reunidas in the following file :");
  nbest.push_back("these sheets are reunidas in the following file :");
  nbest.push_back("those files are reunidas in the following file :");

  Vector<Vector<Score> >nscores;
  Vector<Score> x;
  x.push_back(0.1); x.push_back(0.5);
  nscores.push_back(x); // LM0= -48.661 WordPenalty0= -11 PhrasePenalty0= 20 TranslationModel0= -24.4921 -23.0584 -9.11355 -10.1318 TranslationModel1= 8.99907
  x.clear();
  x.push_back(0.5); x.push_back(0.1);
  nscores.push_back(x); // LM0= -59.655 WordPenalty0= -11 PhrasePenalty0= 20 TranslationModel0= -15.7355 -16.9218 -4.97704 -7.49273 TranslationModel1= 8.99907
  x.clear();
  x.push_back(0.1); x.push_back(0.4);
  nscores.push_back(x); // LM0= -55.5461 WordPenalty0= -11 PhrasePenalty0= 20 TranslationModel0= -22.2297 -17.6222 -9.47195 -6.98359 TranslationModel1= 8.99907

  Vector<Score> wv(2, 1.);
  Vector<Score> nwv;

  updater.update(ref, nbest, nscores, wv, nwv);
  updater.update(ref, nbest, nscores, nwv, wv);
  updater.update(ref, nbest, nscores, wv, nwv);

  for(unsigned int k=0; k<wv; k++) {
    cout << k << ": " << wv[k] << " -> " << nwv[k] << endl;
  }
}
