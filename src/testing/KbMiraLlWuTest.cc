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

}

//---------------------------------------
void KbMiraLlWuTest::tearDown()
{

}

//---------------------------------------
void KbMiraLlWuTest::testOnlineUpdate()
{
  std::string ref = "those documents are reunidas in the following file :";
  Vector<std::string> nbest;
  nbest.push_back("these documents are reunidas in the following file :");
  nbest.push_back("these sheets are reunidas in the following file :");
  nbest.push_back("those files are reunidas in the following file :");

  Vector<Vector<double> >nscores;
  Vector<double> x;
  x.push_back(0.1); x.push_back(0.4);
  nscores.push_back(x);
  x.clear();
  x.push_back(0.5); x.push_back(0.1);
  nscores.push_back(x);
  x.clear();
  x.push_back(0.1); x.push_back(0.4);
  nscores.push_back(x);

  Vector<double> wv(2, 1.);
  Vector<double> nwv;

  updater.update(ref, nbest, nscores, wv, nwv);

  // for(unsigned int k=0; k<wv.size(); k++) {
  //   cout << k << ": " << wv[k] << " -> " << nwv[k] << endl;
  // }
  CPPUNIT_ASSERT( wv[0] > nwv[0] );
  CPPUNIT_ASSERT( wv[1] < nwv[1] );
}
