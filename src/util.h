/**
 * @file util.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 工具辅助类
 * @version 1.0
 * @date 2023-05-30
 */

#ifndef _SEEKER_SRC_UTIL_H_
#define _SEEKER_SRC_UTIL_H_

#include <time.h>
#include <unistd.h>

/**
 * @brief 线程ID
 */
#define TID pid_t
/**
 * @brief 进程ID
 */
#define PID pid_t

#define DO_NOT_ASSIGN_AND_COPY(C) \
public:\
C(C&&) = delete;  \
C(const C&) = delete; \
void operator=(const C&) = delete;

template<class T>
class Single {
 public:
  static T &GetInstance() {
    return _inst;
  }
 private:
  static T _inst;

  DO_NOT_ASSIGN_AND_COPY(Single)
};

template<class T>
T Single<T>::_inst;

template<class T>
class MSingle {
 public:
  static T &GetInstance() {
    static T _inst;
    return _inst;
  }
  DO_NOT_ASSIGN_AND_COPY(MSingle)
};

template<class T>
class LSingle {
 public:
  static T &GetInstance() {
    if (!_inst)
      _inst.reset(new T);
    return _inst;
  }
 private:
  static std::shared_ptr<T> _inst;

 DO_NOT_ASSIGN_AND_COPY(LSingle)
};

/**
 * @brief 获取时间戳
 */
static uint64_t GetTimeStamp() {
  return time(nullptr);
}

/**
 * @brief 时间戳格式化
 * @param format 格式字符串
 * @param time_stamp 时间戳
 * @return std::string 格式化后的时间戳
 */
static std::string TimeStampToString(const std::string &format, uint64_t time_stamp) {
	time_t t = (time_t)time_stamp;
  time(&t);
  struct tm *info;
  info = localtime(&t);
  char buff[80];
  strftime(buff, 80, format.c_str(), info);
  return std::string(buff);
}

/**
 * @brief 获取当前线程ID
 * @return TID 当前线程ID
 */
static TID GetThreadId() {
  return gettid();
}

#endif //_SEEKER_SRC_UTIL_H_