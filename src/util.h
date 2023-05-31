/**
 * @file util.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 工具辅助类
 * @version 1.0
 * @date 2023-05-30
 */

#ifndef _SEEKER_SRC_UTIL_H_
#define _SEEKER_SRC_UTIL_H_

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


#endif //_SEEKER_SRC_UTIL_H_