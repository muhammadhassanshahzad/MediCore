#include "../include/Patient.h"
#include "../include/Validator.h"
#include <iostream>
using namespace std;

Patient::Patient() : Person()
{
    age = 0;
    gender[0] = '\0';
    balance = 0.0f;
}

Patient::Patient(int id, const char* name, int age, const char* gender, const char* contact, const char* password, float balance)
    : Person(id, name, contact, password)
{
    this->age = age;
    Validator::strCopy(this->gender, gender);
    this->balance = balance;
}

int Patient::getAge() const
{
    return age;
}

const char* Patient::getGender() const
{
    return gender;
}

float Patient::getBalance() const
{
    return balance;
}

void Patient::setAge(int age)
{
    this->age = age;
}

void Patient::setGender(const char* gender)
{
    Validator::strCopy(this->gender, gender);
}

void Patient::setBalance(float balance)
{
    this->balance = balance;
}

Patient& Patient::operator+=(float amount)
{
    balance += amount;
    return *this;
}

Patient& Patient::operator-=(float amount)
{
    balance -= amount;
    return *this;
}

bool Patient::operator==(const Patient& other) const
{
    return id == other.id;
}

ostream& operator<<(ostream& out, const Patient& p)
{
    char balBuf[32];
    Validator::floatToStr(p.balance, balBuf);
    out << "ID: " << p.id << " | Name: " << p.name << " | Age: " << p.age
        << " | Gender: " << p.gender << " | Contact: " << p.contact
        << " | Balance: PKR " << balBuf;
    return out;
}

void Patient::display() const
{
    cout << *this << endl;
}

const char* Patient::getRole() const
{
    return "Patient";
}