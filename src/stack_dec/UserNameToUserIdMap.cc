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
 
/*********************************************************************/
/*                                                                   */
/* Module: UserNameToUserIdMap                                       */
/*                                                                   */
/* Definitions file: UserNameToUserIdMap.cc                          */
/*                                                                   */
/*********************************************************************/


//--------------- Include files ---------------------------------------

#include "UserNameToUserIdMap.h"

//--------------- Global variables ------------------------------------

//--------------- Function declarations 

//--------------- Constants

//--------------- Classes ---------------------------------------------

//-------------------------
UserNameToUserIdMap::UserNameToUserIdMap(void)
{
      // Initialize mutex
  pthread_mutex_init(&atomic_op_mut,NULL);
}

//-------------------------
unsigned int UserNameToUserIdMap::idForUserName(std::string username)
{
  int id;
  
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  std::map<std::string,int>::const_iterator mapcIter;

  mapcIter=strToIntMap.find(username);
  if(mapcIter!=strToIntMap.end())
  {
        // identifier exists
    id=mapcIter->second;
  }
  else
  {
        // a new identifier has to be created
    id=strToIntMap.size();
    strToIntMap[username]=id;
  }
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return id;
}

//-------------------------
std::pair<std::string,int> UserNameToUserIdMap::genNewUserNameIdPair(void)
{
  std::pair<std::string,int> strIntPair;
    
  pthread_mutex_lock(&atomic_op_mut);
  /////////// begin of mutex 

  // Find new (username, id) pair
  std::string str;
  int number=0;
  int new_id=0;
  std::map<std::string,int>::const_iterator mapcIter;
  bool end=false;
  while(!end)
  {
        // Convert integer to string
    std::ostringstream convert;  // stream used for the conversion
    convert << number;           // insert the textual representation of
                                 // 'number' in the characters in the stream
    str=convert.str();

        // Check if username already exists
    std::map<std::string,int>::const_iterator mapcIter;
    mapcIter=strToIntMap.find(str);
    if(mapcIter!=strToIntMap.end())
    {
          // username exists
      ++number;
    }
    else
    {
          // username does not exist
      new_id=strToIntMap.size();
      strToIntMap[str]=new_id;
      end=true;
    }
  }

      // Store result
  strIntPair.first=str;
  strIntPair.second=new_id;
  
  /////////// end of mutex 
  pthread_mutex_unlock(&atomic_op_mut);

  return strIntPair;
}

//-------------------------
UserNameToUserIdMap::~UserNameToUserIdMap()
{
      // Destroy mutexes and conditions
  pthread_mutex_destroy(&atomic_op_mut);
}

//-------------------------
