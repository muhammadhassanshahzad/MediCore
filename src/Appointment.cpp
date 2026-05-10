#include "../include/Appointment.h"
#include "../include/Validator.h"
#include <iostream>
using namespace std;

Appointment::Appointment()
{
    appointmentId = 0;
    patientId = 0;
    doctorId = 0;
    date[0] = '\0';
    timeSlot[0] = '\0';
    status[0] = '\0';
}

Appointment::Appointment(int aId, int pId, int dId, const char* date, const char* slot, const char* status)
{
    appointmentId = aId;
    patientId = pId;
    doctorId = dId;
    Validator::strCopy(this->date, date);
    Validator::strCopy(this->timeSlot, slot);
    Validator::strCopy(this->status, status);
}

int Appointment::getAppointmentId() const { return appointmentId; }
int Appointment::getPatientId() const { return patientId; }
int Appointment::getDoctorId() const { return doctorId; }
const char* Appointment::getDate() const { return date; }
const char* Appointment::getTimeSlot() const { return timeSlot; }
const char* Appointment::getStatus() const { return status; }

void Appointment::setStatus(const char* status)
{
    Validator::strCopy(this->status, status);
}

bool Appointment::operator==(const Appointment& other) const
{
    if (doctorId != other.doctorId)
    {
        return false;
    }
    if (Validator::strComp(date, other.date) != 0)
    {
        return false;
    }
    if (Validator::strComp(timeSlot, other.timeSlot) != 0)
    {
        return false;
    }
    if (Validator::strComp(status, "cancelled") == 0 || Validator::strComp(other.status, "cancelled") == 0)
    {
        return false;
    }
    return true;
}

ostream& operator<<(ostream& out, const Appointment& a)
{
    out << "ID: " << a.appointmentId << " | PatientID: " << a.patientId
        << " | DoctorID: " << a.doctorId << " | Date: " << a.date
        << " | Time: " << a.timeSlot << " | Status: " << a.status;
    return out;
}

void Appointment::display() const
{
    cout << *this << endl;
}