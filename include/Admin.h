#pragma once
#include "Person.h"
using namespace std;

class Admin : public Person
{
public:
    Admin();
    Admin(int id, const char* name, const char* password);
    void display() const override;
    const char* getRole() const override;
};
