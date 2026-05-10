#include "../include/Doctor.h"
#include "../include/Validator.h"
#include <iostream>
using namespace std;

Doctor::Doctor() : Person()
{
    specialization[0] = '\0';
    fee = 0.0f;
}

Doctor::Doctor(int id, const char* name, const char* specialization, const char* contact, const char* password, float fee)
    : Person(id, name, contact, password)
{
    Validator::strCopy(this->specialization, specialization);
    this->fee = fee;
}

const char* Doctor::getSpecialization() const
{
    return specialization;
}

float Doctor::getFee() const
{
    return fee;
}

void Doctor::setSpecialization(const char* spec)
{
    Validator::strCopy(this->specialization, spec);
}

void Doctor::setFee(float fee)
{
    this->fee = fee;
}

bool Doctor::operator==(const Doctor& other) const
{
    return id == other.id;
}

ostream& operator<<(ostream& out, const Doctor& d)
{
    char feeBuf[32];
    Validator::floatToStr(d.fee, feeBuf);
    out << "ID: " << d.id << " | Name: " << d.name << " | Specialization: " << d.specialization
        << " | Contact: " << d.contact << " | Fee: PKR " << feeBuf;
    return out;
}

void Doctor::display() const
{
    cout << *this << endl;
}

const char* Doctor::getRole() const
{
    return "Doctor";
}