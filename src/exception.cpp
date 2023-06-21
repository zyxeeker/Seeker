#include "exception.h"
#include <cstring>

namespace seeker {

Exception::Exception(std::string what_str)
    : what_str_(what_str) {}

const char* Exception::what() const throw () {
  return what_str_.c_str();
}

LogicError LogicError::Create(std::string module_name, 
                              std::string explanatory_str,
                              int32_t erro_num) {
  std::ostringstream oss;
  oss << "logicError(" << module_name << "): "
      << explanatory_str;
  if (erro_num)
    oss << ", errno: " << std::strerror(erro_num);
  return { oss.str() };
}

LogicError::LogicError(std::string str) 
    : Exception(str) {};

RunTimeError RunTimeError::Create(std::string module_name, 
                                  std::string explanatory_str,
                                  int32_t erro_num) {
  std::ostringstream oss;
  oss << "runtimeError(" << module_name << "): "
      << explanatory_str;
  if (erro_num)
    oss << ", errno: " << std::strerror(erro_num);
  return { oss.str() };
}

RunTimeError::RunTimeError(std::string str) 
    : Exception(str) {};

OtherError OtherError::Create(std::string module_name, 
                              std::string explanatory_str,
                              int32_t erro_num) {
  std::ostringstream oss;
  oss << "otherError(" << module_name << "): "
      << explanatory_str;
  if (erro_num)
    oss << ", errno: " << std::strerror(erro_num);
  return { oss.str() };
}

OtherError::OtherError(std::string str) 
    : Exception(str) {};

} // seeker