include "../include/Admin.h"
include <iostream>
using namespace std;

Admin::Admin() : Person() {}

Admin::Admin(int id, const char* name, const char* password)
    : Person(id, name, "", password) {}

void Admin::display() const
{
    cout << "Admin ID: " << id << " | Name: " << name << endl;
}

const char* Admin::getRole() const
{
    return "Admin";
}
