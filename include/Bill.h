#pragma once
#include <iostream>
using namespace std;

class Bill
{
private:
    int billId;
    int patientId;
    int appointmentId;
    float amount;
    char status[12];
    char date[11];
public:
    Bill();
    Bill(int bId, int pId, int aId, float amount, const char* status, const char* date);
    int getBillId() const;
    int getId() const 
    { 
        return billId;
    }
    int getPatientId() const;
    int getAppointmentId() const;
    float getAmount() const;
    const char* getStatus() const;
    const char* getDate() const;
    void setStatus(const char* status);
};