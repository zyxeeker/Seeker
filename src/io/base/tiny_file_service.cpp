#include "tiny_file_service.h"

#include <unistd.h>

#include <iostream>

namespace seeker {
namespace base {

TinyFileService::TinyFileService(std::string path)
    : path_(std::move(path)) {}

TinyFileService::~TinyFileService() = default;

TinyFileService::RESULT TinyFileService::ReadImpl(std::stringstream& ss) {
  auto res = FileCanBeRead();
  if (res) {
    return res;
  }

  std::ifstream ifs;
  ifs.open(path_);
  if (!ifs.good()) {
    return NO_EXIST;
  }

  ss << ifs.rdbuf();

  ifs.close();
  return OK;
}

TinyFileService::RESULT TinyFileService::WriteImpl(std::string str, bool append) {
  auto res = FileCanBeWrite();
  if (res == NO_WRITE) {
    return res;
  }

  auto flag = std::ios::out;
  flag |= append ? std::ios::app : std::ios::trunc;

  std::ofstream ofs;
  ofs.open(path_, flag);
  if (!ofs.good()) {
    return NO_EXIST;
  }
  ofs << str;
  ofs.close();

  return OK;
}

TinyFileService::RESULT TinyFileService::FileCanBeRead() {
  if (access(path_.c_str(), F_OK)) {
    return NO_EXIST;
  }
  if (access(path_.c_str(), R_OK)) {
    return NO_READ;
  }
  return OK;
}

TinyFileService::RESULT TinyFileService::FileCanBeWrite() {
  if (access(path_.c_str(), F_OK)) {
    return NO_EXIST;
  }
  if (access(path_.c_str(), W_OK)) {
    return NO_WRITE;
  }
  return OK;
}

} // namespace base
} // namespace seeker