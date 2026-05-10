#include "../include/FileHandler.h"
#include "../include/Validator.h"
#include "../include/Exceptions.h"
#include <fstream>
#include <ctime>
using namespace std;

void FileHandler::parseLine(const char* line, char fields[][256], int& count)
{
    count = 0;
    int fi = 0;
    int ci = 0;
    while (line[fi] != '\0' && line[fi] != '\n' && line[fi] != '\r')
    {
        if (line[fi] == ',')
        {
            fields[count][ci] = '\0';
            count++;
            ci = 0;
        }
        else
        {
            fields[count][ci] = line[fi];
            ci++;
        }
        fi++;
    }
    fields[count][ci] = '\0';
    count++;
}

void FileHandler::getTodayDate(char* buf)
{
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char dayBuf[3], monBuf[3], yearBuf[5];
    Validator::intToStr(t->tm_mday, dayBuf);
    Validator::intToStr(t->tm_mon + 1, monBuf);
    Validator::intToStr(t->tm_year + 1900, yearBuf);
    buf[0] = '\0';
    if (t->tm_mday < 10)
    {
        buf[0] = '0';
        buf[1] = dayBuf[0];
        buf[2] = '-';
        buf[3] = '\0';
    }
    else
    {
        buf[0] = dayBuf[0];
        buf[1] = dayBuf[1];
        buf[2] = '-';
        buf[3] = '\0';
    }
    if (t->tm_mon + 1 < 10)
    {
        char tmp[4];
        tmp[0] = '0';
        tmp[1] = monBuf[0];
        tmp[2] = '-';
        tmp[3] = '\0';
        Validator::strConcat(buf, tmp);
    }
    else
    {
        char tmp[4];
        tmp[0] = monBuf[0];
        tmp[1] = monBuf[1];
        tmp[2] = '-';
        tmp[3] = '\0';
        Validator::strConcat(buf, tmp);
    }
    Validator::strConcat(buf, yearBuf);
}

void FileHandler::writePatientLine(char* buf, const Patient& p)
{
    char idBuf[16], ageBuf[16], balBuf[32];
    Validator::intToStr(p.getId(), idBuf);
    Validator::intToStr(p.getAge(), ageBuf);
    Validator::floatToStr(p.getBalance(), balBuf);
    buf[0] = '\0';
    Validator::strConcat(buf, idBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, p.getName());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, ageBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, p.getGender());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, p.getContact());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, p.getPassword());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, balBuf);
}

void FileHandler::writeDoctorLine(char* buf, const Doctor& d)
{
    char idBuf[16], feeBuf[32];
    Validator::intToStr(d.getId(), idBuf);
    Validator::floatToStr(d.getFee(), feeBuf);
    buf[0] = '\0';
    Validator::strConcat(buf, idBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, d.getName());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, d.getSpecialization());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, d.getContact());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, d.getPassword());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, feeBuf);
}

void FileHandler::writeAppointmentLine(char* buf, const Appointment& a)
{
    char idBuf[16], pBuf[16], dBuf[16];
    Validator::intToStr(a.getAppointmentId(), idBuf);
    Validator::intToStr(a.getPatientId(), pBuf);
    Validator::intToStr(a.getDoctorId(), dBuf);
    buf[0] = '\0';
    Validator::strConcat(buf, idBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, pBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, dBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, a.getDate());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, a.getTimeSlot());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, a.getStatus());
}

void FileHandler::writeBillLine(char* buf, const Bill& b)
{
    char idBuf[16], pBuf[16], aBuf[16], amtBuf[32];
    Validator::intToStr(b.getBillId(), idBuf);
    Validator::intToStr(b.getPatientId(), pBuf);
    Validator::intToStr(b.getAppointmentId(), aBuf);
    Validator::floatToStr(b.getAmount(), amtBuf);
    buf[0] = '\0';
    Validator::strConcat(buf, idBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, pBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, aBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, amtBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, b.getStatus());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, b.getDate());
}

void FileHandler::writePrescriptionLine(char* buf, const Prescription& pr)
{
    char idBuf[16], aBuf[16], pBuf[16], dBuf[16];
    Validator::intToStr(pr.getPrescriptionId(), idBuf);
    Validator::intToStr(pr.getAppointmentId(), aBuf);
    Validator::intToStr(pr.getPatientId(), pBuf);
    Validator::intToStr(pr.getDoctorId(), dBuf);
    buf[0] = '\0';
    Validator::strConcat(buf, idBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, aBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, pBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, dBuf);
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, pr.getDate());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, pr.getMedicines());
    Validator::strConcat(buf, ",");
    Validator::strConcat(buf, pr.getNotes());
}

void FileHandler::loadPatients(Storage<Patient>& storage)
{
    ifstream file("data/patients.txt");
    if (!file.is_open())
    {
        throw FileNotFoundException("Cannot open data/patients.txt");
    }
    char line[512];
    while (file.getline(line, 512))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt < 7)
        {
            continue;
        }
        int id = Validator::strToInt(fields[0]);
        int age = Validator::strToInt(fields[2]);
        float bal = Validator::strToFloat(fields[6]);
        Patient p(id, fields[1], age, fields[3], fields[4], fields[5], bal);
        storage.add(p);
    }
    file.close();
}

void FileHandler::loadDoctors(Storage<Doctor>& storage)
{
    ifstream file("data/doctors.txt");
    if (!file.is_open())
    {
        throw FileNotFoundException("Cannot open data/doctors.txt");
    }
    char line[512];
    while (file.getline(line, 512))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt < 6)
        {
            continue;
        }
        int id = Validator::strToInt(fields[0]);
        float fee = Validator::strToFloat(fields[5]);
        Doctor d(id, fields[1], fields[2], fields[3], fields[4], fee);
        storage.add(d);
    }
    file.close();
}

void FileHandler::loadAdmin(Admin& admin)
{
    ifstream file("data/admin.txt");
    if (!file.is_open())
    {
        throw FileNotFoundException("Cannot open data/admin.txt");
    }
    char line[256];
    if (file.getline(line, 256))
    {
        char fields[5][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 3)
        {
            int id = Validator::strToInt(fields[0]);
            admin = Admin(id, fields[1], fields[2]);
        }
    }
    file.close();
}

void FileHandler::loadAppointments(Storage<Appointment>& storage)
{
    ifstream file("data/appointments.txt");
    if (!file.is_open())
    {
        throw FileNotFoundException("Cannot open data/appointments.txt");
    }
    char line[256];
    while (file.getline(line, 256))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt < 6)
        {
            continue;
        }
        int aId = Validator::strToInt(fields[0]);
        int pId = Validator::strToInt(fields[1]);
        int dId = Validator::strToInt(fields[2]);
        Appointment a(aId, pId, dId, fields[3], fields[4], fields[5]);
        storage.add(a);
    }
    file.close();
}

void FileHandler::loadBills(Storage<Bill>& storage)
{
    ifstream file("data/bills.txt");
    if (!file.is_open())
    {
        throw FileNotFoundException("Cannot open data/bills.txt");
    }
    char line[256];
    while (file.getline(line, 256))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt < 6)
        {
            continue;
        }
        int bId = Validator::strToInt(fields[0]);
        int pId = Validator::strToInt(fields[1]);
        int aId = Validator::strToInt(fields[2]);
        float amt = Validator::strToFloat(fields[3]);
        Bill b(bId, pId, aId, amt, fields[4], fields[5]);
        storage.add(b);
    }
    file.close();
}

void FileHandler::loadPrescriptions(Storage<Prescription>& storage)
{
    ifstream file("data/prescriptions.txt");
    if (!file.is_open())
    {
        throw FileNotFoundException("Cannot open data/prescriptions.txt");
    }
    char line[1024];
    while (file.getline(line, 1024))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][512];
        int fi = 0;
        int cnt = 0;
        int ci = 0;
        int fieldLimit[10] = { 256, 256, 256, 256, 256, 500, 300 };
        while (line[fi] != '\0' && line[fi] != '\n' && line[fi] != '\r')
        {
            if (line[fi] == ',' && cnt < 6)
            {
                fields[cnt][ci] = '\0';
                cnt++;
                ci = 0;
            }
            else
            {
                if (ci < fieldLimit[cnt] - 1)
                {
                    fields[cnt][ci] = line[fi];
                    ci++;
                }
            }
            fi++;
        }
        fields[cnt][ci] = '\0';
        cnt++;
        if (cnt < 7)
        {
            continue;
        }
        int prId = Validator::strToInt(fields[0]);
        int aId = Validator::strToInt(fields[1]);
        int pId = Validator::strToInt(fields[2]);
        int dId = Validator::strToInt(fields[3]);
        Prescription pr(prId, aId, pId, dId, fields[4], fields[5], fields[6]);
        storage.add(pr);
    }
    file.close();
}

void FileHandler::appendPatient(const Patient& p)
{
    ofstream file("data/patients.txt", ios::app);
    char buf[512];
    writePatientLine(buf, p);
    file << buf << endl;
    file.close();
}

void FileHandler::appendDoctor(const Doctor& d)
{
    ofstream file("data/doctors.txt", ios::app);
    char buf[512];
    writeDoctorLine(buf, d);
    file << buf << endl;
    file.close();
}

void FileHandler::appendAppointment(const Appointment& a)
{
    ofstream file("data/appointments.txt", ios::app);
    char buf[256];
    writeAppointmentLine(buf, a);
    file << buf << endl;
    file.close();
}

void FileHandler::appendBill(const Bill& b)
{
    ofstream file("data/bills.txt", ios::app);
    char buf[256];
    writeBillLine(buf, b);
    file << buf << endl;
    file.close();
}

void FileHandler::appendPrescription(const Prescription& pr)
{
    ofstream file("data/prescriptions.txt", ios::app);
    char buf[1024];
    writePrescriptionLine(buf, pr);
    file << buf << endl;
    file.close();
}

void FileHandler::updatePatient(const Patient& p)
{
    ifstream fin("data/patients.txt");
    ofstream fout("data/patients_tmp.txt");
    char line[512];
    while (fin.getline(line, 512))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 1 && Validator::strToInt(fields[0]) == p.getId())
        {
            char buf[512];
            writePatientLine(buf, p);
            fout << buf << endl;
        }
        else
        {
            fout << line << endl;
        }
    }
    fin.close();
    fout.close();
    remove("data/patients.txt");
    rename("data/patients_tmp.txt", "data/patients.txt");
}

void FileHandler::updateDoctor(const Doctor& d)
{
    ifstream fin("data/doctors.txt");
    ofstream fout("data/doctors_tmp.txt");
    char line[512];
    while (fin.getline(line, 512))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 1 && Validator::strToInt(fields[0]) == d.getId())
        {
            char buf[512];
            writeDoctorLine(buf, d);
            fout << buf << endl;
        }
        else
        {
            fout << line << endl;
        }
    }
    fin.close();
    fout.close();
    remove("data/doctors.txt");
    rename("data/doctors_tmp.txt", "data/doctors.txt");
}

void FileHandler::updateAppointment(const Appointment& a)
{
    ifstream fin("data/appointments.txt");
    ofstream fout("data/appointments_tmp.txt");
    char line[256];
    while (fin.getline(line, 256))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 1 && Validator::strToInt(fields[0]) == a.getAppointmentId())
        {
            char buf[256];
            writeAppointmentLine(buf, a);
            fout << buf << endl;
        }
        else
        {
            fout << line << endl;
        }
    }
    fin.close();
    fout.close();
    remove("data/appointments.txt");
    rename("data/appointments_tmp.txt", "data/appointments.txt");
}

void FileHandler::updateBill(const Bill& b)
{
    ifstream fin("data/bills.txt");
    ofstream fout("data/bills_tmp.txt");
    char line[256];
    while (fin.getline(line, 256))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 1 && Validator::strToInt(fields[0]) == b.getBillId())
        {
            char buf[256];
            writeBillLine(buf, b);
            fout << buf << endl;
        }
        else
        {
            fout << line << endl;
        }
    }
    fin.close();
    fout.close();
    remove("data/bills.txt");
    rename("data/bills_tmp.txt", "data/bills.txt");
}

void FileHandler::deletePatient(int id)
{
    ifstream fin("data/patients.txt");
    ofstream fout("data/patients_tmp.txt");
    char line[512];
    while (fin.getline(line, 512))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 1 && Validator::strToInt(fields[0]) == id)
        {
            continue;
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
    remove("data/patients.txt");
    rename("data/patients_tmp.txt", "data/patients.txt");
}

void FileHandler::deleteDoctor(int id)
{
    ifstream fin("data/doctors.txt");
    ofstream fout("data/doctors_tmp.txt");
    char line[512];
    while (fin.getline(line, 512))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 1 && Validator::strToInt(fields[0]) == id)
        {
            continue;
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
    remove("data/doctors.txt");
    rename("data/doctors_tmp.txt", "data/doctors.txt");
}

void FileHandler::deleteAppointmentsByPatient(int patientId)
{
    ifstream fin("data/appointments.txt");
    ofstream fout("data/appointments_tmp.txt");
    char line[256];
    while (fin.getline(line, 256))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 2 && Validator::strToInt(fields[1]) == patientId)
        {
            continue;
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
    remove("data/appointments.txt");
    rename("data/appointments_tmp.txt", "data/appointments.txt");
}

void FileHandler::deleteBillsByPatient(int patientId)
{
    ifstream fin("data/bills.txt");
    ofstream fout("data/bills_tmp.txt");
    char line[256];
    while (fin.getline(line, 256))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][256];
        int cnt = 0;
        parseLine(line, fields, cnt);
        if (cnt >= 2 && Validator::strToInt(fields[1]) == patientId)
        {
            continue;
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
    remove("data/bills.txt");
    rename("data/bills_tmp.txt", "data/bills.txt");
}

void FileHandler::deletePrescriptionsByPatient(int patientId)
{
    ifstream fin("data/prescriptions.txt");
    ofstream fout("data/prescriptions_tmp.txt");
    char line[1024];
    while (fin.getline(line, 1024))
    {
        if (line[0] == '\0')
        {
            continue;
        }
        char fields[10][512];
        int fi = 0;
        int cnt = 0;
        int ci = 0;
        while (line[fi] != '\0' && cnt < 3)
        {
            if (line[fi] == ',')
            {
                fields[cnt][ci] = '\0';
                cnt++;
                ci = 0;
            }
            else
            {
                fields[cnt][ci++] = line[fi];
            }
            fi++;
        }
        fields[cnt][ci] = '\0';
        if (cnt >= 2 && Validator::strToInt(fields[2]) == patientId)
        {
            continue;
        }
        fout << line << endl;
    }
    fin.close();
    fout.close();
    remove("data/prescriptions.txt");
    rename("data/prescriptions_tmp.txt", "data/prescriptions.txt");
}

void FileHandler::archivePatient(int patientId, Storage<Patient>& patients, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions)
{
    ofstream fout("data/discharged.txt", ios::app);
    Patient* p = patients.findById(patientId);
    if (p != nullptr)
    {
        char buf[512];
        writePatientLine(buf, *p);
        fout << buf << endl;
    }
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.getAll()[i].getPatientId() == patientId)
        {
            char buf[256];
            writeAppointmentLine(buf, appointments.getAll()[i]);
            fout << buf << endl;
        }
    }
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.getAll()[i].getPatientId() == patientId)
        {
            char buf[256];
            writeBillLine(buf, bills.getAll()[i]);
            fout << buf << endl;
        }
    }
    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.getAll()[i].getPatientId() == patientId)
        {
            char buf[1024];
            writePrescriptionLine(buf, prescriptions.getAll()[i]);
            fout << buf << endl;
        }
    }
    fout.close();
}

void FileHandler::logSecurityEvent(const char* role, const char* enteredId, const char* result)
{
    ofstream file("data/security_log.txt", ios::app);
    char dateBuf[20];
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char tmp[64];
    tmp[0] = '\0';
    char dayBuf[3], monBuf[3], yearBuf[5], hourBuf[3], minBuf[3], secBuf[3];
    Validator::intToStr(t->tm_mday, dayBuf);
    Validator::intToStr(t->tm_mon + 1, monBuf);
    Validator::intToStr(t->tm_year + 1900, yearBuf);
    Validator::intToStr(t->tm_hour, hourBuf);
    Validator::intToStr(t->tm_min, minBuf);
    Validator::intToStr(t->tm_sec, secBuf);
    dateBuf[0] = '\0';
    if (t->tm_mday < 10) { Validator::strConcat(dateBuf, "0"); }
    Validator::strConcat(dateBuf, dayBuf);
    Validator::strConcat(dateBuf, "-");
    if (t->tm_mon + 1 < 10) { Validator::strConcat(dateBuf, "0"); }
    Validator::strConcat(dateBuf, monBuf);
    Validator::strConcat(dateBuf, "-");
    Validator::strConcat(dateBuf, yearBuf);
    Validator::strConcat(dateBuf, " ");
    if (t->tm_hour < 10) { Validator::strConcat(dateBuf, "0"); }
    Validator::strConcat(dateBuf, hourBuf);
    Validator::strConcat(dateBuf, ":");
    if (t->tm_min < 10) { Validator::strConcat(dateBuf, "0"); }
    Validator::strConcat(dateBuf, minBuf);
    Validator::strConcat(dateBuf, ":");
    if (t->tm_sec < 10) { Validator::strConcat(dateBuf, "0"); }
    Validator::strConcat(dateBuf, secBuf);
    file << dateBuf << "," << role << "," << enteredId << "," << result << endl;
    file.close();
}