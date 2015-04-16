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
 
#include "options.h"

/**
 * @brief Verifies if a list of parameters was introduced in a given command-line.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param posPars a map containing a list of labels associated to the parameters that must be verified in the command-line.
 * @return 1 if the parameters are correct, 0 in other case.
 */
bool verifyParams(int argc, char **argv, map<string,bool> posPars)
{
  int i;
  std::map<string,bool>::iterator parMapPos;
	
  i = 1;
  while (i < argc)
  {
    if (argv[i][0]=='-')
    {
      parMapPos=posPars.find(argv[i]);
      if(parMapPos==posPars.end()) return false;
    }
    i++;
  }
  return true;		
}

/**
 * @brief Verifies if a parameter was introduced in the command line.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @return 0 if the label was found, -1 in other case.
 */
bool existParam(int argc, char **argv, const char *pszLabel)
{
  int i;
  
  i = 1;
  while (i < argc)
  {    
    if (!strcmp(argv[i], pszLabel)) {
      return true;
    }
    i++;
  }
  return false;	
}

/**
 * @brief Returns true if a certain option was introduced in the command line.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @return 0 if the label was found, -1 in other case.
 */
int readOption(int argc, char **argv, const char *pszLabel)
{
  int i;

  i = 1;
  while (i < argc)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Returns true if str is an option.
 *
 * @param str character string.
 * @return true if str is an option, false otherwise.
 */
int isOption(const char* str)
{
  if(strncmp("-",str,1)==0 && strlen(str)>1)
  {
    char c=str[1];
    if(c!='0' && c!='1' && c!='2' && c!='3' && c!='4' && c!='5' && c!='6' && c!='7' && c!='8' && c!='9')
      return true;
    else
      return false;
  }
  else return false;
}

/**
 * @brief Reads the boolean value associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readBool(int argc, char **argv, const char *pszLabel, bool *Value)
{
  int i,j;

  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      sscanf(argv[i+1], "%d", &j);
      if(j==0){*Value=0;}
      else{*Value=1;}
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads the integer associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readInt(int argc, char **argv, const char *pszLabel, int *Value)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      sscanf(argv[i+1], "%d", Value);
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads the unsigned integer associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readUnsignedInt(int argc,
                    char **argv,
                    const char *pszLabel,
                    unsigned int *Value)
{
 int i;

 i = 1;
 while (i < argc-1)
 {
   if (!strcmp(argv[i], pszLabel))
   {
     sscanf(argv[i+1], "%d", Value);
     return 0;
   }
   i++;
 }
 return -1;	
}

/**
 * @brief Reads the floating point number associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readFloat(int argc, char **argv, const char *pszLabel, float *Value)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      sscanf(argv[i+1], "%f", Value);
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads a sequence of floating point numbers associated to a given label
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option.
 * @param Float vector given as reference which will be used to store
 * the sequence of floating point numbers.
 * @return 0 if the label was found, -1 in other case.
 */
int readFloatSeq(int argc,
                 char **argv,
                 const char *pszLabel,
                 Vector<float>& floatVec)
{
  int i;
  float value;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      Vector<float> floatVecAux;
      bool end=false;
      while(!end)
      {
        ++i;
        sscanf(argv[i], "%f", &value);
        floatVecAux.push_back(value);
        if(i==argc-1) end=true;
        else
        {
          if(isOption(argv[i+1]))
          {
            end=true;
          }
        }
      }
      floatVec=floatVecAux;
      return 0;
    }
    i++;
  }
  return -1;   
}

/**
 * @brief Reads the double associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readDouble(int argc, char **argv, const char *pszLabel, double *Value)
{
  int i;

  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      sscanf(argv[i+1], "%lf", Value);
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads the string associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readString(int argc, char **argv, const char *pszLabel, char *Value)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      strcpy(Value,argv[i+1]);
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads the string associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param strPtr Pointer to the string identified by the label. Size of the string pointed by strPtr is modified if necessary.
 * @return 0 if the label was found, -1 in other case.
 */

int readStringSafe(int argc, char **argv, const char *pszLabel, char **strPtr)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    *strPtr=(char*) mem_alloc_utils::my_realloc(strPtr,strlen(argv[i+1])*sizeof(char));
    if (!strcmp(argv[i], pszLabel))
    {
      strcpy(*strPtr,argv[i+1]);
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads two consecutive strings associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param val1 pointer to the first string.
 * @param val2 pointer to the second string.
 * @return 0 if the label was found, -1 in other case.
 */
int readTwoStrings(int argc,
                   char **argv,
                   const char *pszLabel,
                   char *val1,
                   char *val2)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      if(i+2>=argc)
      {
        return -1;
      }
      else
      {
        strcpy(val1,argv[i+1]);
        strcpy(val2,argv[i+2]);
        return 0;
      }
    }
    i++;
  }
  return -1;  
}

/**
 * @brief Reads two consecutive strings associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param strPtr1 pointer to the first string. Size modified if necessary.
 * @param strPtr2 pointer to the second string. Size modified if necessary.
 * @return 0 if the label was found, -1 in other case.
 */
int readTwoStringsSafe(int argc,
                       char **argv,
                       const char *pszLabel,
                       char** strPtr1,
                       char** strPtr2)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      if(i+2>=argc)
      {
        return -1;
      }
      else
      {
        *strPtr1=(char*) mem_alloc_utils::my_realloc(strPtr1,strlen(argv[i+1])*sizeof(char));
        strcpy(*strPtr1,argv[i+1]);
        *strPtr2=(char*) mem_alloc_utils::my_realloc(strPtr2,strlen(argv[i+2])*sizeof(char));
        strcpy(*strPtr2,argv[i+2]);
        return 0;
      }
    }
    i++;
  }
  return -1;  
}

/**
 * @brief Reads the stl string associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param Value pointer to the value identified by the label.
 * @return 0 if the label was found, -1 in other case.
 */
int readSTLstring(int argc, char **argv, const char *pszLabel, string *Value)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      *Value=argv[i+1]; 
      return 0;
    }
    i++;
  }
  return -1;
}

/**
 * @brief Reads two consecutive strings associated to a given label.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option
 * @param val1 pointer to the first stl string.
 * @param val2 pointer to the second stl string.
 * @return 0 if the label was found, -1 in other case.
 */
int readTwoSTLstrings(int argc,
                      char **argv,
                      const char *pszLabel,
                      string *val1,
                      string *val2)
{
  int i;
  
  i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      if(i+2>=argc)
      {
        return -1;
      }
      else
      {
        *val1=argv[i+1];
        *val2=argv[i+2];
        return 0;
      }
    }
    i++;
  }
  return -1;    
}

/**
 * @brief Converts const char **argv into Vector<std::string>.
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @return A Vector of strings equivalent to argv.
 */
Vector<std::string> argv2argv_stl(int argc, char **argv)
{
  Vector<std::string> result;

  for(int i=0;i<argc;++i)
    result.push_back(argv[i]);
  return result;
}


/**
 * @brief Reads a sequence of floating point numbers associated to a given label
 *
 * @param argv pointer array containing the options introduced in the command-line.
 * @param argc number of arguments introduced in the command-line.
 * @param pszLabel string containg the label associated to an option.
 * @param String vector given as reference which will be used to store
 * the sequence of floating point numbers.
 * @return 0 if the label was found, -1 in other case.
 */
int readStringSeq(int argc,
                 char **argv,
                 const char *pszLabel,
                 Vector<string>& strVec)
{
  int i = 1;
  while (i < argc-1)
  {
    if (!strcmp(argv[i], pszLabel))
    {
      Vector<string> strVecAux;
      bool end=false;
      while(!end)
      {
        ++i;
        strVecAux.push_back(argv[i]);
        if(i==argc-1) end=true;
        else
        {
          if(isOption(argv[i+1]))
          {
            end=true;
          }
        }
      }
      strVec=strVecAux;
      return 0;
    }
    i++;
  }
  return -1;   
}

/**
 * @brief Given a file name, extracts the parameters contained in it,
 * ignoring those lines starting with the string "comment".
 *
 * @param filename File containing the parameters to be extracted.
 * @param argc number of arguments extracted from filename+1 (argv_stl[0]="filename").
 * @param argv arguments extracted from filename. argv_stl[0] contains the string "filename"
 * @param comment Starting string for comments.
 * @return ERROR if filename does not exist, OK otherwise.
 */
bool extractParsFromFile(const char* filename,
                         int& argc,
                         Vector<std::string>& argv_stl,
                         std::string comment)
{
  awkInputStream awk;
  
  argc=1;
  argv_stl.clear();
  argv_stl.push_back(filename);
  
  if(awk.open(filename)==ERROR)
  {
    cerr<<"Error: file "<<filename<<" does not exist.\n";
    return ERROR;
  }
  else
  {
    while(awk.getline())
    {
      if(awk.NF>=1)
      {
            // Decide whether the line is a comment or not
        std::string firstCol=awk.dollar(1);
        bool isComment=false;
        if(firstCol==comment) isComment=true;
        else
        {
              // Check if comment is a prefix of firstCol
          if(firstCol.size()>=comment.size())
          {
            isComment=true;
            for(unsigned int i=0;i<comment.size();++i)
            {
              if(firstCol[i]!=comment[i])
              {
                isComment=false;
                break;
              }
            }
          }
        }

            // Process line if it is not a comment
        if(!isComment)
        {
          for(unsigned int i=1;i<=awk.NF;++i)
          {
            ++argc;
            argv_stl.push_back(awk.dollar(i));
          }
        }
      }
    }
    return OK;
  }
}
