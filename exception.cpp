#include "exception.hpp"

namespace NWbMqtt {

const char* TException::what() const noexcept {
    return Report.c_str();
}

TException::TException(const std::string &message, const char *fileName, const char *functionName, const int line) {
    char lineStr[20];
    sprintf(lineStr, "(%d)", line);
    this->Message = message;
    this->Report = "Exception!!!\n" + 
            "In file: " + fileName + lineStr + "\n" +
            "In function: " + functionName + lineStr + "\n" + 
            "With message: " + message + "\n";
}

    
};
