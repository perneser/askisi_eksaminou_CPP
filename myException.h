#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <string>

class MyException
{
    private:
        std::string errorMessage;
    public:

        // Constructor
        MyException(std::string msg);

        // Getter
        std:: string getErrorMessage();
};

#endif