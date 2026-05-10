#include "../include/Prescription.h"
#include "../include/Validator.h"
using namespace std;

Prescription::Prescription()
{
    prescriptionId = 0;
    appointmentId = 0;
    patientId = 0;
    doctorId = 0;
    date[0] = '\0';
    medicines[0] = '\0';
    notes[0] = '\0';
}

Prescription::Prescription(int prId, int aId, int pId, int dId, const char* date, const char* medicines, const char* notes)
{
    prescriptionId = prId;
    appointmentId = aId;
    patientId = pId;
    doctorId = dId;
    Validator::strCopy(this->date, date);
    int i = 0;
    while (medicines[i] != '\0' && i < 499)
    {
        this->medicines[i] = medicines[i];
        i++;
    }
    this->medicines[i] = '\0';
    i = 0;
    while (notes[i] != '\0' && i < 299)
    {
        this->notes[i] = notes[i];
        i++;
    }
    this->notes[i] = '\0';
}

int Prescription::getPrescriptionId() const { return prescriptionId; }
int Prescription::getAppointmentId() const { return appointmentId; }
int Prescription::getPatientId() const { return patientId; }
int Prescription::getDoctorId() const { return doctorId; }
const char* Prescription::getDate() const { return date; }
const char* Prescription::getMedicines() const { return medicines; }
const char* Prescription::getNotes() const { return notes; }