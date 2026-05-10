#pragma once
#include <iostream>
using namespace std;

class HospitalException
{
protected:
    char message[200];
public:
    HospitalException(const char* msg)
    {
        int i = 0;
        while (msg[i] != '\0' && i < 199)
        {
            message[i] = msg[i];
            i++;
        }
        message[i] = '\0';
    }
    virtual const char* what() const
    {
        return message;
    }
    virtual ~HospitalException() {}
};

class FileNotFoundException : public HospitalException
{
public:
    FileNotFoundException(const char* msg) : HospitalException(msg) {}
};

class InsufficientFundsException : public HospitalException
{
public:
    InsufficientFundsException(const char* msg) : HospitalException(msg) {}
};

class InvalidInputException : public HospitalException
{
public:
    InvalidInputException(const char* msg) : HospitalException(msg) {}
};

class SlotUnavailableException : public HospitalException
{
public:
    SlotUnavailableException(const char* msg) : HospitalException(msg) {}
};