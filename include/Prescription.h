#pragma once
#include <iostream>
using namespace std;

class Prescription
{
private:
    int prescriptionId;
    int appointmentId;
    int patientId;
    int doctorId;
    char date[11];
    char medicines[500];
    char notes[300];
public:
    Prescription();
    Prescription(int prId, int aId, int pId, int dId, const char* date, const char* medicines, const char* notes);
    int getPrescriptionId() const;
    int getId() const 
    {
        return prescriptionId; 
    }
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;
};