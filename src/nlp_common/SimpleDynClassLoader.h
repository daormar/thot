// Author: Kyle Nusbaum
// 
// Code extracted and modified from: https://github.com/knusbaum/CPP-Dynamic-Class-Loading
//

#ifndef _SimpleDynClassLoader_H
#define _SimpleDynClassLoader_H

//--------------- Include files --------------------------------------

#if HAVE_CONFIG_H
#  include <thot_config.h>
#endif /* HAVE_CONFIG_H */

#include <memory>
#include <string>
#include <dlfcn.h>
#include <iostream>

//--------------- Classes --------------------------------------------

//---------------------------------
template <class T>
class SimpleDynClassLoader
{
 public:
  SimpleDynClassLoader();

      // Functions to open and close modules
  bool open_module(std::string module,int verbose=1);
  void close_module();

      // Function to create objects
  T* make_obj(std::string str);

  ~SimpleDynClassLoader();

 private:
  typename T::create_t* create;
  void* dll_handle;
};

//--------------- SimpleDynClassLoader class functions
//

//---------------------------------
template <class T>
SimpleDynClassLoader<T>::SimpleDynClassLoader()
{
  create=NULL;
  dll_handle=NULL;
}

//---------------------------------
template <class T>
void SimpleDynClassLoader<T>::close_module()
{
  if(dll_handle)
  {
    dlclose(dll_handle);
    dll_handle = NULL;
  }
  if(create) create = NULL;
}

//---------------------------------
template <class T>
bool SimpleDynClassLoader<T>::open_module(std::string module,int verbose/*=1*/)
{
      // Clear previous data
  close_module();

  if(verbose>=1)
    std::cerr<<"Opening module "<<module<<endl;
  
  dll_handle = dlopen(module.c_str(), RTLD_LAZY);

  if(!dll_handle)
  {
    std::cerr << "Failed to open library: " << dlerror() << std::endl;
    return false;
  }

      // Reset errors
  dlerror();

  create = (typename T::create_t*) dlsym(dll_handle, "create");
  const char * err = dlerror();
  if(err)
  {
    std::cerr << "Failed to load create symbol: " << err << std::endl;
    close_module();
    return false;
  }
    
  return true;
}

//---------------------------------
template <class T>
T* SimpleDynClassLoader<T>::make_obj(std::string str)
{
  if(!create)
  {
    return NULL;
  }
  return create(str);
}

//---------------------------------
template <class T>
SimpleDynClassLoader<T>::~SimpleDynClassLoader()
{
}

#endif
