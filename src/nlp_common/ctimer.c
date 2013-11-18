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
 

#include "ctimer.h"

static double firstcall;

/**
 *
 * @brief Function for time measuring
 *
 * @param elapsed elapsed time since the first call to ctimer (miliseconds.)
 * @param ucpu    cpu elapsed time since the first call to ctimer (miliseconds.)
 * @param scpu    system elapsed time since the first call to ctimer (miliseconds.)
 * @return 0 if OK. 
 * 
 * The function must be executed at least two times.
 * The first one starts the timer and sets elapsed to zero.
 * The second execution of ctimer returns the elapsed time between
 * the first and the second execution.
 * The following calls to ctimer return the elapsed time between the first
 * call and the current call.
 *
 */
void ctimer(double *elapsed, double *ucpu, double *scpu)
{
#ifdef THOT_MINGW
  fprintf(stderr,"Warning: ctimer function not implemented for mingw host!");
#else
  struct timeval tm;
  struct timezone tz;
  struct tms sistema;
  double usegs;

  gettimeofday(&tm, &tz);
  times(&sistema);

  usegs = tm.tv_usec+tm.tv_sec*1E6;

  if (firstcall)
  {
    *elapsed = usegs - firstcall;
  }
  else
  {
    *elapsed = 0;
    firstcall = usegs;
  }

  *elapsed = *elapsed/1E6;
  *ucpu = (double)sistema.tms_utime/(double)CLOCKS_PER_SEC*1E4;
  *scpu = (double)sistema.tms_stime/(double)CLOCKS_PER_SEC*1E4;
#endif
}
