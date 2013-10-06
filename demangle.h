#ifndef _DEMANGLE_H_
#define _DEMANGLE_H_ 1

#include <string>
#include <iostream>
#include <cxxabi.h>


#define quote(x) #x

template<typename ATYPE>
std::string show_name(const ATYPE& obj)
{
  int status;
  char * demangled = abi::__cxa_demangle(typeid(obj).name(),0,0,&status);
  std::string ret(demangled);
  free(demangled);
  return ret;
}

#endif 
