/*!
 * Class of exceptions 
 */
#pragma once

#include <exception>
#include <string>


namespace NWbMqtt {

class TException : public std::exception {
    std::string Message, Report;
  public:
    virtual const char* what() const noexcept override;
    TException(const std::string &message, const char *fileName, const char *functionName, const int line);
};
    
};

#define WBMQTT_EXCEPTION(what) TException(what, __FILE__, __FUNCTION__, __LINE__)


