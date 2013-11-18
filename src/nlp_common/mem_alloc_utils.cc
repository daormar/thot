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
 

#include "mem_alloc_utils.h"

namespace mem_alloc_utils
{
  //--------------- my_calloc function
  void *my_calloc(size_t nmemb,size_t size)
  {
    void *ptr=calloc(nmemb,size);
    if(ptr==NULL)
    {
      fprintf(stderr,"Fatal error during memory allocation!\n");
      exit(1);
    }
    return ptr;
  }

  //--------------- my_realloc function
  void *my_realloc(void* ptr,size_t size)
  {
    void *newptr=realloc(ptr,size);
    if(newptr==NULL)
    {
      fprintf(stderr,"Fatal error during memory allocation!\n");
      exit(1);
    }
    return newptr;    
  }
}
