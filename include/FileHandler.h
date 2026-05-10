#pragma once
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
using namespace std;

class FileHandler
{
public:
    static void loadPatients(Storage<Patient>& storage);
    static void loadDoctors(Storage<Doctor>& storage);
    static void loadAdmin(Admin& admin);
    static void loadAppointments(Storage<Appointment>& storage);
    static void loadBills(Storage<Bill>& storage);
    static void loadPrescriptions(Storage<Prescription>& storage);

    static void appendPatient(const Patient& p);
    static void appendDoctor(const Doctor& d);
    static void appendAppointment(const Appointment& a);
    static void appendBill(const Bill& b);
    static void appendPrescription(const Prescription& pr);

    static void updatePatient(const Patient& p);
    static void updateDoctor(const Doctor& d);
    static void updateAppointment(const Appointment& a);
    static void updateBill(const Bill& b);

    static void deletePatient(int id);
    static void deleteDoctor(int id);
    static void deleteAppointmentsByPatient(int patientId);
    static void deleteBillsByPatient(int patientId);
    static void deletePrescriptionsByPatient(int patientId);

    static void archivePatient(int patientId, Storage<Patient>& patients, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions);

    static void logSecurityEvent(const char* role, const char* enteredId, const char* result);

    static void getTodayDate(char* buf);

private:
    static void parseLine(const char* line, char fields[][256], int& count);
    static void writePatientLine(char* buf, const Patient& p);
    static void writeDoctorLine(char* buf, const Doctor& d);
    static void writeAppointmentLine(char* buf, const Appointment& a);
    static void writeBillLine(char* buf, const Bill& b);
    static void writePrescriptionLine(char* buf, const Prescription& pr);
};