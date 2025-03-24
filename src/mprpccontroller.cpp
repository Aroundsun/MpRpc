#include "mprpccontroller.h"

void MprpcController::Reset()
{
}

bool MprpcController::Failed() const
{
  return true;
}

std::string MprpcController::ErrorText() const
{
  return "";
}

void MprpcController::StartCancel()
{
}

void MprpcController::SetFailed(const std::string &reason)
{
}

bool MprpcController::IsCanceled() const
{
  return true;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{
}