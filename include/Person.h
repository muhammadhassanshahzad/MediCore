#pragma once
#include <iostream>
using namespace std;

class Person
{
protected:
    int id;
    char name[51];
    char contact[12];
    char password[51];
public:
    Person();
    Person(int id, const char* name, const char* contact, const char* password);
    virtual ~Person() {}
    int getId() const;
    const char* getName() const;
    const char* getContact() const;
    const char* getPassword() const;
    void setId(int id);
    void setName(const char* name);
    void setContact(const char* contact);
    void setPassword(const char* password);
    virtual void display() const = 0;
    virtual const char* getRole() const = 0;
};