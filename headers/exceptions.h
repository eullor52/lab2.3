#pragma once

#include <string>
#include <iostream>
#include <cstdlib>

class Exception {
    private:
        std::string errorMessage;
    public:
        Exception(const char* message) {
            if (!message)
                errorMessage = "";
            else
                errorMessage = message;
        }
        Exception(const std::string& message) 
            : errorMessage(message) {}
        const char* what() const {
                return errorMessage.c_str();
        }
};

class OutOfRangeException : public Exception {
    public:
        OutOfRangeException(const char* message) : Exception(message) {}
        OutOfRangeException(const std::string& message) 
            : Exception(message) {}
};

class InvalidArgumentException : public Exception {
    public:
        InvalidArgumentException(const char* message) : Exception(message) {}
        InvalidArgumentException(const std::string& message)
            : Exception(message) {}
};

class LengthErrorException : public Exception {
    public:
        LengthErrorException(const char* message) : Exception(message) {}
        LengthErrorException(const std::string& message)
            : Exception(message) {}
};

class LogicErrorException : public Exception {
    public:
        LogicErrorException(const char* message) : Exception(message) {}
        LogicErrorException(const std::string& message)
            : Exception(message) {}
};

class DereferenceNullException : public Exception {
    public:
        DereferenceNullException(const char* message) : Exception(message) {}
        DereferenceNullException(const std::string& message)
            : Exception(message) {}
};