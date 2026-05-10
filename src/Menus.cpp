#include "../include/Menus.h"
#include "../include/FileHandler.h"
#include "../include/Validator.h"
#include "../include/Exceptions.h"
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;

int getMenuChoice(int min, int max)
{
    char buf[16];
    cin >> buf;
    if (!Validator::isPositiveInt(buf) && !(buf[0] == '0'))
    {
        return -1;
    }
    int choice = Validator::strToInt(buf);
    if (!Validator::isValidMenuChoice(choice, min, max))
    {
        return -1;
    }
    return choice;
}

int compareDates(const char* d1, const char* d2)
{
    int day1 = (d1[0] - '0') * 10 + (d1[1] - '0');
    int mon1 = (d1[3] - '0') * 10 + (d1[4] - '0');
    int yr1  = (d1[6] - '0') * 1000 + (d1[7] - '0') * 100 + (d1[8] - '0') * 10 + (d1[9] - '0');
    int day2 = (d2[0] - '0') * 10 + (d2[1] - '0');
    int mon2 = (d2[3] - '0') * 10 + (d2[4] - '0');
    int yr2  = (d2[6] - '0') * 1000 + (d2[7] - '0') * 100 + (d2[8] - '0') * 10 + (d2[9] - '0');
    if (yr1 != yr2)
    {
        return yr1 - yr2;
    }
    if (mon1 != mon2)
    {
        return mon1 - mon2;
    }
    return day1 - day2;
}

int dateDiffFromToday(const char* date)
{
    char today[11];
    FileHandler::getTodayDate(today);
    int day1 = (today[0] - '0') * 10 + (today[1] - '0');
    int mon1 = (today[3] - '0') * 10 + (today[4] - '0');
    int yr1  = (today[6] - '0') * 1000 + (today[7] - '0') * 100 + (today[8] - '0') * 10 + (today[9] - '0');
    int day2 = (date[0] - '0') * 10 + (date[1] - '0');
    int mon2 = (date[3] - '0') * 10 + (date[4] - '0');
    int yr2  = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
    struct tm t1 = {};
    t1.tm_mday = day1;
    t1.tm_mon  = mon1 - 1;
    t1.tm_year = yr1 - 1900;
    struct tm t2 = {};
    t2.tm_mday = day2;
    t2.tm_mon  = mon2 - 1;
    t2.tm_year = yr2 - 1900;
    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);
    double diff = difftime(time1, time2);
    return (int)(diff / 86400.0);
}

void sortAppointmentsByDateAsc(Appointment* arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            int cmp = compareDates(arr[j].getDate(), arr[j + 1].getDate());
            if (cmp > 0)
            {
                Appointment tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
            else if (cmp == 0)
            {
                if (Validator::strComp(arr[j].getTimeSlot(), arr[j + 1].getTimeSlot()) > 0)
                {
                    Appointment tmp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = tmp;
                }
            }
        }
    }
}

void sortAppointmentsByDateDesc(Appointment* arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (compareDates(arr[j].getDate(), arr[j + 1].getDate()) < 0)
            {
                Appointment tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

void sortPrescriptionsByDateDesc(Prescription* arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (compareDates(arr[j].getDate(), arr[j + 1].getDate()) < 0)
            {
                Prescription tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

void patientMenu(Patient& patient, Storage<Doctor>& doctors, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions)
{
    int choice = 0;
    do
    {
        char balBuf[32];
        Validator::floatToStr(patient.getBalance(), balBuf);
        cout << endl;
        cout << "Welcome, " << patient.getName() << endl;
        cout << "Balance: PKR " << balBuf << endl;
        cout << "========================" << endl;
        cout << "1. Book Appointment" << endl;
        cout << "2. Cancel Appointment" << endl;
        cout << "3. View My Appointments" << endl;
        cout << "4. View My Medical Records" << endl;
        cout << "5. View My Bills" << endl;
        cout << "6. Pay Bill" << endl;
        cout << "7. Top Up Balance" << endl;
        cout << "8. Logout" << endl;
        cout << "Enter choice: ";
        choice = getMenuChoice(1, 8);
        if (choice == -1)
        {
            cout << "Invalid choice." << endl;
            continue;
        }
        if (choice == 1)
        {
            char spec[51];
            cout << "Enter specialization to search: ";
            cin.ignore();
            cin.getline(spec, 51);
            char specLower[51];
            Validator::toLowerStr(specLower, spec);
            bool found = false;
            for (int i = 0; i < doctors.size(); i++)
            {
                char dSpecLower[51];
                Validator::toLowerStr(dSpecLower, doctors.getAll()[i].getSpecialization());
                if (Validator::strComp(specLower, dSpecLower) == 0)
                {
                    char feeBuf[32];
                    Validator::floatToStr(doctors.getAll()[i].getFee(), feeBuf);
                    cout << "ID: " << doctors.getAll()[i].getId()
                         << " | Name: " << doctors.getAll()[i].getName()
                         << " | Fee: PKR " << feeBuf << endl;
                    found = true;
                }
            }
            if (!found)
            {
                cout << "No doctors available for that specialization." << endl;
                continue;
            }
            char docIdBuf[16];
            cout << "Enter Doctor ID: ";
            cin >> docIdBuf;
            int docId = Validator::strToInt(docIdBuf);
            Doctor* doc = doctors.findById(docId);
            if (doc == nullptr)
            {
                cout << "Doctor not found." << endl;
                continue;
            }
            char date[11];
            int dateAttempts = 0;
            bool validDate = false;
            while (dateAttempts < 3 && !validDate)
            {
                cout << "Enter date (DD-MM-YYYY): ";
                cin >> date;
                if (Validator::isValidDate(date))
                {
                    validDate = true;
                }
                else
                {
                    cout << "Invalid date. Use format DD-MM-YYYY." << endl;
                    dateAttempts++;
                }
            }
            if (!validDate)
            {
                continue;
            }
            const char* validSlots[] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };
            cout << "Available time slots:" << endl;
            for (int s = 0; s < 8; s++)
            {
                bool taken = false;
                for (int i = 0; i < appointments.size(); i++)
                {
                    Appointment& a = appointments.getAll()[i];
                    if (a.getDoctorId() == docId &&
                        Validator::strComp(a.getDate(), date) == 0 &&
                        Validator::strComp(a.getTimeSlot(), validSlots[s]) == 0 &&
                        Validator::strComp(a.getStatus(), "cancelled") != 0)
                    {
                        taken = true;
                        break;
                    }
                }
                if (!taken)
                {
                    cout << validSlots[s] << endl;
                }
            }
            char slot[6];
            bool booked = false;
            while (!booked)
            {
                cout << "Enter time slot: ";
                cin >> slot;
                if (!Validator::isValidTimeSlot(slot))
                {
                    cout << "Invalid time slot." << endl;
                    continue;
                }
                bool taken = false;
                for (int i = 0; i < appointments.size(); i++)
                {
                    Appointment& a = appointments.getAll()[i];
                    if (a.getDoctorId() == docId &&
                        Validator::strComp(a.getDate(), date) == 0 &&
                        Validator::strComp(a.getTimeSlot(), slot) == 0 &&
                        Validator::strComp(a.getStatus(), "cancelled") != 0)
                    {
                        taken = true;
                        break;
                    }
                }
                if (taken)
                {
                    try
                    {
                        throw SlotUnavailableException("Slot is already taken. Please choose another.");
                    }
                    catch (SlotUnavailableException& e)
                    {
                        cout << e.what() << endl;
                        cout << "Available time slots:" << endl;
                        for (int s = 0; s < 8; s++)
                        {
                            bool t2 = false;
                            for (int i = 0; i < appointments.size(); i++)
                            {
                                Appointment& a = appointments.getAll()[i];
                                if (a.getDoctorId() == docId &&
                                    Validator::strComp(a.getDate(), date) == 0 &&
                                    Validator::strComp(a.getTimeSlot(), validSlots[s]) == 0 &&
                                    Validator::strComp(a.getStatus(), "cancelled") != 0)
                                {
                                    t2 = true;
                                    break;
                                }
                            }
                            if (!t2)
                            {
                                cout << validSlots[s] << endl;
                            }
                        }
                        continue;
                    }
                }
                if (patient.getBalance() < doc->getFee())
                {
                    try
                    {
                        throw InsufficientFundsException("Insufficient balance to book appointment.");
                    }
                    catch (InsufficientFundsException& e)
                    {
                        cout << e.what() << endl;
                        break;
                    }
                }
                int maxAId = 0;
                for (int i = 0; i < appointments.size(); i++)
                {
                    if (appointments.getAll()[i].getAppointmentId() > maxAId)
                    {
                        maxAId = appointments.getAll()[i].getAppointmentId();
                    }
                }
                int newAId = maxAId + 1;
                Appointment newAppt(newAId, patient.getId(), docId, date, slot, "pending");
                appointments.add(newAppt);
                FileHandler::appendAppointment(newAppt);
                int maxBId = 0;
                for (int i = 0; i < bills.size(); i++)
                {
                    if (bills.getAll()[i].getBillId() > maxBId)
                    {
                        maxBId = bills.getAll()[i].getBillId();
                    }
                }
                int newBId = maxBId + 1;
                char today[11];
                FileHandler::getTodayDate(today);
                Bill newBill(newBId, patient.getId(), newAId, doc->getFee(), "unpaid", today);
                bills.add(newBill);
                FileHandler::appendBill(newBill);
                patient -= doc->getFee();
                FileHandler::updatePatient(patient);
                char idBuf[16];
                Validator::intToStr(newAId, idBuf);
                cout << "Appointment booked successfully. Appointment ID: " << idBuf << endl;
                booked = true;
            }
        }
        else if (choice == 2)
        {
            bool hasPending = false;
            for (int i = 0; i < appointments.size(); i++)
            {
                Appointment& a = appointments.getAll()[i];
                if (a.getPatientId() == patient.getId() && Validator::strComp(a.getStatus(), "pending") == 0)
                {
                    Doctor* doc = doctors.findById(a.getDoctorId());
                    cout << "ID: " << a.getAppointmentId()
                         << " | Doctor: " << (doc ? doc->getName() : "Unknown")
                         << " | Date: " << a.getDate()
                         << " | Time: " << a.getTimeSlot() << endl;
                    hasPending = true;
                }
            }
            if (!hasPending)
            {
                cout << "You have no pending appointments." << endl;
                continue;
            }
            cout << "Enter Appointment ID to cancel: ";
            char aBuf[16];
            cin >> aBuf;
            int aId = Validator::strToInt(aBuf);
            Appointment* appt = appointments.findById(aId);
            if (appt == nullptr || appt->getPatientId() != patient.getId() || Validator::strComp(appt->getStatus(), "pending") != 0)
            {
                cout << "Invalid appointment ID." << endl;
                continue;
            }
            Doctor* doc = doctors.findById(appt->getDoctorId());
            float refund = doc ? doc->getFee() : 0.0f;
            appt->setStatus("cancelled");
            FileHandler::updateAppointment(*appt);
            patient += refund;
            FileHandler::updatePatient(patient);
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.getAll()[i].getAppointmentId() == aId)
                {
                    bills.getAll()[i].setStatus("cancelled");
                    FileHandler::updateBill(bills.getAll()[i]);
                    break;
                }
            }
            char refBuf[32];
            Validator::floatToStr(refund, refBuf);
            cout << "Appointment cancelled. PKR " << refBuf << " refunded to your balance." << endl;
        }
        else if (choice == 3)
        {
            Appointment tmp[100];
            int cnt = 0;
            for (int i = 0; i < appointments.size(); i++)
            {
                if (appointments.getAll()[i].getPatientId() == patient.getId())
                {
                    tmp[cnt++] = appointments.getAll()[i];
                }
            }
            if (cnt == 0)
            {
                cout << "No appointments found." << endl;
                continue;
            }
            sortAppointmentsByDateAsc(tmp, cnt);
            for (int i = 0; i < cnt; i++)
            {
                Doctor* doc = doctors.findById(tmp[i].getDoctorId());
                cout << "ID: " << tmp[i].getAppointmentId()
                     << " | Doctor: " << (doc ? doc->getName() : "Unknown")
                     << " | Spec: " << (doc ? doc->getSpecialization() : "Unknown")
                     << " | Date: " << tmp[i].getDate()
                     << " | Time: " << tmp[i].getTimeSlot()
                     << " | Status: " << tmp[i].getStatus() << endl;
            }
        }
        else if (choice == 4)
        {
            Prescription tmp[100];
            int cnt = 0;
            for (int i = 0; i < prescriptions.size(); i++)
            {
                if (prescriptions.getAll()[i].getPatientId() == patient.getId())
                {
                    tmp[cnt++] = prescriptions.getAll()[i];
                }
            }
            if (cnt == 0)
            {
                cout << "No medical records found." << endl;
                continue;
            }
            sortPrescriptionsByDateDesc(tmp, cnt);
            for (int i = 0; i < cnt; i++)
            {
                Doctor* doc = doctors.findById(tmp[i].getDoctorId());
                cout << "Date: " << tmp[i].getDate()
                     << " | Doctor: " << (doc ? doc->getName() : "Unknown")
                     << " | Medicines: " << tmp[i].getMedicines()
                     << " | Notes: " << tmp[i].getNotes() << endl;
            }
        }
        else if (choice == 5)
        {
            float totalUnpaid = 0.0f;
            bool found = false;
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.getAll()[i].getPatientId() == patient.getId())
                {
                    char amtBuf[32];
                    Validator::floatToStr(bills.getAll()[i].getAmount(), amtBuf);
                    cout << "Bill ID: " << bills.getAll()[i].getBillId()
                         << " | Appt ID: " << bills.getAll()[i].getAppointmentId()
                         << " | Amount: PKR " << amtBuf
                         << " | Status: " << bills.getAll()[i].getStatus()
                         << " | Date: " << bills.getAll()[i].getDate() << endl;
                    if (Validator::strComp(bills.getAll()[i].getStatus(), "unpaid") == 0)
                    {
                        totalUnpaid += bills.getAll()[i].getAmount();
                    }
                    found = true;
                }
            }
            if (!found)
            {
                cout << "No bills found." << endl;
                continue;
            }
            char totBuf[32];
            Validator::floatToStr(totalUnpaid, totBuf);
            cout << "Total outstanding: PKR " << totBuf << endl;
        }
        else if (choice == 6)
        {
            bool hasUnpaid = false;
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.getAll()[i].getPatientId() == patient.getId() && Validator::strComp(bills.getAll()[i].getStatus(), "unpaid") == 0)
                {
                    char amtBuf[32];
                    Validator::floatToStr(bills.getAll()[i].getAmount(), amtBuf);
                    cout << "Bill ID: " << bills.getAll()[i].getBillId()
                         << " | Amount: PKR " << amtBuf
                         << " | Date: " << bills.getAll()[i].getDate() << endl;
                    hasUnpaid = true;
                }
            }
            if (!hasUnpaid)
            {
                cout << "No unpaid bills." << endl;
                continue;
            }
            cout << "Enter Bill ID to pay: ";
            char bBuf[16];
            cin >> bBuf;
            int bId = Validator::strToInt(bBuf);
            Bill* bill = bills.findById(bId);
            if (bill == nullptr || bill->getPatientId() != patient.getId() || Validator::strComp(bill->getStatus(), "unpaid") != 0)
            {
                cout << "Invalid Bill ID." << endl;
                continue;
            }
            if (patient.getBalance() < bill->getAmount())
            {
                try
                {
                    throw InsufficientFundsException("Insufficient balance to pay bill.");
                }
                catch (InsufficientFundsException& e)
                {
                    cout << e.what() << endl;
                    continue;
                }
            }
            patient -= bill->getAmount();
            bill->setStatus("paid");
            FileHandler::updateBill(*bill);
            FileHandler::updatePatient(patient);
            char balBuf2[32];
            Validator::floatToStr(patient.getBalance(), balBuf2);
            cout << "Bill paid successfully. Remaining balance: PKR " << balBuf2 << endl;
        }
        else if (choice == 7)
        {
            int attempts = 0;
            while (attempts < 3)
            {
                cout << "Enter amount to add (PKR): ";
                char amtBuf[32];
                cin >> amtBuf;
                if (!Validator::isPositiveFloat(amtBuf))
                {
                    try
                    {
                        throw InvalidInputException("Invalid amount. Must be a positive number.");
                    }
                    catch (InvalidInputException& e)
                    {
                        cout << e.what() << endl;
                        attempts++;
                        continue;
                    }
                }
                float amt = Validator::strToFloat(amtBuf);
                patient += amt;
                FileHandler::updatePatient(patient);
                char newBalBuf[32];
                Validator::floatToStr(patient.getBalance(), newBalBuf);
                cout << "Balance updated. New balance: PKR " << newBalBuf << endl;
                break;
            }
        }
    }
    while (choice != 8);
    cout << "Logged out." << endl;
}

void doctorMenu(Doctor& doctor, Storage<Patient>& patients, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions)
{
    int choice = 0;
    do
    {
        cout << endl;
        cout << "Welcome, Dr. " << doctor.getName() << " | Specialization: " << doctor.getSpecialization() << endl;
        cout << "===============================================" << endl;
        cout << "1. View Today's Appointments" << endl;
        cout << "2. Mark Appointment Complete" << endl;
        cout << "3. Mark Appointment No-Show" << endl;
        cout << "4. Write Prescription" << endl;
        cout << "5. View Patient Medical History" << endl;
        cout << "6. Logout" << endl;
        cout << "Enter choice: ";
        choice = getMenuChoice(1, 6);
        if (choice == -1)
        {
            cout << "Invalid choice." << endl;
            continue;
        }
        char today[11];
        FileHandler::getTodayDate(today);
        if (choice == 1)
        {
            Appointment tmp[100];
            int cnt = 0;
            for (int i = 0; i < appointments.size(); i++)
            {
                Appointment& a = appointments.getAll()[i];
                if (a.getDoctorId() == doctor.getId() && Validator::strComp(a.getDate(), today) == 0)
                {
                    tmp[cnt++] = a;
                }
            }
            if (cnt == 0)
            {
                cout << "No appointments scheduled for today." << endl;
                continue;
            }
            sortAppointmentsByDateAsc(tmp, cnt);
            for (int i = 0; i < cnt; i++)
            {
                Patient* p = patients.findById(tmp[i].getPatientId());
                cout << "ID: " << tmp[i].getAppointmentId()
                     << " | Patient: " << (p ? p->getName() : "Unknown")
                     << " | Time: " << tmp[i].getTimeSlot()
                     << " | Status: " << tmp[i].getStatus() << endl;
            }
        }
        else if (choice == 2)
        {
            bool hasToday = false;
            for (int i = 0; i < appointments.size(); i++)
            {
                Appointment& a = appointments.getAll()[i];
                if (a.getDoctorId() == doctor.getId() && Validator::strComp(a.getDate(), today) == 0 && Validator::strComp(a.getStatus(), "pending") == 0)
                {
                    Patient* p = patients.findById(a.getPatientId());
                    cout << "ID: " << a.getAppointmentId()
                         << " | Patient: " << (p ? p->getName() : "Unknown")
                         << " | Time: " << a.getTimeSlot() << endl;
                    hasToday = true;
                }
            }
            if (!hasToday)
            {
                cout << "No pending appointments today." << endl;
                continue;
            }
            cout << "Enter Appointment ID: ";
            char aBuf[16];
            cin >> aBuf;
            int aId = Validator::strToInt(aBuf);
            Appointment* appt = appointments.findById(aId);
            if (appt == nullptr || appt->getDoctorId() != doctor.getId() || Validator::strComp(appt->getStatus(), "pending") != 0 || Validator::strComp(appt->getDate(), today) != 0)
            {
                cout << "Invalid appointment ID." << endl;
                continue;
            }
            appt->setStatus("completed");
            FileHandler::updateAppointment(*appt);
            cout << "Appointment marked as completed." << endl;
        }
        else if (choice == 3)
        {
            bool hasToday = false;
            for (int i = 0; i < appointments.size(); i++)
            {
                Appointment& a = appointments.getAll()[i];
                if (a.getDoctorId() == doctor.getId() && Validator::strComp(a.getDate(), today) == 0 && Validator::strComp(a.getStatus(), "pending") == 0)
                {
                    Patient* p = patients.findById(a.getPatientId());
                    cout << "ID: " << a.getAppointmentId()
                         << " | Patient: " << (p ? p->getName() : "Unknown")
                         << " | Time: " << a.getTimeSlot() << endl;
                    hasToday = true;
                }
            }
            if (!hasToday)
            {
                cout << "No pending appointments today." << endl;
                continue;
            }
            cout << "Enter Appointment ID: ";
            char aBuf[16];
            cin >> aBuf;
            int aId = Validator::strToInt(aBuf);
            Appointment* appt = appointments.findById(aId);
            if (appt == nullptr || appt->getDoctorId() != doctor.getId() || Validator::strComp(appt->getStatus(), "pending") != 0 || Validator::strComp(appt->getDate(), today) != 0)
            {
                cout << "Invalid appointment ID." << endl;
                continue;
            }
            appt->setStatus("noshow");
            FileHandler::updateAppointment(*appt);
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.getAll()[i].getAppointmentId() == aId)
                {
                    bills.getAll()[i].setStatus("cancelled");
                    FileHandler::updateBill(bills.getAll()[i]);
                    break;
                }
            }
            cout << "Appointment marked as no-show." << endl;
        }
        else if (choice == 4)
        {
            cout << "Enter Appointment ID: ";
            char aBuf[16];
            cin >> aBuf;
            int aId = Validator::strToInt(aBuf);
            Appointment* appt = appointments.findById(aId);
            if (appt == nullptr || appt->getDoctorId() != doctor.getId() || Validator::strComp(appt->getStatus(), "completed") != 0)
            {
                cout << "Invalid appointment ID." << endl;
                continue;
            }
            for (int i = 0; i < prescriptions.size(); i++)
            {
                if (prescriptions.getAll()[i].getAppointmentId() == aId)
                {
                    cout << "Prescription already written for this appointment." << endl;
                    goto endPrescription;
                }
            }
            {
                char medicines[500];
                char notes[300];
                cin.ignore();
                cout << "Enter medicines (format: MedicineName Dosage; e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
                cin.getline(medicines, 500);
                cout << "Enter notes (max 300 chars): ";
                cin.getline(notes, 300);
                int maxPrId = 0;
                for (int i = 0; i < prescriptions.size(); i++)
                {
                    if (prescriptions.getAll()[i].getPrescriptionId() > maxPrId)
                    {
                        maxPrId = prescriptions.getAll()[i].getPrescriptionId();
                    }
                }
                int newPrId = maxPrId + 1;
                Prescription pr(newPrId, aId, appt->getPatientId(), doctor.getId(), appt->getDate(), medicines, notes);
                prescriptions.add(pr);
                FileHandler::appendPrescription(pr);
                cout << "Prescription saved." << endl;
            }
            endPrescription:;
        }
        else if (choice == 5)
        {
            cout << "Enter Patient ID: ";
            char pBuf[16];
            cin >> pBuf;
            int pId = Validator::strToInt(pBuf);
            Patient* p = patients.findById(pId);
            if (p == nullptr)
            {
                cout << "Access denied. You can only view records of your own patients." << endl;
                continue;
            }
            bool hasCompleted = false;
            for (int i = 0; i < appointments.size(); i++)
            {
                Appointment& a = appointments.getAll()[i];
                if (a.getPatientId() == pId && a.getDoctorId() == doctor.getId() && Validator::strComp(a.getStatus(), "completed") == 0)
                {
                    hasCompleted = true;
                    break;
                }
            }
            if (!hasCompleted)
            {
                cout << "Access denied. You can only view records of your own patients." << endl;
                continue;
            }
            Prescription tmp[100];
            int cnt = 0;
            for (int i = 0; i < prescriptions.size(); i++)
            {
                if (prescriptions.getAll()[i].getPatientId() == pId && prescriptions.getAll()[i].getDoctorId() == doctor.getId())
                {
                    tmp[cnt++] = prescriptions.getAll()[i];
                }
            }
            if (cnt == 0)
            {
                cout << "No medical records found." << endl;
                continue;
            }
            sortPrescriptionsByDateDesc(tmp, cnt);
            for (int i = 0; i < cnt; i++)
            {
                cout << "Date: " << tmp[i].getDate()
                     << " | Medicines: " << tmp[i].getMedicines()
                     << " | Notes: " << tmp[i].getNotes() << endl;
            }
        }
    }
    while (choice != 6);
    cout << "Logged out." << endl;
}

void adminMenu(Admin& admin, Storage<Patient>& patients, Storage<Doctor>& doctors, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions)
{
    int choice = 0;
    do
    {
        cout << endl;
        cout << "Admin Panel - MediCore" << endl;
        cout << "======================" << endl;
        cout << "1. Add Doctor" << endl;
        cout << "2. Remove Doctor" << endl;
        cout << "3. Add Patient" << endl;
        cout << "4. Remove Patient" << endl;
        cout << "5. View All Patients" << endl;
        cout << "6. View All Doctors" << endl;
        cout << "7. View All Appointments" << endl;
        cout << "8. View Unpaid Bills" << endl;
        cout << "9. Discharge Patient" << endl;
        cout << "10. View Security Log" << endl;
        cout << "11. Generate Daily Report" << endl;
        cout << "12. Logout" << endl;
        cout << "Enter choice: ";
        choice = getMenuChoice(1, 12);
        if (choice == -1)
        {
            cout << "Invalid choice." << endl;
            continue;
        }
        if (choice == 1)
        {
            char name[51], spec[51], contact[12], password[51], feeBuf[32];
            cin.ignore();
            cout << "Enter name (max 50 chars): ";
            cin.getline(name, 51);
            cout << "Enter specialization (max 50 chars): ";
            cin.getline(spec, 51);
            cout << "Enter contact (11 digits): ";
            cin >> contact;
            if (!Validator::isValidContact(contact))
            {
                cout << "Invalid contact." << endl;
                continue;
            }
            cout << "Enter password (min 6 chars): ";
            cin >> password;
            if (!Validator::isValidPassword(password))
            {
                cout << "Invalid password." << endl;
                continue;
            }
            cout << "Enter consultation fee: ";
            cin >> feeBuf;
            if (!Validator::isPositiveFloat(feeBuf))
            {
                cout << "Invalid fee." << endl;
                continue;
            }
            float fee = Validator::strToFloat(feeBuf);
            int maxId = 0;
            for (int i = 0; i < doctors.size(); i++)
            {
                if (doctors.getAll()[i].getId() > maxId)
                {
                    maxId = doctors.getAll()[i].getId();
                }
            }
            int newId = maxId + 1;
            Doctor d(newId, name, spec, contact, password, fee);
            doctors.add(d);
            FileHandler::appendDoctor(d);
            char idBuf[16];
            Validator::intToStr(newId, idBuf);
            cout << "Doctor added successfully. ID: " << idBuf << endl;
        }
        else if (choice == 2)
        {
            for (int i = 0; i < doctors.size(); i++)
            {
                char feeBuf[32];
                Validator::floatToStr(doctors.getAll()[i].getFee(), feeBuf);
                cout << "ID: " << doctors.getAll()[i].getId()
                     << " | Name: " << doctors.getAll()[i].getName()
                     << " | Spec: " << doctors.getAll()[i].getSpecialization()
                     << " | Fee: PKR " << feeBuf << endl;
            }
            cout << "Enter Doctor ID to remove: ";
            char dBuf[16];
            cin >> dBuf;
            int dId = Validator::strToInt(dBuf);
            for (int i = 0; i < appointments.size(); i++)
            {
                Appointment& a = appointments.getAll()[i];
                if (a.getDoctorId() == dId && Validator::strComp(a.getStatus(), "pending") == 0)
                {
                    cout << "Cannot remove doctor with pending appointments. Cancel or reassign them first." << endl;
                    goto endRemoveDoc;
                }
            }
            doctors.removeById(dId);
            FileHandler::deleteDoctor(dId);
            cout << "Doctor removed." << endl;
            endRemoveDoc:;
        }
        else if (choice == 3)
        {
            char name[51], gender[2], contact[12], password[51], ageBuf[8], balBuf[32];
            cin.ignore();
            cout << "Enter name (max 50 chars): ";
            cin.getline(name, 51);
            cout << "Enter age: ";
            cin >> ageBuf;
            if (!Validator::isPositiveInt(ageBuf))
            {
                cout << "Invalid age." << endl;
                continue;
            }
            int age = Validator::strToInt(ageBuf);
            cout << "Enter gender (M/F): ";
            cin >> gender;
            if (!Validator::isValidGender(gender))
            {
                cout << "Invalid gender." << endl;
                continue;
            }
            cout << "Enter contact (11 digits): ";
            cin >> contact;
            if (!Validator::isValidContact(contact))
            {
                cout << "Invalid contact." << endl;
                continue;
            }
            cout << "Enter password (min 6 chars): ";
            cin >> password;
            if (!Validator::isValidPassword(password))
            {
                cout << "Invalid password." << endl;
                continue;
            }
            cout << "Enter initial balance: ";
            cin >> balBuf;
            if (!Validator::isPositiveFloat(balBuf))
            {
                cout << "Invalid balance." << endl;
                continue;
            }
            float bal = Validator::strToFloat(balBuf);
            int maxId = 0;
            for (int i = 0; i < patients.size(); i++)
            {
                if (patients.getAll()[i].getId() > maxId)
                {
                    maxId = patients.getAll()[i].getId();
                }
            }
            int newId = maxId + 1;
            Patient p(newId, name, age, gender, contact, password, bal);
            patients.add(p);
            FileHandler::appendPatient(p);
            char idBuf[16];
            Validator::intToStr(newId, idBuf);
            cout << "Patient added successfully. ID: " << idBuf << endl;
        }
        else if (choice == 4)
        {
            for (int i = 0; i < patients.size(); i++)
            {
                char balBuf[32];
                Validator::floatToStr(patients.getAll()[i].getBalance(), balBuf);
                cout << "ID: " << patients.getAll()[i].getId()
                     << " | Name: " << patients.getAll()[i].getName()
                     << " | Age: " << patients.getAll()[i].getAge()
                     << " | Gender: " << patients.getAll()[i].getGender()
                     << " | Contact: " << patients.getAll()[i].getContact()
                     << " | Balance: PKR " << balBuf << endl;
            }
            cout << "Enter Patient ID to remove: ";
            char pBuf[16];
            cin >> pBuf;
            int pId = Validator::strToInt(pBuf);
            for (int i = 0; i < appointments.size(); i++)
            {
                if (appointments.getAll()[i].getPatientId() == pId && Validator::strComp(appointments.getAll()[i].getStatus(), "pending") == 0)
                {
                    cout << "Cannot remove patient with pending appointments." << endl;
                    goto endRemovePatient;
                }
            }
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.getAll()[i].getPatientId() == pId && Validator::strComp(bills.getAll()[i].getStatus(), "unpaid") == 0)
                {
                    cout << "Cannot remove patient with unpaid bills." << endl;
                    goto endRemovePatient;
                }
            }
            patients.removeById(pId);
            FileHandler::deletePatient(pId);
            FileHandler::deleteAppointmentsByPatient(pId);
            FileHandler::deleteBillsByPatient(pId);
            FileHandler::deletePrescriptionsByPatient(pId);
            cout << "Patient removed successfully." << endl;
            endRemovePatient:;
        }
        else if (choice == 5)
        {
            for (int i = 0; i < patients.size(); i++)
            {
                int unpaidCount = 0;
                for (int j = 0; j < bills.size(); j++)
                {
                    if (bills.getAll()[j].getPatientId() == patients.getAll()[i].getId() && Validator::strComp(bills.getAll()[j].getStatus(), "unpaid") == 0)
                    {
                        unpaidCount++;
                    }
                }
                char balBuf[32];
                Validator::floatToStr(patients.getAll()[i].getBalance(), balBuf);
                cout << "ID: " << patients.getAll()[i].getId()
                     << " | Name: " << patients.getAll()[i].getName()
                     << " | Age: " << patients.getAll()[i].getAge()
                     << " | Gender: " << patients.getAll()[i].getGender()
                     << " | Contact: " << patients.getAll()[i].getContact()
                     << " | Balance: PKR " << balBuf
                     << " | Unpaid Bills: " << unpaidCount << endl;
            }
        }
        else if (choice == 6)
        {
            for (int i = 0; i < doctors.size(); i++)
            {
                char feeBuf[32];
                Validator::floatToStr(doctors.getAll()[i].getFee(), feeBuf);
                cout << "ID: " << doctors.getAll()[i].getId()
                     << " | Name: " << doctors.getAll()[i].getName()
                     << " | Spec: " << doctors.getAll()[i].getSpecialization()
                     << " | Contact: " << doctors.getAll()[i].getContact()
                     << " | Fee: PKR " << feeBuf << endl;
            }
        }
        else if (choice == 7)
        {
            Appointment tmp[100];
            int cnt = 0;
            for (int i = 0; i < appointments.size(); i++)
            {
                tmp[cnt++] = appointments.getAll()[i];
            }
            sortAppointmentsByDateDesc(tmp, cnt);
            for (int i = 0; i < cnt; i++)
            {
                Patient* p = patients.findById(tmp[i].getPatientId());
                Doctor* d = doctors.findById(tmp[i].getDoctorId());
                cout << "ID: " << tmp[i].getAppointmentId()
                     << " | Patient: " << (p ? p->getName() : "Unknown")
                     << " | Doctor: " << (d ? d->getName() : "Unknown")
                     << " | Date: " << tmp[i].getDate()
                     << " | Time: " << tmp[i].getTimeSlot()
                     << " | Status: " << tmp[i].getStatus() << endl;
            }
        }
        else if (choice == 8)
        {
            char today[11];
            FileHandler::getTodayDate(today);
            for (int i = 0; i < bills.size(); i++)
            {
                if (Validator::strComp(bills.getAll()[i].getStatus(), "unpaid") == 0)
                {
                    Patient* p = patients.findById(bills.getAll()[i].getPatientId());
                    char amtBuf[32];
                    Validator::floatToStr(bills.getAll()[i].getAmount(), amtBuf);
                    int diff = dateDiffFromToday(bills.getAll()[i].getDate());
                    cout << "Bill ID: " << bills.getAll()[i].getBillId()
                         << " | Patient: " << (p ? p->getName() : "Unknown")
                         << " | Amount: PKR " << amtBuf
                         << " | Date: " << bills.getAll()[i].getDate();
                    if (diff > 7)
                    {
                        cout << " [OVERDUE]";
                    }
                    cout << endl;
                }
            }
        }
        else if (choice == 9)
        {
            cout << "Enter Patient ID: ";
            char pBuf[16];
            cin >> pBuf;
            int pId = Validator::strToInt(pBuf);
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.getAll()[i].getPatientId() == pId && Validator::strComp(bills.getAll()[i].getStatus(), "unpaid") == 0)
                {
                    cout << "Cannot discharge patient with unpaid bills." << endl;
                    goto endDischarge;
                }
            }
            for (int i = 0; i < appointments.size(); i++)
            {
                if (appointments.getAll()[i].getPatientId() == pId && Validator::strComp(appointments.getAll()[i].getStatus(), "pending") == 0)
                {
                    cout << "Cannot discharge patient with pending appointments." << endl;
                    goto endDischarge;
                }
            }
            FileHandler::archivePatient(pId, patients, appointments, bills, prescriptions);
            patients.removeById(pId);
            FileHandler::deletePatient(pId);
            FileHandler::deleteAppointmentsByPatient(pId);
            FileHandler::deleteBillsByPatient(pId);
            FileHandler::deletePrescriptionsByPatient(pId);
            cout << "Patient discharged and archived successfully." << endl;
            endDischarge:;
        }
        else if (choice == 10)
        {
            ifstream logFile("data/security_log.txt");
            if (!logFile.is_open())
            {
                cout << "No security events logged." << endl;
                continue;
            }
            char line[256];
            bool hasContent = false;
            while (logFile.getline(line, 256))
            {
                if (line[0] != '\0')
                {
                    cout << line << endl;
                    hasContent = true;
                }
            }
            logFile.close();
            if (!hasContent)
            {
                cout << "No security events logged." << endl;
            }
        }
        else if (choice == 11)
        {
            char today[11];
            FileHandler::getTodayDate(today);
            int total = 0, pending = 0, completed = 0, noshow = 0, cancelled = 0;
            float revenue = 0.0f;
            for (int i = 0; i < appointments.size(); i++)
            {
                if (Validator::strComp(appointments.getAll()[i].getDate(), today) == 0)
                {
                    total++;
                    const char* st = appointments.getAll()[i].getStatus();
                    if (Validator::strComp(st, "pending") == 0) { pending++; }
                    else if (Validator::strComp(st, "completed") == 0) { completed++; }
                    else if (Validator::strComp(st, "noshow") == 0) { noshow++; }
                    else if (Validator::strComp(st, "cancelled") == 0) { cancelled++; }
                }
            }
            cout << "Total appointments today: " << total
                 << " (Pending: " << pending
                 << " Completed: " << completed
                 << " No-show: " << noshow
                 << " Cancelled: " << cancelled << ")" << endl;
            for (int i = 0; i < bills.size(); i++)
            {
                if (Validator::strComp(bills.getAll()[i].getStatus(), "paid") == 0 && Validator::strComp(bills.getAll()[i].getDate(), today) == 0)
                {
                    revenue += bills.getAll()[i].getAmount();
                }
            }
            char revBuf[32];
            Validator::floatToStr(revenue, revBuf);
            cout << "Revenue collected today (paid bills): PKR " << revBuf << endl;
            cout << "Patients with outstanding unpaid bills:" << endl;
            for (int i = 0; i < patients.size(); i++)
            {
                float owed = 0.0f;
                for (int j = 0; j < bills.size(); j++)
                {
                    if (bills.getAll()[j].getPatientId() == patients.getAll()[i].getId() && Validator::strComp(bills.getAll()[j].getStatus(), "unpaid") == 0)
                    {
                        owed += bills.getAll()[j].getAmount();
                    }
                }
                if (owed > 0.0f)
                {
                    char owedBuf[32];
                    Validator::floatToStr(owed, owedBuf);
                    cout << patients.getAll()[i].getName() << " | Total Owed: PKR " << owedBuf << endl;
                }
            }
            cout << "Doctor-wise summary for today:" << endl;
            for (int d = 0; d < doctors.size(); d++)
            {
                int dComp = 0, dPend = 0, dNo = 0;
                for (int i = 0; i < appointments.size(); i++)
                {
                    if (appointments.getAll()[i].getDoctorId() == doctors.getAll()[d].getId() && Validator::strComp(appointments.getAll()[i].getDate(), today) == 0)
                    {
                        const char* st = appointments.getAll()[i].getStatus();
                        if (Validator::strComp(st, "completed") == 0) { dComp++; }
                        else if (Validator::strComp(st, "pending") == 0) { dPend++; }
                        else if (Validator::strComp(st, "noshow") == 0) { dNo++; }
                    }
                }
                cout << "Dr. " << doctors.getAll()[d].getName()
                     << " | Completed: " << dComp
                     << " | Pending: " << dPend
                     << " | No-show: " << dNo << endl;
            }
        }
    }
    while (choice != 12);
    cout << "Logged out." << endl;
}