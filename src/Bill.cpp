#include "../include/Bill.h"
#include "../include/Validator.h"
using namespace std;

Bill::Bill()
{
    billId = 0;
    patientId = 0;
    appointmentId = 0;
    amount = 0.0f;
    status[0] = '\0';
    date[0] = '\0';
}

Bill::Bill(int bId, int pId, int aId, float amount, const char* status, const char* date)
{
    billId = bId;
    patientId = pId;
    appointmentId = aId;
    this->amount = amount;
    Validator::strCopy(this->status, status);
    Validator::strCopy(this->date, date);
}

int Bill::getBillId() const { return billId; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
float Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status; }
const char* Bill::getDate() const { return date; }

void Bill::setStatus(const char* status)
{
    Validator::strCopy(this->status, status);
}