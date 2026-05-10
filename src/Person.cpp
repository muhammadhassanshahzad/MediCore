#include "../include/Person.h"
#include "../include/Validator.h"
using namespace std;

Person::Person()
{
    id = 0;
    name[0] = '\0';
    contact[0] = '\0';
    password[0] = '\0';
}

Person::Person(int id, const char* name, const char* contact, const char* password)
{
    this->id = id;
    Validator::strCopy(this->name, name);
    Validator::strCopy(this->contact, contact);
    Validator::strCopy(this->password, password);
}

int Person::getId() const
{
    return id;
}

const char* Person::getName() const
{
    return name;
}

const char* Person::getContact() const
{
    return contact;
}

const char* Person::getPassword() const
{
    return password;
}

void Person::setId(int id)
{
    this->id = id;
}

void Person::setName(const char* name)
{
    Validator::strCopy(this->name, name);
}

void Person::setContact(const char* contact)
{
    Validator::strCopy(this->contact, contact);
}

void Person::setPassword(const char* password)
{
    Validator::strCopy(this->password, password);
}