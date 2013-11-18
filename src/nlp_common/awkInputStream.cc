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
/* Module: awkInputStream                                           */
/*                                                                  */
/* Definitions file: awkInputStream.cc                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "awkInputStream.h"


//--------------- awkInputStream class functions
//
// Functions returning bool values, returns non-zero 
// if succeeds

awkInputStream::awkInputStream(void)
{
 FS=0;
 buff=NULL;
 buftlen=0;
 fopen_called=false;
}

//----------
awkInputStream& awkInputStream::operator= (const awkInputStream &awk)
{
 FS=0;
 if(awk.FS!=0)
 {
  open((const char*)awk.fileName);
  FS=awk.FS;	
  while(FNR!=awk.FNR) getline();
 }
 return *this;
}

//----------
bool awkInputStream::getline(void)
{
  if(FS!=0)
  {
    ssize_t read;

    read=::getline(&buff,&buftlen,filePtr);
    if(read!=-1)
    {
      buff[read-1]='\0';
      ++FNR;   
      NF=get_NF();   
      return true;   
    }
    else return false;	
  }
  else return false; 
}

//----------
std::string awkInputStream::dollar(unsigned int n)
{
 if(FS!=0)
 {
  if(n==0)
  {
    return buff;
  }
  else 
  {
    if(n>NF) return "";
    else
    {
      retrieveField(n-1);
      return fieldStr;
    }
  }
 }
 else return NULL;
}

//----------
bool awkInputStream::open(const char *str)
{	
 if(fopen_called) close();
 filePtr=fopen(str,"r");
 if(filePtr==NULL)
 {
   FS=0;
   return ERROR;
 }
 else
 {
   fopen_called=true;
   FNR=0;
   FS=' ';
   return OK;
 }
}

//----------
bool awkInputStream::open_stream(FILE *stream)
{	
 if(fopen_called) close();
 filePtr=stream;
 if(filePtr==NULL)
 {
   FS=0;
   return ERROR;
 }
 else
 {
   FNR=0;
   FS=' ';
   return OK;
 }
}

//----------
void awkInputStream::close(void)
{
  if(fopen_called)
    fclose(filePtr);
  fieldStr="";
  FS=0;
  fopen_called=false;
}

//----------
bool awkInputStream::rewind(void)
{
 if(FS!=0)
 {
   FNR=0;
   ::rewind(filePtr);
   return OK;
 }
 else return ERROR; 
}

//----------
void awkInputStream::printFields(void)
{
 unsigned int i;
	
 if(FS!=0)
 {
   for(i=0;i<NF;++i) 	
   {
     retrieveField(i);
     printf("|%s",fieldStr.c_str());
   }
 }
 printf("|\n");	 
}

//----------
awkInputStream::~awkInputStream()
{
 if(buff!=NULL) free(buff);
 if(fopen_called) close();
}

//----------
int awkInputStream::get_NF(void)
{
 unsigned int i=0;

 NF=0;
	
 while(buff[i]!=0 && buff[i]==FS) ++i;	
 while(buff[i]!=0)
 {
   if(buff[i]==FS)
   {
     ++NF;
	 while(buff[i]!=0 && buff[i]==FS) ++i;	
   }
   else
   {
     ++i;
     if(buff[i]==0) ++NF;
   }
 }
 return NF;
}

//----------
void awkInputStream::retrieveField(unsigned int n)
{
 unsigned int numFields=0,i=0;

 fieldStr="";
 NF=0;
	
 while(buff[i]!=0 && buff[i]==FS) ++i;	
 while(buff[i]!=0)
 {
  if(buff[i]==FS)
  {
    ++NF;
    while(buff[i]!=0 && buff[i]==FS) ++i;	
  }
  else
  {
    ++i;
    if(buff[i]==0) ++NF;
  }
 }
 
 i=0;
 numFields=0; 
 while(buff[i]!=0 && buff[i]==FS) ++i;
 while(buff[i]!=0)
 {
   if(buff[i]==FS)
   {
	 ++numFields;
	 while(buff[i]!=0 && buff[i]==FS) ++i;
   }
  else
  {
    if(n==numFields) fieldStr+=buff[i];
    ++i;
    if(buff[i]==0 && n==numFields) {++numFields;}
  }
   if(numFields>n) break;	
 }	
}
