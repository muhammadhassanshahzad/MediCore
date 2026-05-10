#pragma once
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
using namespace std;

void patientMenu(Patient& patient, Storage<Doctor>& doctors, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions);
void doctorMenu(Doctor& doctor, Storage<Patient>& patients, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions);
void adminMenu(Admin& admin, Storage<Patient>& patients, Storage<Doctor>& doctors, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions);
int getMenuChoice(int min, int max);
void sortAppointmentsByDateAsc(Appointment* arr, int n);
void sortAppointmentsByDateDesc(Appointment* arr, int n);
void sortPrescriptionsByDateDesc(Prescription* arr, int n);
int compareDates(const char* d1, const char* d2);
int dateDiffFromToday(const char* date);