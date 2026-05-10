#pragma once
#include "Person.h"
using namespace std;

class Patient : public Person
{
private:
    int age;
    char gender[2];
    float balance;
public:
    Patient();
    Patient(int id, const char* name, int age, const char* gender, const char* contact, const char* password, float balance);
    int getAge() const;
    const char* getGender() const;
    float getBalance() const;
    void setAge(int age);
    void setGender(const char* gender);
    void setBalance(float balance);
    Patient& operator+=(float amount);
    Patient& operator-=(float amount);
    bool operator==(const Patient& other) const;
    friend ostream& operator<<(ostream& out, const Patient& p);
    void display() const override;
    const char* getRole() const override;
};