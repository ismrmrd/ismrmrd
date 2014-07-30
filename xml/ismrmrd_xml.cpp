#include "ismrmrd_xml.h"

#include <new> //For std::badalloc
#include <stdexcept> //For std::length_error
#include <stdio.h>
#include <string.h>
#include <iostream>


namespace ISMRMRD
{

  IsmrmrdXmlString::IsmrmrdXmlString()
    : string_(0)
    , strlen_(0)
  {

  }

  IsmrmrdXmlString::IsmrmrdXmlString(const char* s)
  {
    strlen_ = strlen(s);
    allocate(strlen_);
    copy_string(s,strlen_);
  }

  IsmrmrdXmlString::IsmrmrdXmlString(const char* s, size_t length)
  {
    strlen_ = length;
    allocate(strlen_);
    copy_string(s,strlen_);
  }

  IsmrmrdXmlString::~IsmrmrdXmlString() 
  {
    if (string_) delete [] string_;
  }

  IsmrmrdXmlString::IsmrmrdXmlString(const IsmrmrdXmlString& s) 
  {
    allocate(s.strlen_);
    strlen_ = s.strlen_;
    copy_string(s.string_,strlen_);
  }

  IsmrmrdXmlString& IsmrmrdXmlString::operator=(const IsmrmrdXmlString& s) 
  {
    allocate(s.strlen_);
    strlen_ = s.strlen_;
    copy_string(s.string_,strlen_);    
  }

  
  const char* IsmrmrdXmlString::c_str() {
    return string_;
  }

  void  IsmrmrdXmlString::set(const char* s)
  {
    size_t len = strlen(s);
    set(s,len);
  }
      
  void  IsmrmrdXmlString::set(const char* s, size_t len)
  {
    allocate(len);
    strlen_ = len;
    copy_string(s,len);
  }

  size_t IsmrmrdXmlString::size() 
  {
    return strlen_;
  }

  void IsmrmrdXmlString::allocate(size_t len) 
  {
    if (len != strlen_) {
      if (string_) delete [] string_;
      try {
	string_ = new char[len+1];
      } catch (std::bad_alloc&) {
	std::cout << "Error allocating memory in IsmrmrdXmlString" << std::endl;
	throw;
      }
    }
  }

  void IsmrmrdXmlString::copy_string(const char* s, size_t len)
  {
    if (len > strlen_) {
      throw std::length_error("Invalid length array specified for copy in IsmrmrdXmlString");
    }
    memcpy(string_,s,len);
    string_[len-1] = '\0';
  }


}
