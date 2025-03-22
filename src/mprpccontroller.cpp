#include"mprpccontroller.h"



void Reset();

  bool Failed() const;

  std::string ErrorText() const;


  void StartCancel();

  void SetFailed(const std::string& reason);

 
  bool IsCanceled() const;


  void NotifyOnCancel(google::protobuf::Closure* callback);