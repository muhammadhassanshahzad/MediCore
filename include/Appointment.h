#pragma once
#include <iostream>
using namespace std;

class Appointment
{
private:
    int appointmentId;
    int patientId;
    int doctorId;
    char date[11];
    char timeSlot[6];
    char status[12];
public:
    Appointment();
    Appointment(int aId, int pId, int dId, const char* date, const char* slot, const char* status);
    int getAppointmentId() const;
    int getId() const 
    { 
        return appointmentId; 
    }
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;
    void setStatus(const char* status);
    bool operator==(const Appointment& other) const;
    friend ostream& operator<<(ostream& out, const Appointment& a);
    void display() const;
};