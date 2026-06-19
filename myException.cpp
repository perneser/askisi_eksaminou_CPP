#include "myException.h"
#include <string>

// Constructor

MyException::MyException(std::string msg): errorMessage(msg)
{
}

std::string MyException::getErrorMessage()
{
    return errorMessage;
}
