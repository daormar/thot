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
/* Module: thot_calc_bleu                                           */
/*                                                                  */
/* Definitions file: thot_calc_bleu.cc                              */
/*                                                                  */
/* Description: Calculates the translation quality measure called   */
/*              "bleu"                                              */
/*                                                                  */
/********************************************************************/


//--------------- Include files --------------------------------------

#include "bleu.h"
#include "options.h"

//--------------- Constants ------------------------------------------


//--------------- Function Declarations ------------------------------

int TakeParameters(int argc,char *argv[]);
void printUsage(void);

//--------------- Type definitions -----------------------------------


//--------------- Global variables -----------------------------------

int verbosity;
char ref_file[1024];
char sys_file[1024];
bool sm_opt;

//--------------- Function Definitions -------------------------------

//--------------- main function
int main(int argc, char *argv[])
{
  std::vector<float> bleu_n;
  float bleu;
  float bp;
  int err;
  
  if(TakeParameters(argc,argv)) return 1;
  
  err=calc_bleu(ref_file,sys_file,bleu,bp,bleu_n,verbosity);

  if(!err)
  {
    if(!sm_opt)
    {
      std::cout<<"BLEU= "<< bleu <<" , BP= "<<bp<<" , ";
      for(unsigned int i=0;i<bleu_n.size();++i)
      {
        if(i!=bleu_n.size()-1) std::cout<</*i+1<<"-gram prec: "<<*/bleu_n[i]<<" / ";
        else std::cout<<bleu_n[i];
      }
      std::cout<<std::endl;
      return THOT_OK;
    }
    else
    {
      float smoothed_bleu=0;
      for(unsigned int i=1;i<=4;++i)
      {
        smoothed_bleu+=(bleu_n[i-1])/((float)pow((float)2,(float)4-i+1));
      }
      std::cout<<"Smoothed BLEU= "<< smoothed_bleu <<std::endl;
    }
  }
  else return err;
}

//--------------- TakeParameters function

int TakeParameters(int argc,char *argv[])
{
 int err;
  
     // Take reference file name
 err=readString(argc,argv, "-r", ref_file);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;   
 }

     // Take reference file name
 err=readString(argc,argv, "-t", sys_file);
 if(err==-1)
 {
   printUsage();
   return THOT_ERROR;   
 }

     // -v option
 err=readOption(argc,argv,"-v");
 verbosity=1;
 if(err==-1)
 {
   verbosity=0;
 }      

     // -sm option
 err=readOption(argc,argv,"-sm");
 sm_opt=1;
 if(err==-1)
 {
   sm_opt=0;
 }      

 return THOT_OK;
}

//--------------- printUsage() function

void printUsage(void)
{
  std::cerr << "thot_calc_bleu -r <string> -t <string> [-sm] [-v]"<<std::endl<<std::endl;
  std::cerr << "-r <string>       File containing the reference sentences"<<std::endl<<std::endl;
  std::cerr << "-t <string>       File containing the system translations"<<std::endl<<std::endl;
  std::cerr << "-sm               Calculate smoothed BLEU."<<std::endl<<std::endl;
  std::cerr << "-v                Verbose mode"<<std::endl;
}
