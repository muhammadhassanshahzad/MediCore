#include <iostream>
#include "include/GUI.h"
#include "include/Storage.h"
#include "include/Patient.h"
#include "include/Doctor.h"
#include "include/Admin.h"
#include "include/Appointment.h"
#include "include/Bill.h"
#include "include/Prescription.h"
#include "include/FileHandler.h"
#include "include/Validator.h"
#include "include/Exceptions.h"
#include "include/Menus.h"
using namespace std;

Storage<Patient> patients;
Storage<Doctor> doctors;
Storage<Appointment> appointments;
Storage<Bill> bills;
Storage<Prescription> prescriptions;
Admin admin;

void loadAllData()
{
    try { FileHandler::loadPatients(patients); }
    catch (FileNotFoundException& e) { cout << e.what() << endl; }
    try { FileHandler::loadDoctors(doctors); }
    catch (FileNotFoundException& e) { cout << e.what() << endl; }
    try { FileHandler::loadAdmin(admin); }
    catch (FileNotFoundException& e) { cout << e.what() << endl; }
    try { FileHandler::loadAppointments(appointments); }
    catch (FileNotFoundException& e) { cout << e.what() << endl; }
    try { FileHandler::loadBills(bills); }
    catch (FileNotFoundException& e) { cout << e.what() << endl; }
    try { FileHandler::loadPrescriptions(prescriptions); }
    catch (FileNotFoundException& e) { cout << e.what() << endl; }
}

bool loginPatient(Patient*& outPatient)
{
    int attempts = 0;
    while (attempts < 3)
    {
        cout << "Enter Patient ID: ";
        char idBuf[16];
        cin >> idBuf;
        cout << "Enter Password: ";
        char passBuf[51];
        cin >> passBuf;
        int id = Validator::strToInt(idBuf);
        Patient* p = patients.findById(id);
        if (p != nullptr && Validator::strComp(p->getPassword(), passBuf) == 0)
        {
            outPatient = p;
            return true;
        }
        attempts++;
        cout << "Invalid credentials. Attempts left: " << (3 - attempts) << endl;
        if (attempts == 3)
        {
            cout << "Account locked. Contact admin." << endl;
            FileHandler::logSecurityEvent("Patient", idBuf, "FAILED");
        }
    }
    return false;
}

bool loginDoctor(Doctor*& outDoctor)
{
    int attempts = 0;
    while (attempts < 3)
    {
        cout << "Enter Doctor ID: ";
        char idBuf[16];
        cin >> idBuf;
        cout << "Enter Password: ";
        char passBuf[51];
        cin >> passBuf;
        int id = Validator::strToInt(idBuf);
        Doctor* d = doctors.findById(id);
        if (d != nullptr && Validator::strComp(d->getPassword(), passBuf) == 0)
        {
            outDoctor = d;
            return true;
        }
        attempts++;
        cout << "Invalid credentials. Attempts left: " << (3 - attempts) << endl;
        if (attempts == 3)
        {
            cout << "Account locked. Contact admin." << endl;
            FileHandler::logSecurityEvent("Doctor", idBuf, "FAILED");
        }
    }
    return false;
}

bool loginAdmin()
{
    int attempts = 0;
    while (attempts < 3)
    {
        cout << "Enter Admin ID: ";
        char idBuf[16];
        cin >> idBuf;
        cout << "Enter Password: ";
        char passBuf[51];
        cin >> passBuf;
        int id = Validator::strToInt(idBuf);
        if (id == admin.getId() && Validator::strComp(admin.getPassword(), passBuf) == 0)
        {
            return true;
        }
        attempts++;
        cout << "Invalid credentials. Attempts left: " << (3 - attempts) << endl;
        if (attempts == 3)
        {
            cout << "Account locked. Contact admin." << endl;
            FileHandler::logSecurityEvent("Admin", idBuf, "FAILED");
        }
    }
    return false;
}

int main()
{
    loadAllData();
    GUI gui(patients, doctors, appointments, bills, prescriptions, admin);
    gui.run();
    return 0;
}