#include "exception.h"
#include <cstring>

namespace seeker {

Exception::Exception(std::string what_str, 
                     EXCEPTION_TYPE type)
    : type_(type) {
  switch (type_) {
    case EXCEPTION_LOGIC_ERROR:
      ex_ = std::logic_error(what_str);
      break;
    default:
      ex_ = std::runtime_error(what_str);
      break;
  }
}

const char* Exception::what() const throw () {
  return ex_.what();
}

LogicError LogicError::Create(std::string module_name, 
                              std::string explanatory_str,
                              int32_t erro_num) {
  std::ostringstream oss;
  oss << "(" << module_name << ")"
      <<  "logic error: " << explanatory_str;
  if (erro_num)
    oss << ", errno: " << std::strerror(erro_num);
  return { oss.str() };
}

LogicError::LogicError(std::string str) 
    : Exception(str, EXCEPTION_LOGIC_ERROR) {};

RunTimeError RunTimeError::Create(std::string module_name, 
                                  std::string explanatory_str,
                                  int32_t erro_num) {
  std::ostringstream oss;
  oss << "(" << module_name << ")"
      <<  "runtime error: " << explanatory_str;
  if (erro_num)
    oss << ", errno: " << std::strerror(erro_num);
  return { oss.str() };
}

RunTimeError::RunTimeError(std::string str) 
    : Exception(str, EXCEPTION_UNKNOWN_ERROR) {};

OtherError OtherError::Create(std::string module_name, 
                              std::string explanatory_str,
                              int32_t erro_num) {
  std::ostringstream oss;
  oss << "(" << module_name << ")"
      <<  "other error: " << explanatory_str;
  if (erro_num)
    oss << ", errno: " << std::strerror(erro_num);
  return { oss.str() };
}

OtherError::OtherError(std::string str) 
    : Exception(str, EXCEPTION_UNKNOWN_ERROR) {};

} // seeker