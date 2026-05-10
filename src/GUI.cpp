#include "../include/GUI.h"
#include "../include/FileHandler.h"
#include "../include/Validator.h"
#include "../include/Exceptions.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <ctime>
using namespace std;

GUI::GUI(Storage<Patient>& p, Storage<Doctor>& d, Storage<Appointment>& a, Storage<Bill>& b, Storage<Prescription>& pr, Admin& adm)
{
    window.create(sf::VideoMode(sf::Vector2u(1280, 720)), "MediCore Hospital Management System");
    window.setFramerateLimit(60);
    if (!font.openFromFile("data/arial.ttf"))
    {
        window.close();
        return;
    }
    if (bgTexture.loadFromFile("data/background.jpg")) {
        bgSprite.emplace(bgTexture);
    }
    
    patients = &p;
    doctors = &d;
    appointments = &a;
    bills = &b;
    prescriptions = &pr;
    adminPtr = &adm;
    currentScreen = Screen::MainLogin;
    previousScreen = Screen::MainLogin;
    messageText[0] = '\0';
    messageTitle[0] = '\0';
    loggedPatient = nullptr;
    loggedDoctor = nullptr;
    adminLogged = false;
    loginAttempts = 0;
    listLineCount = 0;
    listScrollY = 0.f;
    bookSelectedDoctorId = -1;
    bookSelectedDoctorFee = 0.f;
    bookAvailableSlotCount = 0;
    selectedAppointmentId = -1;
    selectedBillId = -1;
    inputSingleBuffer[0] = '\0';
    bookSpecInput[0] = '\0';
    bookDateInput[0] = '\0';
    bookSlotInput[0] = '\0';
}

void GUI::drawBackground()
{
    if (bgSprite)
    {
        sf::Vector2u texSize = bgTexture.getSize();
        bgSprite->setScale(sf::Vector2f(1280.f / texSize.x, 720.f / texSize.y));
        window.draw(*bgSprite);
    }
    else
    {
        sf::RectangleShape bg;
        bg.setSize(sf::Vector2f(1280, 720));
        bg.setFillColor(sf::Color(245, 246, 250));
        window.draw(bg);
    }
}

void GUI::drawTopBar(const char* title)
{
    sf::RectangleShape bar;
    bar.setSize(sf::Vector2f(1280, 60));
    bar.setFillColor(sf::Color(41, 128, 185));
    window.draw(bar);

    sf::Text t(font, title, 24);
    t.setFillColor(sf::Color::White);
    t.setPosition(sf::Vector2f(20, 15));
    window.draw(t);

    sf::Text sub(font, "MediCore Hospital Management System", 13);
    sub.setFillColor(sf::Color(174, 214, 241));
    sub.setPosition(sf::Vector2f(900, 22));
    window.draw(sub);
}

void GUI::showMessage(const char* title, const char* msg, Screen returnTo)
{
    Validator::strCopy(messageTitle, title);
    Validator::strCopy(messageText, msg);
    previousScreen = returnTo;
    currentScreen = Screen::MessageScreen;
}

void GUI::clearInputBoxes()
{
    for (int i = 0; i < 6; i++) { inputBoxes[i].clear(); }
}

int GUI::compareDates(const char* d1, const char* d2)
{
    int day1 = (d1[0]-'0')*10+(d1[1]-'0'), mon1 = (d1[3]-'0')*10+(d1[4]-'0');
    int yr1  = (d1[6]-'0')*1000+(d1[7]-'0')*100+(d1[8]-'0')*10+(d1[9]-'0');
    int day2 = (d2[0]-'0')*10+(d2[1]-'0'), mon2 = (d2[3]-'0')*10+(d2[4]-'0');
    int yr2  = (d2[6]-'0')*1000+(d2[7]-'0')*100+(d2[8]-'0')*10+(d2[9]-'0');
    if (yr1 != yr2) { return yr1 - yr2; }
    if (mon1 != mon2) { return mon1 - mon2; }
    return day1 - day2;
}

int GUI::dateDiffFromToday(const char* date)
{
    char today[11];
    FileHandler::getTodayDate(today);
    int day1=(today[0]-'0')*10+(today[1]-'0'), mon1=(today[3]-'0')*10+(today[4]-'0');
    int yr1=(today[6]-'0')*1000+(today[7]-'0')*100+(today[8]-'0')*10+(today[9]-'0');
    int day2=(date[0]-'0')*10+(date[1]-'0'), mon2=(date[3]-'0')*10+(date[4]-'0');
    int yr2=(date[6]-'0')*1000+(date[7]-'0')*100+(date[8]-'0')*10+(date[9]-'0');
    struct tm t1={}; t1.tm_mday=day1; t1.tm_mon=mon1-1; t1.tm_year=yr1-1900;
    struct tm t2={}; t2.tm_mday=day2; t2.tm_mon=mon2-1; t2.tm_year=yr2-1900;
    return (int)(difftime(mktime(&t1), mktime(&t2)) / 86400.0);
}

void GUI::sortAppointmentsByDateAsc(Appointment* arr, int n)
{
    for (int i=0;i<n-1;i++) for (int j=0;j<n-i-1;j++)
        if (compareDates(arr[j].getDate(),arr[j+1].getDate())>0) { Appointment tmp=arr[j]; arr[j]=arr[j+1]; arr[j+1]=tmp; }
}

void GUI::sortAppointmentsByDateDesc(Appointment* arr, int n)
{
    for (int i=0;i<n-1;i++) for (int j=0;j<n-i-1;j++)
        if (compareDates(arr[j].getDate(),arr[j+1].getDate())<0) { Appointment tmp=arr[j]; arr[j]=arr[j+1]; arr[j+1]=tmp; }
}

void GUI::sortPrescriptionsByDateDesc(Prescription* arr, int n)
{
    for (int i=0;i<n-1;i++) for (int j=0;j<n-i-1;j++)
        if (compareDates(arr[j].getDate(),arr[j+1].getDate())<0) { Prescription tmp=arr[j]; arr[j]=arr[j+1]; arr[j+1]=tmp; }
}

void GUI::drawMessageScreen()
{
    drawBackground();
    drawTopBar(messageTitle);

    sf::Text msg(font, messageText, 30);
    msg.setFillColor(sf::Color::White);
    msg.setStyle(sf::Text::Bold);
    sf::FloatRect tb = msg.getLocalBounds();
    msg.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f));
    msg.setPosition(sf::Vector2f(640, 320));
    window.draw(msg);

    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    buttons[0].setup(540, 420, 200, 50, font, "OK");
    buttons[0].updateHover(mp);
    buttons[0].draw(window);
}

void GUI::drawMainLogin()
{
    drawBackground();
    drawTopBar("Welcome to MediCore");
   
    sf::Text heading(font, "Login As", 26);
    heading.setFillColor(sf::Color::White);
    heading.setPosition(sf::Vector2f(575, 150));
    window.draw(heading);

    sf::Text sub(font, "Select your role to continue", 14);
    sub.setFillColor(sf::Color(200, 200, 200));
    sub.setPosition(sf::Vector2f(530, 188));
    window.draw(sub);

    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    buttons[0].setup(440, 230, 400, 60, font, "Patient", 20);
    buttons[1].setup(440, 310, 400, 60, font, "Doctor", 20);
    buttons[2].setup(440, 390, 400, 60, font, "Admin", 20);
    buttons[3].setup(440, 470, 400, 60, font, "Exit", 20);
    buttons[3].rect.setFillColor(sf::Color(231, 76, 60));
    buttons[3].rect.setOutlineColor(sf::Color(192, 57, 43));
    if (buttons[3].isHovered) { buttons[3].rect.setFillColor(sf::Color(192, 57, 43)); }
    for (int i = 0; i < 4; i++) { buttons[i].updateHover(mp); buttons[i].draw(window); }
}

void GUI::drawRoleLogin(const char* role)
{
    drawBackground();
    char titleBuf[64]; titleBuf[0] = '\0';
    Validator::strConcat(titleBuf, role);
    Validator::strConcat(titleBuf, " Login");
    drawTopBar(titleBuf);

    sf::Text heading(font, titleBuf, 30);
    heading.setFillColor(sf::Color(41, 128, 185));
    heading.setPosition(sf::Vector2f(560, 150));
    window.draw(heading);

    sf::Text idLabel(font, "ID:", 21);
    idLabel.setFillColor(sf::Color::Cyan);
    idLabel.setPosition(sf::Vector2f(420, 225));
    idLabel.setStyle(sf::Text::Bold);
    window.draw(idLabel);

    inputBoxes[0].draw(window);

    sf::Text passLabel(font, "Password:", 21);
    passLabel.setFillColor(sf::Color::Cyan);
    passLabel.setPosition(sf::Vector2f(420, 310));
    passLabel.setStyle(sf::Text::Bold);
    window.draw(passLabel);

    inputBoxes[1].draw(window);

    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    buttons[0].setup(420, 400, 200, 50, font, "Login");
    buttons[1].setup(640, 400, 200, 50, font, "Back");
    buttons[1].rect.setFillColor(sf::Color(127, 140, 141));
    buttons[1].rect.setOutlineColor(sf::Color(100, 110, 115));
    for (int i = 0; i < 2; i++) { buttons[i].updateHover(mp); buttons[i].draw(window); }

    if (loginAttempts > 0)
    {
        char attemptMsg[64]; attemptMsg[0] = '\0';
        Validator::strConcat(attemptMsg, "Failed attempts: ");
        char numBuf[4]; Validator::intToStr(loginAttempts, numBuf);
        Validator::strConcat(attemptMsg, numBuf);
        Validator::strConcat(attemptMsg, " / 3");
        sf::Text attText(font, attemptMsg, 14);
        attText.setFillColor(sf::Color(231, 76, 60));
        attText.setPosition(sf::Vector2f(420, 465));
        window.draw(attText);
    }
}

void GUI::drawListScreen(const char* title, const char* backLabel)
{
    drawBackground();
    drawTopBar(title);

    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(1220, 580));
    panel.setPosition(sf::Vector2f(30, 75));
    panel.setFillColor(sf::Color(0, 0, 0, 0));
    panel.setOutlineThickness(1);
    panel.setOutlineColor(sf::Color(189, 195, 199));
    window.draw(panel);

    float y = 85;
    for (int i = 0; i < listLineCount; i++)
    {
        sf::RectangleShape row;
        row.setSize(sf::Vector2f(1218, 38));
        row.setPosition(sf::Vector2f(31, y + listScrollY));
        row.setFillColor(i % 2 == 0 ? sf::Color(0, 0, 0, 60) : sf::Color(0, 0, 0, 30));
        window.draw(row);

        sf::Text rowText(font, listLines[i], 14);
        rowText.setFillColor(sf::Color::White);
        rowText.setStyle(sf::Text::Bold);
        rowText.setPosition(sf::Vector2f(42, y + 10 + listScrollY));
        window.draw(rowText);
        y += 40;
    }

    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    buttons[0].setup(30, 670, 150, 40, font, backLabel, 16);
    buttons[0].updateHover(mp);
    buttons[0].draw(window);
}

void GUI::drawPatientMenu()
{
    drawBackground();
    char titleBuf[128]; titleBuf[0] = '\0';
    Validator::strConcat(titleBuf, "Patient: ");
    Validator::strConcat(titleBuf, loggedPatient->getName());
    char balBuf[32]; Validator::floatToStr(loggedPatient->getBalance(), balBuf);
    Validator::strConcat(titleBuf, "  |  Balance: PKR ");
    Validator::strConcat(titleBuf, balBuf);
    drawTopBar(titleBuf);

    sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const char* menuItems[] = { "Book Appointment","Cancel Appointment","View My Appointments","View My Medical Records","View My Bills","Pay Bill","Top Up Balance","Logout" };
    sf::Color btnColors[] = { sf::Color(39,174,96),sf::Color(231,76,60),sf::Color(52,152,219),sf::Color(52,152,219),sf::Color(52,152,219),sf::Color(243,156,18),sf::Color(39,174,96),sf::Color(127,140,141) };
    float btnW=270,btnH=80,startX=42,startY=250,gapX=40,gapY=30;
    for (int i=0;i<8;i++)
    {
        float x=startX+(i%4)*(btnW+gapX), y=startY+(i/4)*(btnH+gapY);
        buttons[i].setup(x,y,btnW,btnH,font,menuItems[i],17);
        buttons[i].rect.setFillColor(btnColors[i]);
        buttons[i].updateHover(mp); buttons[i].draw(window);
    }
}

void GUI::drawDoctorMenu()
{
    drawBackground();
    char titleBuf[128]; titleBuf[0]='\0';
    Validator::strConcat(titleBuf,"Dr. "); Validator::strConcat(titleBuf,loggedDoctor->getName());
    Validator::strConcat(titleBuf,"  |  "); Validator::strConcat(titleBuf,loggedDoctor->getSpecialization());
    drawTopBar(titleBuf);

    sf::Vector2f mp=window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const char* menuItems[]={"View Today's Appointments","Mark Appointment Complete","Mark Appointment No-Show","Write Prescription","View Patient Medical History","Logout"};
    sf::Color btnColors[]={sf::Color(52,152,219),sf::Color(39,174,96),sf::Color(231,76,60),sf::Color(155,89,182),sf::Color(52,152,219),sf::Color(127,140,141)};
    float btnW=350,btnH=90,startX=50,startY=255,gapX=60,gapY=35;
    for (int i=0;i<6;i++)
    {
        float x=startX+(i%3)*(btnW+gapX), y=startY+(i/3)*(btnH+gapY);
        buttons[i].setup(x,y,btnW,btnH,font,menuItems[i],17);
        buttons[i].rect.setFillColor(btnColors[i]);
        buttons[i].updateHover(mp); buttons[i].draw(window);
    }
}

void GUI::drawAdminMenu()
{
    drawBackground();
    drawTopBar("Admin Panel - MediCore");
    sf::Vector2f mp=window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const char* menuItems[]={"Add Doctor","Remove Doctor","Add Patient","Remove Patient","View All Patients","View All Doctors","View All Appointments","View Unpaid Bills","Discharge Patient","View Security Log","Daily Report","Logout"};
    sf::Color btnColors[]={sf::Color(39,174,96),sf::Color(231,76,60),sf::Color(39,174,96),sf::Color(231,76,60),sf::Color(52,152,219),sf::Color(52,152,219),sf::Color(52,152,219),sf::Color(243,156,18),sf::Color(155,89,182),sf::Color(52,152,219),sf::Color(41,128,185),sf::Color(127,140,141)};
    float btnW=255,btnH=70,startX=80,startY=255,gapX=35,gapY=30;
    for (int i=0;i<12;i++)
    {
        float x=startX+(i%4)*(btnW+gapX), y=startY+(i/4)*(btnH+gapY);
        buttons[i].setup(x,y,btnW,btnH,font,menuItems[i],15);
        buttons[i].rect.setFillColor(btnColors[i]);
        buttons[i].updateHover(mp); buttons[i].draw(window);
    }
}

void GUI::doViewAppointments()
{
    listLineCount = 0;
    Appointment tmp[100]; int cnt=0;
    for (int i=0;i<appointments->size();i++)
        if (appointments->getAll()[i].getPatientId()==loggedPatient->getId()) { tmp[cnt++]=appointments->getAll()[i]; }
    if (cnt==0) { showMessage("My Appointments","No appointments found.",Screen::PatientMenu); return; }
    sortAppointmentsByDateAsc(tmp,cnt);
    for (int i=0;i<cnt;i++)
    {
        Doctor* doc=doctors->findById(tmp[i].getDoctorId());
        char line[256]; line[0]='\0'; char idBuf[16]; Validator::intToStr(tmp[i].getAppointmentId(),idBuf);
        Validator::strConcat(line,"ID: "); Validator::strConcat(line,idBuf);
        Validator::strConcat(line,"  |  Doctor: "); Validator::strConcat(line,doc?doc->getName():"Unknown");
        Validator::strConcat(line,"  |  Spec: "); Validator::strConcat(line,doc?doc->getSpecialization():"Unknown");
        Validator::strConcat(line,"  |  Date: "); Validator::strConcat(line,tmp[i].getDate());
        Validator::strConcat(line,"  |  Time: "); Validator::strConcat(line,tmp[i].getTimeSlot());
        Validator::strConcat(line,"  |  Status: "); Validator::strConcat(line,tmp[i].getStatus());
        Validator::strCopy(listLines[listLineCount++],line);
    }
    currentScreen=Screen::ViewAppointments;
}

void GUI::doViewMedicalRecords()
{
    listLineCount=0;
    Prescription tmp[100]; int cnt=0;
    for (int i=0;i<prescriptions->size();i++)
        if (prescriptions->getAll()[i].getPatientId()==loggedPatient->getId()) { tmp[cnt++]=prescriptions->getAll()[i]; }
    if (cnt==0) { showMessage("Medical Records","No medical records found.",Screen::PatientMenu); return; }
    sortPrescriptionsByDateDesc(tmp,cnt);
    for (int i=0;i<cnt;i++)
    {
        Doctor* doc=doctors->findById(tmp[i].getDoctorId());
        char line[256]; line[0]='\0';
        Validator::strConcat(line,"Date: "); Validator::strConcat(line,tmp[i].getDate());
        Validator::strConcat(line,"  |  Doctor: "); Validator::strConcat(line,doc?doc->getName():"Unknown");
        Validator::strConcat(line,"  |  Medicines: ");
        int mLen=Validator::strLen(tmp[i].getMedicines()); char medShort[60]; int cL=mLen>55?55:mLen;
        for (int j=0;j<cL;j++){medShort[j]=tmp[i].getMedicines()[j];} medShort[cL]='\0';
        if (mLen>55){Validator::strConcat(medShort,"...");}
        Validator::strConcat(line,medShort);
        Validator::strConcat(line,"  |  Notes: ");
        int nLen=Validator::strLen(tmp[i].getNotes()); char notShort[40]; int cN=nLen>35?35:nLen;
        for (int j=0;j<cN;j++){notShort[j]=tmp[i].getNotes()[j];} notShort[cN]='\0';
        if (nLen>35){Validator::strConcat(notShort,"...");}
        Validator::strConcat(line,notShort);
        Validator::strCopy(listLines[listLineCount++],line);
    }
    currentScreen=Screen::ViewMedicalRecords;
}

void GUI::doViewBills()
{
    listLineCount=0; float totalUnpaid=0.f;
    for (int i=0;i<bills->size();i++)
    {
        if (bills->getAll()[i].getPatientId()==loggedPatient->getId())
        {
            char line[256]; line[0]='\0'; char idBuf[16],amtBuf[32];
            Validator::intToStr(bills->getAll()[i].getBillId(),idBuf);
            Validator::floatToStr(bills->getAll()[i].getAmount(),amtBuf);
            Validator::strConcat(line,"Bill ID: "); Validator::strConcat(line,idBuf);
            Validator::strConcat(line,"  |  Amount: PKR "); Validator::strConcat(line,amtBuf);
            Validator::strConcat(line,"  |  Status: "); Validator::strConcat(line,bills->getAll()[i].getStatus());
            Validator::strConcat(line,"  |  Date: "); Validator::strConcat(line,bills->getAll()[i].getDate());
            Validator::strCopy(listLines[listLineCount++],line);
            if (Validator::strComp(bills->getAll()[i].getStatus(),"unpaid")==0) totalUnpaid+=bills->getAll()[i].getAmount();
        }
    }
    if (listLineCount==0){showMessage("My Bills","No bills found.",Screen::PatientMenu);return;}
    char totLine[64]; totLine[0]='\0'; char totBuf[32]; Validator::floatToStr(totalUnpaid,totBuf);
    Validator::strConcat(totLine,"Total Outstanding: PKR "); Validator::strConcat(totLine,totBuf);
    Validator::strCopy(listLines[listLineCount++],totLine);
    currentScreen=Screen::ViewBills;
}

void GUI::doDoctorToday()
{
    listLineCount=0; char today[11]; FileHandler::getTodayDate(today);
    Appointment tmp[100]; int cnt=0;
    for (int i=0;i<appointments->size();i++)
    {
        Appointment& a=appointments->getAll()[i];
        if (a.getDoctorId()==loggedDoctor->getId()&&Validator::strComp(a.getDate(),today)==0) tmp[cnt++]=a;
    }
    if (cnt==0){showMessage("Today's Appointments","No appointments scheduled for today.",Screen::DoctorMenu);return;}
    sortAppointmentsByDateAsc(tmp,cnt);
    for (int i=0;i<cnt;i++)
    {
        Patient* p=patients->findById(tmp[i].getPatientId());
        char line[256]; line[0]='\0'; char idBuf[16]; Validator::intToStr(tmp[i].getAppointmentId(),idBuf);
        Validator::strConcat(line,"ID: "); Validator::strConcat(line,idBuf);
        Validator::strConcat(line,"  |  Patient: "); Validator::strConcat(line,p?p->getName():"Unknown");
        Validator::strConcat(line,"  |  Time: "); Validator::strConcat(line,tmp[i].getTimeSlot());
        Validator::strConcat(line,"  |  Status: "); Validator::strConcat(line,tmp[i].getStatus());
        Validator::strCopy(listLines[listLineCount++],line);
    }
    currentScreen=Screen::DoctorTodayAppts;
}

void GUI::doAdminViewPatients()
{
    listLineCount=0;
    for (int i=0;i<patients->size();i++)
    {
        int unpaid=0;
        for (int j=0;j<bills->size();j++)
            if (bills->getAll()[j].getPatientId()==patients->getAll()[i].getId()&&Validator::strComp(bills->getAll()[j].getStatus(),"unpaid")==0) unpaid++;
        char line[256]; line[0]='\0'; char idBuf[16],ageBuf[8],balBuf[32],unpaidBuf[8];
        Validator::intToStr(patients->getAll()[i].getId(),idBuf); Validator::intToStr(patients->getAll()[i].getAge(),ageBuf);
        Validator::floatToStr(patients->getAll()[i].getBalance(),balBuf); Validator::intToStr(unpaid,unpaidBuf);
        Validator::strConcat(line,"ID: "); Validator::strConcat(line,idBuf);
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,patients->getAll()[i].getName());
        Validator::strConcat(line,"  |  Age: "); Validator::strConcat(line,ageBuf);
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,patients->getAll()[i].getGender());
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,patients->getAll()[i].getContact());
        Validator::strConcat(line,"  |  PKR "); Validator::strConcat(line,balBuf);
        Validator::strConcat(line,"  |  Unpaid Bills: "); Validator::strConcat(line,unpaidBuf);
        Validator::strCopy(listLines[listLineCount++],line);
    }
    if (listLineCount==0){showMessage("All Patients","No patients found.",Screen::AdminMenu);return;}
    currentScreen=Screen::AdminViewPatients;
}

void GUI::doAdminViewDoctors()
{
    listLineCount=0;
    for (int i=0;i<doctors->size();i++)
    {
        char line[256]; line[0]='\0'; char idBuf[16],feeBuf[32];
        Validator::intToStr(doctors->getAll()[i].getId(),idBuf); Validator::floatToStr(doctors->getAll()[i].getFee(),feeBuf);
        Validator::strConcat(line,"ID: "); Validator::strConcat(line,idBuf);
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,doctors->getAll()[i].getName());
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,doctors->getAll()[i].getSpecialization());
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,doctors->getAll()[i].getContact());
        Validator::strConcat(line,"  |  PKR "); Validator::strConcat(line,feeBuf);
        Validator::strCopy(listLines[listLineCount++],line);
    }
    if (listLineCount==0){showMessage("All Doctors","No doctors found.",Screen::AdminMenu);return;}
    currentScreen=Screen::AdminViewDoctors;
}

void GUI::doAdminViewAppointments()
{
    listLineCount=0; Appointment tmp[100]; int cnt=0;
    for (int i=0;i<appointments->size();i++) tmp[cnt++]=appointments->getAll()[i];
    sortAppointmentsByDateDesc(tmp,cnt);
    for (int i=0;i<cnt;i++)
    {
        Patient* p=patients->findById(tmp[i].getPatientId());
        Doctor* d=doctors->findById(tmp[i].getDoctorId());
        char line[256]; line[0]='\0'; char idBuf[16]; Validator::intToStr(tmp[i].getAppointmentId(),idBuf);
        Validator::strConcat(line,"ID: "); Validator::strConcat(line,idBuf);
        Validator::strConcat(line,"  |  Patient: "); Validator::strConcat(line,p?p->getName():"Unknown");
        Validator::strConcat(line,"  |  Doctor: "); Validator::strConcat(line,d?d->getName():"Unknown");
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,tmp[i].getDate());
        Validator::strConcat(line,"  "); Validator::strConcat(line,tmp[i].getTimeSlot());
        Validator::strConcat(line,"  |  "); Validator::strConcat(line,tmp[i].getStatus());
        Validator::strCopy(listLines[listLineCount++],line);
    }
    if (listLineCount==0){showMessage("All Appointments","No appointments found.",Screen::AdminMenu);return;}
    currentScreen=Screen::AdminViewAppointments;
}

void GUI::doAdminViewUnpaidBills()
{
    listLineCount=0;
    for (int i=0;i<bills->size();i++)
    {
        if (Validator::strComp(bills->getAll()[i].getStatus(),"unpaid")==0)
        {
            Patient* p=patients->findById(bills->getAll()[i].getPatientId());
            char line[256]; line[0]='\0'; char idBuf[16],amtBuf[32];
            Validator::intToStr(bills->getAll()[i].getBillId(),idBuf); Validator::floatToStr(bills->getAll()[i].getAmount(),amtBuf);
            Validator::strConcat(line,"Bill ID: "); Validator::strConcat(line,idBuf);
            Validator::strConcat(line,"  |  Patient: "); Validator::strConcat(line,p?p->getName():"Unknown");
            Validator::strConcat(line,"  |  PKR "); Validator::strConcat(line,amtBuf);
            Validator::strConcat(line,"  |  Date: "); Validator::strConcat(line,bills->getAll()[i].getDate());
            if (dateDiffFromToday(bills->getAll()[i].getDate())>7) Validator::strConcat(line,"  [OVERDUE]");
            Validator::strCopy(listLines[listLineCount++],line);
        }
    }
    if (listLineCount==0){showMessage("Unpaid Bills","No unpaid bills.",Screen::AdminMenu);return;}
    currentScreen=Screen::AdminViewUnpaidBills;
}

void GUI::doAdminSecurityLog()
{
    listLineCount=0;
    ifstream logFile("data/security_log.txt");
    if (logFile.is_open())
    {
        char line[256];
        while (logFile.getline(line,256)&&listLineCount<100)
            if (line[0]!='\0') Validator::strCopy(listLines[listLineCount++],line);
        logFile.close();
    }
    if (listLineCount==0){showMessage("Security Log","No security events logged.",Screen::AdminMenu);return;}
    currentScreen=Screen::AdminSecurityLog;
}

void GUI::doAdminDailyReport()
{
    listLineCount=0; char today[11]; FileHandler::getTodayDate(today);
    int total=0,pending=0,completed=0,noshow=0,cancelled=0; float revenue=0.f;
    for (int i=0;i<appointments->size();i++)
    {
        if (Validator::strComp(appointments->getAll()[i].getDate(),today)==0)
        {
            total++; const char* st=appointments->getAll()[i].getStatus();
            if (Validator::strComp(st,"pending")==0) pending++;
            else if (Validator::strComp(st,"completed")==0) completed++;
            else if (Validator::strComp(st,"noshow")==0) noshow++;
            else if (Validator::strComp(st,"cancelled")==0) cancelled++;
        }
    }
    for (int i=0;i<bills->size();i++)
        if (Validator::strComp(bills->getAll()[i].getStatus(),"paid")==0&&Validator::strComp(bills->getAll()[i].getDate(),today)==0)
            revenue+=bills->getAll()[i].getAmount();

    char line[256];
    line[0]='\0'; Validator::strConcat(line,"Date: "); Validator::strConcat(line,today); Validator::strCopy(listLines[listLineCount++],line);
    line[0]='\0'; char tB[8],pB[8],cB[8],nB[8],canB[8];
    Validator::intToStr(total,tB); Validator::intToStr(pending,pB); Validator::intToStr(completed,cB); Validator::intToStr(noshow,nB); Validator::intToStr(cancelled,canB);
    Validator::strConcat(line,"Appointments - Total: "); Validator::strConcat(line,tB);
    Validator::strConcat(line,"  Pending: "); Validator::strConcat(line,pB);
    Validator::strConcat(line,"  Completed: "); Validator::strConcat(line,cB);
    Validator::strConcat(line,"  No-show: "); Validator::strConcat(line,nB);
    Validator::strConcat(line,"  Cancelled: "); Validator::strConcat(line,canB);
    Validator::strCopy(listLines[listLineCount++],line);
    line[0]='\0'; char revBuf[32]; Validator::floatToStr(revenue,revBuf);
    Validator::strConcat(line,"Revenue Collected Today: PKR "); Validator::strConcat(line,revBuf);
    Validator::strCopy(listLines[listLineCount++],line);
    Validator::strCopy(listLines[listLineCount++],"--- Patients with Outstanding Bills ---");
    for (int i=0;i<patients->size();i++)
    {
        float owed=0.f;
        for (int j=0;j<bills->size();j++)
            if (bills->getAll()[j].getPatientId()==patients->getAll()[i].getId()&&Validator::strComp(bills->getAll()[j].getStatus(),"unpaid")==0)
                owed+=bills->getAll()[j].getAmount();
        if (owed>0.f)
        {
            line[0]='\0'; char owedBuf[32]; Validator::floatToStr(owed,owedBuf);
            Validator::strConcat(line,patients->getAll()[i].getName());
            Validator::strConcat(line,"  |  Owed: PKR "); Validator::strConcat(line,owedBuf);
            Validator::strCopy(listLines[listLineCount++],line);
        }
    }
    Validator::strCopy(listLines[listLineCount++],"--- Doctor Summary for Today ---");
    for (int d=0;d<doctors->size();d++)
    {
        int dC=0,dP=0,dN=0;
        for (int i=0;i<appointments->size();i++)
            if (appointments->getAll()[i].getDoctorId()==doctors->getAll()[d].getId()&&Validator::strComp(appointments->getAll()[i].getDate(),today)==0)
            {
                const char* st=appointments->getAll()[i].getStatus();
                if (Validator::strComp(st,"completed")==0) dC++;
                else if (Validator::strComp(st,"pending")==0) dP++;
                else if (Validator::strComp(st,"noshow")==0) dN++;
            }
        line[0]='\0'; char cB2[8],pB2[8],nB2[8];
        Validator::intToStr(dC,cB2); Validator::intToStr(dP,pB2); Validator::intToStr(dN,nB2);
        Validator::strConcat(line,doctors->getAll()[d].getName());
        Validator::strConcat(line,"  |  Completed: "); Validator::strConcat(line,cB2);
        Validator::strConcat(line,"  Pending: "); Validator::strConcat(line,pB2);
        Validator::strConcat(line,"  No-show: "); Validator::strConcat(line,nB2);
        Validator::strCopy(listLines[listLineCount++],line);
    }
    currentScreen=Screen::AdminDailyReport;
}

// NOTE: sf::Event& param removed — event handling is done in the main run() loop
void GUI::handleMainLogin(sf::Event& /*event*/, sf::Vector2f mousePos, bool clicked)
{
    if (!clicked) return;
   if (buttons[0].contains(mousePos)) { 
    clearInputBoxes(); 
    loginAttempts = 0;
    inputBoxes[0].setup(420, 250, 440, 45, font, "Enter your ID");
    inputBoxes[1].setup(420, 335, 440, 45, font, "Enter your password", true);
    currentScreen = Screen::PatientLogin; 
}
else if (buttons[1].contains(mousePos)) { 
    clearInputBoxes(); 
    loginAttempts = 0;
    inputBoxes[0].setup(420, 250, 440, 45, font, "Enter your ID");
    inputBoxes[1].setup(420, 335, 440, 45, font, "Enter your password", true);
    currentScreen = Screen::DoctorLogin; 
}
else if (buttons[2].contains(mousePos)) { 
    clearInputBoxes(); 
    loginAttempts = 0;
    inputBoxes[0].setup(420, 250, 440, 45, font, "Enter your ID");
    inputBoxes[1].setup(420, 335, 440, 45, font, "Enter your password", true);
    currentScreen = Screen::AdminLogin; 
}
    else if (buttons[3].contains(mousePos)) { window.close(); }
}

void GUI::handleRoleLogin(sf::Event& /*event*/, sf::Vector2f mousePos, bool clicked, const char* role)
{
    // Input box event handling is done in the main run() loop
    if (!clicked) return;
    inputBoxes[0].updateActive(mousePos, true);
    inputBoxes[1].updateActive(mousePos, true);

    if (buttons[1].contains(mousePos)) { clearInputBoxes(); loginAttempts=0; currentScreen=Screen::MainLogin; return; }
    if (buttons[0].contains(mousePos))
    {
        char idBuf[16]; Validator::strCopy(idBuf,inputBoxes[0].buffer);
        char passBuf[51]; Validator::strCopy(passBuf,inputBoxes[1].buffer);
        int id=Validator::strToInt(idBuf);
        bool success=false;

        if (Validator::strComp(role,"Patient")==0)
        {
            Patient* p=patients->findById(id);
            if (p&&Validator::strComp(p->getPassword(),passBuf)==0) { loggedPatient=p; success=true; loginAttempts=0; clearInputBoxes(); currentScreen=Screen::PatientMenu; }
        }
        else if (Validator::strComp(role,"Doctor")==0)
        {
            Doctor* d=doctors->findById(id);
            if (d&&Validator::strComp(d->getPassword(),passBuf)==0) { loggedDoctor=d; success=true; loginAttempts=0; clearInputBoxes(); currentScreen=Screen::DoctorMenu; }
        }
        else if (Validator::strComp(role,"Admin")==0)
        {
            if (id==adminPtr->getId()&&Validator::strComp(adminPtr->getPassword(),passBuf)==0) { adminLogged=true; success=true; loginAttempts=0; clearInputBoxes(); currentScreen=Screen::AdminMenu; }
        }

        if (!success)
        {
            loginAttempts++; clearInputBoxes();
            if (loginAttempts>=3)
            {
                FileHandler::logSecurityEvent(role,idBuf,"FAILED");
                showMessage("Account Locked","Account locked. Contact admin.",Screen::MainLogin);
                loginAttempts=0;
            }
            else { showMessage("Login Failed","Invalid ID or password.",currentScreen); }
        }
    }
}

void GUI::handlePatientMenu(sf::Vector2f mousePos, bool clicked)
{
    if (!clicked) return;
    if (buttons[0].contains(mousePos))
    {
        clearInputBoxes();
        inputBoxes[0].setup(200,220,500,45,font,"Enter specialization (e.g. Cardiology)");
        inputBoxes[1].setup(200,340,200,45,font,"Doctor ID");
        inputBoxes[2].setup(200,450,200,45,font,"Date (DD-MM-YYYY)");
        inputBoxes[3].setup(200,560,200,45,font,"Time slot (e.g. 09:00)");
        currentScreen=Screen::BookAppointment;
    }
    else if (buttons[1].contains(mousePos))
    {
        listLineCount=0; bool hasPending=false;
        for (int i=0;i<appointments->size();i++)
        {
            Appointment& a=appointments->getAll()[i];
            if (a.getPatientId()==loggedPatient->getId()&&Validator::strComp(a.getStatus(),"pending")==0)
            {
                Doctor* doc=doctors->findById(a.getDoctorId());
                char line[256]; line[0]='\0'; char idBuf[16]; Validator::intToStr(a.getAppointmentId(),idBuf);
                Validator::strConcat(line,"ID: "); Validator::strConcat(line,idBuf);
                Validator::strConcat(line,"  |  Doctor: "); Validator::strConcat(line,doc?doc->getName():"Unknown");
                Validator::strConcat(line,"  |  Date: "); Validator::strConcat(line,a.getDate());
                Validator::strConcat(line,"  |  Time: "); Validator::strConcat(line,a.getTimeSlot());
                Validator::strCopy(listLines[listLineCount++],line); hasPending=true;
            }
        }
        if (!hasPending){showMessage("Cancel Appointment","You have no pending appointments.",Screen::PatientMenu);return;}
        clearInputBoxes(); inputBoxes[0].setup(200,500,300,45,font,"Enter Appointment ID to cancel");
        currentScreen=Screen::CancelAppointment;
    }
    else if (buttons[2].contains(mousePos)) { doViewAppointments(); }
    else if (buttons[3].contains(mousePos)) { doViewMedicalRecords(); }
    else if (buttons[4].contains(mousePos)) { doViewBills(); }
    else if (buttons[5].contains(mousePos))
    {
        listLineCount=0; bool hasUnpaid=false;
        for (int i=0;i<bills->size();i++)
        {
            if (bills->getAll()[i].getPatientId()==loggedPatient->getId()&&Validator::strComp(bills->getAll()[i].getStatus(),"unpaid")==0)
            {
                char line[256]; line[0]='\0'; char idBuf[16],amtBuf[32];
                Validator::intToStr(bills->getAll()[i].getBillId(),idBuf); Validator::floatToStr(bills->getAll()[i].getAmount(),amtBuf);
                Validator::strConcat(line,"Bill ID: "); Validator::strConcat(line,idBuf);
                Validator::strConcat(line,"  |  PKR "); Validator::strConcat(line,amtBuf);
                Validator::strConcat(line,"  |  Date: "); Validator::strConcat(line,bills->getAll()[i].getDate());
                Validator::strCopy(listLines[listLineCount++],line); hasUnpaid=true;
            }
        }
        if (!hasUnpaid){showMessage("Pay Bill","No unpaid bills.",Screen::PatientMenu);return;}
        clearInputBoxes(); inputBoxes[0].setup(200,500,300,45,font,"Enter Bill ID to pay");
        currentScreen=Screen::PayBill;
    }
    else if (buttons[6].contains(mousePos))
    {
        clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter amount (PKR)");
        currentScreen=Screen::TopUpBalance;
    }
    else if (buttons[7].contains(mousePos)) { loggedPatient=nullptr; currentScreen=Screen::MainLogin; }
}

void GUI::handleDoctorMenu(sf::Vector2f mousePos, bool clicked)
{
    if (!clicked) return;
    if (buttons[0].contains(mousePos)) { doDoctorToday(); }
    else if (buttons[1].contains(mousePos))
    {
        clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter Appointment ID");
        currentScreen=Screen::DoctorMarkComplete;
    }
    else if (buttons[2].contains(mousePos))
    {
        clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter Appointment ID");
        currentScreen=Screen::DoctorMarkNoShow;
    }
    else if (buttons[3].contains(mousePos))
    {
        clearInputBoxes();
        inputBoxes[0].setup(200,200,400,45,font,"Appointment ID");
        inputBoxes[1].setup(200,310,800,45,font,"Medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg)");
        inputBoxes[2].setup(200,420,800,45,font,"Notes (max 300 chars)");
        currentScreen=Screen::DoctorWritePrescription;
    }
    else if (buttons[4].contains(mousePos))
    {
        clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter Patient ID");
        currentScreen=Screen::DoctorViewHistory;
    }
    else if (buttons[5].contains(mousePos)) { loggedDoctor=nullptr; currentScreen=Screen::MainLogin; }
}

void GUI::handleAdminMenu(sf::Vector2f mousePos, bool clicked)
{
    if (!clicked) return;
    if (buttons[0].contains(mousePos))
    {
        clearInputBoxes();
        inputBoxes[0].setup(100,130,400,40,font,"Name (max 50 chars)");
        inputBoxes[1].setup(100,210,400,40,font,"Specialization");
        inputBoxes[2].setup(100,290,400,40,font,"Contact (11 digits)");
        inputBoxes[3].setup(600,130,400,40,font,"Password (min 6 chars)",true);
        inputBoxes[4].setup(600,210,400,40,font,"Consultation Fee");
        currentScreen=Screen::AdminAddDoctor;
    }
    else if (buttons[1].contains(mousePos))
    { clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter Doctor ID to remove"); currentScreen=Screen::AdminRemoveDoctor; }
    else if (buttons[2].contains(mousePos))
    {
        clearInputBoxes();
        inputBoxes[0].setup(100,130,400,40,font,"Name (max 50 chars)");
        inputBoxes[1].setup(100,210,200,40,font,"Age");
        inputBoxes[2].setup(100,290,200,40,font,"Gender (M/F)");
        inputBoxes[3].setup(600,130,400,40,font,"Contact (11 digits)");
        inputBoxes[4].setup(600,210,400,40,font,"Password (min 6 chars)",true);
        inputBoxes[5].setup(600,290,400,40,font,"Initial Balance");
        currentScreen=Screen::AdminAddPatient;
    }
    else if (buttons[3].contains(mousePos))
    { clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter Patient ID to remove"); currentScreen=Screen::AdminRemovePatient; }
    else if (buttons[4].contains(mousePos)) { doAdminViewPatients(); }
    else if (buttons[5].contains(mousePos)) { doAdminViewDoctors(); }
    else if (buttons[6].contains(mousePos)) { doAdminViewAppointments(); }
    else if (buttons[7].contains(mousePos)) { doAdminViewUnpaidBills(); }
    else if (buttons[8].contains(mousePos))
    { clearInputBoxes(); inputBoxes[0].setup(440,350,400,45,font,"Enter Patient ID to discharge"); currentScreen=Screen::AdminDischarge; }
    else if (buttons[9].contains(mousePos)) { doAdminSecurityLog(); }
    else if (buttons[10].contains(mousePos)) { doAdminDailyReport(); }
    else if (buttons[11].contains(mousePos)) { adminLogged=false; currentScreen=Screen::MainLogin; }
}

void GUI::run()
{
    while (window.isOpen())
    {
        sf::Vector2f mousePos=window.mapPixelToCoords(sf::Mouse::getPosition(window));
        bool clicked=false;
        sf::Event dummyEvent(sf::Event::MouseMoved{});

        while (auto event=window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) { window.close(); }
            if (event->is<sf::Event::MouseButtonReleased>())
            {
                auto* mb=event->getIf<sf::Event::MouseButtonReleased>();
                if (mb&&mb->button==sf::Mouse::Button::Left) clicked=true;
            }
            if (event->is<sf::Event::MouseWheelScrolled>())
            {
                auto* mw=event->getIf<sf::Event::MouseWheelScrolled>();
                if (mw) { listScrollY-=mw->delta*30.f; if (listScrollY>0.f) listScrollY=0.f; }
            }
            if (currentScreen==Screen::PatientLogin||currentScreen==Screen::DoctorLogin||currentScreen==Screen::AdminLogin)
            { inputBoxes[0].handleEvent(*event); inputBoxes[1].handleEvent(*event); }
            else if (currentScreen==Screen::BookAppointment)
            { for (int i=0;i<4;i++) inputBoxes[i].handleEvent(*event); }
            else if (currentScreen==Screen::CancelAppointment||currentScreen==Screen::PayBill||currentScreen==Screen::TopUpBalance||
                     currentScreen==Screen::DoctorMarkComplete||currentScreen==Screen::DoctorMarkNoShow||
                     currentScreen==Screen::DoctorViewHistory||currentScreen==Screen::AdminRemoveDoctor||
                     currentScreen==Screen::AdminRemovePatient||currentScreen==Screen::AdminDischarge)
            { inputBoxes[0].handleEvent(*event); }
            else if (currentScreen==Screen::DoctorWritePrescription)
            { for (int i=0;i<3;i++) inputBoxes[i].handleEvent(*event); }
            else if (currentScreen==Screen::AdminAddDoctor)
            { for (int i=0;i<5;i++) inputBoxes[i].handleEvent(*event); }
            else if (currentScreen==Screen::AdminAddPatient)
            { for (int i=0;i<6;i++) inputBoxes[i].handleEvent(*event); }
        }

        if (clicked) { for (int i=0;i<6;i++) inputBoxes[i].updateActive(mousePos,true); }

        window.clear(sf::Color::Black);

        if (currentScreen==Screen::MainLogin)
        { drawMainLogin(); handleMainLogin(dummyEvent,mousePos,clicked); }
        else if (currentScreen==Screen::PatientLogin)
        { drawRoleLogin("Patient"); if (clicked) handleRoleLogin(dummyEvent,mousePos,clicked,"Patient"); }
        else if (currentScreen==Screen::DoctorLogin)
        { drawRoleLogin("Doctor"); if (clicked) handleRoleLogin(dummyEvent,mousePos,clicked,"Doctor"); }
        else if (currentScreen==Screen::AdminLogin)
        { drawRoleLogin("Admin"); if (clicked) handleRoleLogin(dummyEvent,mousePos,clicked,"Admin"); }
        else if (currentScreen==Screen::PatientMenu)
        { drawPatientMenu(); handlePatientMenu(mousePos,clicked); }
        else if (currentScreen==Screen::DoctorMenu)
        { drawDoctorMenu(); handleDoctorMenu(mousePos,clicked); }
        else if (currentScreen==Screen::AdminMenu)
        { drawAdminMenu(); handleAdminMenu(mousePos,clicked); }
        else if (currentScreen==Screen::MessageScreen)
        { drawMessageScreen(); if (clicked&&buttons[0].contains(mousePos)) currentScreen=previousScreen; }
        else if (currentScreen==Screen::ViewAppointments)
        { drawListScreen("My Appointments"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::PatientMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::ViewMedicalRecords)
        { drawListScreen("My Medical Records"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::PatientMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::ViewBills)
        { drawListScreen("My Bills"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::PatientMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::DoctorTodayAppts)
        { drawListScreen("Today's Appointments"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::DoctorMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::AdminViewPatients)
        { drawListScreen("All Patients"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::AdminMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::AdminViewDoctors)
        { drawListScreen("All Doctors"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::AdminMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::AdminViewAppointments)
        { drawListScreen("All Appointments"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::AdminMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::AdminViewUnpaidBills)
        { drawListScreen("Unpaid Bills"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::AdminMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::AdminSecurityLog)
        { drawListScreen("Security Log"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::AdminMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::AdminDailyReport)
        { drawListScreen("Daily Report"); if (clicked&&buttons[0].contains(mousePos)){currentScreen=Screen::AdminMenu;listScrollY=0.f;} }
        else if (currentScreen==Screen::BookAppointment)
        {
            drawBackground(); drawTopBar("Book Appointment");
            sf::Text t1(font,"",15); t1.setFillColor(sf::Color::White);
            t1.setString("Specialization:"); t1.setPosition(sf::Vector2f(200,195)); window.draw(t1);
            inputBoxes[0].draw(window);
            t1.setString("Doctor ID:"); t1.setPosition(sf::Vector2f(200,315)); window.draw(t1);
            inputBoxes[1].draw(window);
            t1.setString("Date (DD-MM-YYYY):"); t1.setPosition(sf::Vector2f(200,425)); window.draw(t1);
            inputBoxes[2].draw(window);
            t1.setString("Time Slot:"); t1.setPosition(sf::Vector2f(200,535)); window.draw(t1);
            inputBoxes[3].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(200,625,180,45,font,"Book");
            buttons[1].setup(420,625,180,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::PatientMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    char spec[51]; Validator::strCopy(spec,inputBoxes[0].buffer);
                    char docIdBuf[16]; Validator::strCopy(docIdBuf,inputBoxes[1].buffer);
                    char date[11]; Validator::strCopy(date,inputBoxes[2].buffer);
                    char slot[6]; Validator::strCopy(slot,inputBoxes[3].buffer);
                    if (!Validator::isPositiveInt(docIdBuf)){showMessage("Error","Invalid Doctor ID.",Screen::BookAppointment);}
                    else if (!Validator::isValidDate(date)){showMessage("Error","Invalid date. Use DD-MM-YYYY.",Screen::BookAppointment);}
                    else if (!Validator::isValidTimeSlot(slot)){showMessage("Error","Invalid time slot.",Screen::BookAppointment);}
                    else
                    {
                        int docId=Validator::strToInt(docIdBuf); Doctor* doc=doctors->findById(docId);
                        if (!doc){showMessage("Error","Doctor not found.",Screen::BookAppointment);}
                        else
                        {
                            bool taken=false;
                            for (int i=0;i<appointments->size();i++)
                            {
                                Appointment& a=appointments->getAll()[i];
                                if (a.getDoctorId()==docId&&Validator::strComp(a.getDate(),date)==0&&Validator::strComp(a.getTimeSlot(),slot)==0&&Validator::strComp(a.getStatus(),"cancelled")!=0)
                                { taken=true; break; }
                            }
                            if (taken){showMessage("Slot Unavailable","That slot is already taken.",Screen::BookAppointment);}
                            else if (loggedPatient->getBalance()<doc->getFee()){showMessage("Insufficient Funds","Balance is less than doctor's fee.",Screen::BookAppointment);}
                            else
                            {
                                int maxAId=0;
                                for (int i=0;i<appointments->size();i++) if (appointments->getAll()[i].getAppointmentId()>maxAId) maxAId=appointments->getAll()[i].getAppointmentId();
                                Appointment newAppt(maxAId+1,loggedPatient->getId(),docId,date,slot,"pending");
                                appointments->add(newAppt); FileHandler::appendAppointment(newAppt);
                                int maxBId=0;
                                for (int i=0;i<bills->size();i++) if (bills->getAll()[i].getBillId()>maxBId) maxBId=bills->getAll()[i].getBillId();
                                char today2[11]; FileHandler::getTodayDate(today2);
                                Bill newBill(maxBId+1,loggedPatient->getId(),maxAId+1,doc->getFee(),"unpaid",today2);
                                bills->add(newBill); FileHandler::appendBill(newBill);
                                *loggedPatient-=doc->getFee(); FileHandler::updatePatient(*loggedPatient);
                                char idBuf2[16]; Validator::intToStr(maxAId+1,idBuf2);
                                char msg2[64]; msg2[0]='\0';
                                Validator::strConcat(msg2,"Appointment booked! ID: "); Validator::strConcat(msg2,idBuf2);
                                clearInputBoxes(); showMessage("Success",msg2,Screen::PatientMenu);
                            }
                        }
                    }
                }
            }
        }
        else if (currentScreen==Screen::CancelAppointment)
        {
            drawListScreen("Cancel Appointment - Pending Appointments");
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[1].setup(520,500,180,45,font,"Confirm Cancel"); buttons[1].rect.setFillColor(sf::Color(231,76,60));
            buttons[1].updateHover(mp2); buttons[1].draw(window);
            if (clicked)
            {
                if (buttons[0].contains(mousePos)){currentScreen=Screen::PatientMenu;listScrollY=0.f;}
                else if (buttons[1].contains(mousePos))
                {
                    int aId=Validator::strToInt(inputBoxes[0].buffer);
                    Appointment* appt=appointments->findById(aId);
                    if (!appt||appt->getPatientId()!=loggedPatient->getId()||Validator::strComp(appt->getStatus(),"pending")!=0)
                    { showMessage("Error","Invalid appointment ID.",Screen::CancelAppointment); }
                    else
                    {
                        Doctor* doc=doctors->findById(appt->getDoctorId()); float refund=doc?doc->getFee():0.f;
                        appt->setStatus("cancelled"); FileHandler::updateAppointment(*appt);
                        *loggedPatient+=refund; FileHandler::updatePatient(*loggedPatient);
                        for (int i=0;i<bills->size();i++)
                            if (bills->getAll()[i].getAppointmentId()==aId){bills->getAll()[i].setStatus("cancelled");FileHandler::updateBill(bills->getAll()[i]);break;}
                        char refBuf[32]; Validator::floatToStr(refund,refBuf);
                        char msg2[128]; msg2[0]='\0';
                        Validator::strConcat(msg2,"Cancelled. PKR "); Validator::strConcat(msg2,refBuf); Validator::strConcat(msg2," refunded.");
                        clearInputBoxes(); showMessage("Cancelled",msg2,Screen::PatientMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::PayBill)
        {
            drawListScreen("Pay Bill - Unpaid Bills"); inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[1].setup(520,500,150,45,font,"Pay"); buttons[1].rect.setFillColor(sf::Color(39,174,96));
            buttons[1].updateHover(mp2); buttons[1].draw(window);
            if (clicked)
            {
                if (buttons[0].contains(mousePos)){currentScreen=Screen::PatientMenu;listScrollY=0.f;}
                else if (buttons[1].contains(mousePos))
                {
                    int bId=Validator::strToInt(inputBoxes[0].buffer); Bill* bill=bills->findById(bId);
                    if (!bill||bill->getPatientId()!=loggedPatient->getId()||Validator::strComp(bill->getStatus(),"unpaid")!=0)
                    { showMessage("Error","Invalid Bill ID.",Screen::PayBill); }
                    else if (loggedPatient->getBalance()<bill->getAmount())
                    { showMessage("Insufficient Funds","Balance is less than bill amount.",Screen::PayBill); }
                    else
                    {
                        *loggedPatient-=bill->getAmount(); bill->setStatus("paid");
                        FileHandler::updateBill(*bill); FileHandler::updatePatient(*loggedPatient);
                        char balBuf2[32]; Validator::floatToStr(loggedPatient->getBalance(),balBuf2);
                        char msg2[128]; msg2[0]='\0';
                        Validator::strConcat(msg2,"Bill paid. Remaining balance: PKR "); Validator::strConcat(msg2,balBuf2);
                        clearInputBoxes(); showMessage("Success",msg2,Screen::PatientMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::TopUpBalance)
        {
            drawBackground(); drawTopBar("Top Up Balance");
            sf::Text lbl(font,"Enter amount to add (PKR):",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,180,45,font,"Top Up"); buttons[0].rect.setFillColor(sf::Color(39,174,96));
            buttons[1].setup(640,420,180,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::PatientMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    if (!Validator::isPositiveFloat(inputBoxes[0].buffer)){showMessage("Error","Invalid amount.",Screen::TopUpBalance);}
                    else
                    {
                        float amt=Validator::strToFloat(inputBoxes[0].buffer);
                        *loggedPatient+=amt; FileHandler::updatePatient(*loggedPatient);
                        char newBal[32]; Validator::floatToStr(loggedPatient->getBalance(),newBal);
                        char msg2[128]; msg2[0]='\0';
                        Validator::strConcat(msg2,"Balance updated. New balance: PKR "); Validator::strConcat(msg2,newBal);
                        clearInputBoxes(); showMessage("Success",msg2,Screen::PatientMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::DoctorMarkComplete)
        {
            drawBackground(); drawTopBar("Mark Appointment Complete");
            sf::Text lbl(font,"Enter Appointment ID:",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,200,45,font,"Mark Complete"); buttons[0].rect.setFillColor(sf::Color(39,174,96));
            buttons[1].setup(660,420,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::DoctorMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    char today2[11]; FileHandler::getTodayDate(today2);
                    int aId=Validator::strToInt(inputBoxes[0].buffer); Appointment* appt=appointments->findById(aId);
                    if (!appt||appt->getDoctorId()!=loggedDoctor->getId()||Validator::strComp(appt->getStatus(),"pending")!=0||Validator::strComp(appt->getDate(),today2)!=0)
                    { showMessage("Error","Invalid appointment ID.",Screen::DoctorMarkComplete); }
                    else { appt->setStatus("completed"); FileHandler::updateAppointment(*appt); clearInputBoxes(); showMessage("Success","Appointment marked as completed.",Screen::DoctorMenu); }
                }
            }
        }
        else if (currentScreen==Screen::DoctorMarkNoShow)
        {
            drawBackground(); drawTopBar("Mark Appointment No-Show");
            sf::Text lbl(font,"Enter Appointment ID:",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,200,45,font,"Mark No-Show"); buttons[0].rect.setFillColor(sf::Color(231,76,60));
            buttons[1].setup(660,420,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::DoctorMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    char today2[11]; FileHandler::getTodayDate(today2);
                    int aId=Validator::strToInt(inputBoxes[0].buffer); Appointment* appt=appointments->findById(aId);
                    if (!appt||appt->getDoctorId()!=loggedDoctor->getId()||Validator::strComp(appt->getStatus(),"pending")!=0||Validator::strComp(appt->getDate(),today2)!=0)
                    { showMessage("Error","Invalid appointment ID.",Screen::DoctorMarkNoShow); }
                    else
                    {
                        appt->setStatus("noshow"); FileHandler::updateAppointment(*appt);
                        for (int i=0;i<bills->size();i++)
                            if (bills->getAll()[i].getAppointmentId()==aId){bills->getAll()[i].setStatus("cancelled");FileHandler::updateBill(bills->getAll()[i]);break;}
                        clearInputBoxes(); showMessage("Success","Appointment marked as no-show.",Screen::DoctorMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::DoctorWritePrescription)
        {
            drawBackground(); drawTopBar("Write Prescription");
            sf::Text lbl(font,"",15); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setString("Appointment ID:"); lbl.setPosition(sf::Vector2f(200,175)); window.draw(lbl);
            inputBoxes[0].draw(window);
            lbl.setString("Medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg):"); lbl.setPosition(sf::Vector2f(200,285)); window.draw(lbl);
            inputBoxes[1].draw(window);
            lbl.setString("Notes:"); lbl.setPosition(sf::Vector2f(200,395)); window.draw(lbl);
            inputBoxes[2].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(200,490,200,45,font,"Save Prescription"); buttons[0].rect.setFillColor(sf::Color(155,89,182));
            buttons[1].setup(430,490,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::DoctorMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    int aId=Validator::strToInt(inputBoxes[0].buffer); Appointment* appt=appointments->findById(aId);
                    if (!appt||appt->getDoctorId()!=loggedDoctor->getId()||Validator::strComp(appt->getStatus(),"completed")!=0)
                    { showMessage("Error","Invalid appointment ID.",Screen::DoctorWritePrescription); }
                    else
                    {
                        bool exists=false;
                        for (int i=0;i<prescriptions->size();i++) if (prescriptions->getAll()[i].getAppointmentId()==aId){exists=true;break;}
                        if (exists){showMessage("Error","Prescription already written for this appointment.",Screen::DoctorWritePrescription);}
                        else
                        {
                            int maxPrId=0;
                            for (int i=0;i<prescriptions->size();i++) if (prescriptions->getAll()[i].getPrescriptionId()>maxPrId) maxPrId=prescriptions->getAll()[i].getPrescriptionId();
                            Prescription pr(maxPrId+1,aId,appt->getPatientId(),loggedDoctor->getId(),appt->getDate(),inputBoxes[1].buffer,inputBoxes[2].buffer);
                            prescriptions->add(pr); FileHandler::appendPrescription(pr);
                            clearInputBoxes(); showMessage("Success","Prescription saved.",Screen::DoctorMenu);
                        }
                    }
                }
            }
        }
        else if (currentScreen==Screen::DoctorViewHistory)
        {
            drawBackground(); drawTopBar("View Patient Medical History");
            sf::Text lbl(font,"Enter Patient ID:",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,180,45,font,"View History");
            buttons[1].setup(640,420,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::DoctorMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    int pId=Validator::strToInt(inputBoxes[0].buffer); Patient* p=patients->findById(pId);
                    if (!p){showMessage("Access Denied","Patient not found.",Screen::DoctorViewHistory);}
                    else
                    {
                        bool hasCompleted=false;
                        for (int i=0;i<appointments->size();i++)
                            if (appointments->getAll()[i].getPatientId()==pId&&appointments->getAll()[i].getDoctorId()==loggedDoctor->getId()&&Validator::strComp(appointments->getAll()[i].getStatus(),"completed")==0)
                            { hasCompleted=true; break; }
                        if (!hasCompleted){showMessage("Access Denied","You can only view records of your own patients.",Screen::DoctorViewHistory);}
                        else
                        {
                            listLineCount=0; Prescription tmp2[100]; int cnt2=0;
                            for (int i=0;i<prescriptions->size();i++)
                                if (prescriptions->getAll()[i].getPatientId()==pId&&prescriptions->getAll()[i].getDoctorId()==loggedDoctor->getId())
                                    tmp2[cnt2++]=prescriptions->getAll()[i];
                            if (cnt2==0){showMessage("History","No records found.",Screen::DoctorMenu);}
                            else
                            {
                                sortPrescriptionsByDateDesc(tmp2,cnt2);
                                for (int i=0;i<cnt2;i++)
                                {
                                    char line[256]; line[0]='\0';
                                    Validator::strConcat(line,"Date: "); Validator::strConcat(line,tmp2[i].getDate());
                                    Validator::strConcat(line,"  |  Medicines: ");
                                    char medShort[60]; int mLen=Validator::strLen(tmp2[i].getMedicines()); int cL=mLen>55?55:mLen;
                                    for (int j=0;j<cL;j++) medShort[j]=tmp2[i].getMedicines()[j];
                                    medShort[cL]='\0'; if (mLen>55) Validator::strConcat(medShort,"...");
                                    Validator::strConcat(line,medShort);
                                    Validator::strCopy(listLines[listLineCount++],line);
                                }
                                clearInputBoxes(); currentScreen=Screen::DoctorTodayAppts;
                            }
                        }
                    }
                }
            }
        }
        else if (currentScreen==Screen::AdminAddDoctor)
        {
            drawBackground(); drawTopBar("Add Doctor");
            sf::Text lbl(font,"",14); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            const char* lblTxt[]={"Name:","Specialization:","Contact (11 digits):","Password (min 6):","Fee:"};
            float lx[]={100,100,100,600,600}, ly[]={108,188,268,108,188};
            for (int i=0;i<5;i++){lbl.setString(lblTxt[i]);lbl.setPosition(sf::Vector2f(lx[i],ly[i]));window.draw(lbl);inputBoxes[i].draw(window);}
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(100,380,180,45,font,"Add Doctor"); buttons[0].rect.setFillColor(sf::Color(39,174,96));
            buttons[1].setup(310,380,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::AdminMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    if (!Validator::isValidContact(inputBoxes[2].buffer)){showMessage("Error","Invalid contact.",Screen::AdminAddDoctor);}
                    else if (!Validator::isValidPassword(inputBoxes[3].buffer)){showMessage("Error","Password too short.",Screen::AdminAddDoctor);}
                    else if (!Validator::isPositiveFloat(inputBoxes[4].buffer)){showMessage("Error","Invalid fee.",Screen::AdminAddDoctor);}
                    else
                    {
                        int maxId=0;
                        for (int i=0;i<doctors->size();i++) if (doctors->getAll()[i].getId()>maxId) maxId=doctors->getAll()[i].getId();
                        float fee=Validator::strToFloat(inputBoxes[4].buffer);
                        Doctor d(maxId+1,inputBoxes[0].buffer,inputBoxes[1].buffer,inputBoxes[2].buffer,inputBoxes[3].buffer,fee);
                        doctors->add(d); FileHandler::appendDoctor(d);
                        char idBuf2[16]; Validator::intToStr(maxId+1,idBuf2);
                        char msg2[64]; msg2[0]='\0'; Validator::strConcat(msg2,"Doctor added. ID: "); Validator::strConcat(msg2,idBuf2);
                        clearInputBoxes(); showMessage("Success",msg2,Screen::AdminMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::AdminRemoveDoctor)
        {
            drawBackground(); drawTopBar("Remove Doctor");
            sf::Text lbl(font,"Enter Doctor ID to remove:",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,180,45,font,"Remove"); buttons[0].rect.setFillColor(sf::Color(231,76,60));
            buttons[1].setup(640,420,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::AdminMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    int dId=Validator::strToInt(inputBoxes[0].buffer); bool hasPending=false;
                    for (int i=0;i<appointments->size();i++)
                        if (appointments->getAll()[i].getDoctorId()==dId&&Validator::strComp(appointments->getAll()[i].getStatus(),"pending")==0){hasPending=true;break;}
                    if (hasPending){showMessage("Error","Cannot remove doctor with pending appointments.",Screen::AdminRemoveDoctor);}
                    else{doctors->removeById(dId);FileHandler::deleteDoctor(dId);clearInputBoxes();showMessage("Success","Doctor removed.",Screen::AdminMenu);}
                }
            }
        }
        else if (currentScreen==Screen::AdminAddPatient)
        {
            drawBackground(); drawTopBar("Add Patient");
            sf::Text lbl(font,"",14); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            const char* lblTxt[]={"Name:","Age:","Gender (M/F):","Contact (11 digits):","Password (min 6):","Initial Balance:"};
            float lx[]={100,100,100,600,600,600}, ly[]={108,188,268,108,188,268};
            for (int i=0;i<6;i++){lbl.setString(lblTxt[i]);lbl.setPosition(sf::Vector2f(lx[i],ly[i]));window.draw(lbl);inputBoxes[i].draw(window);}
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(100,380,180,45,font,"Add Patient"); buttons[0].rect.setFillColor(sf::Color(39,174,96));
            buttons[1].setup(310,380,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::AdminMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    if (!Validator::isPositiveInt(inputBoxes[1].buffer)){showMessage("Error","Invalid age.",Screen::AdminAddPatient);}
                    else if (!Validator::isValidGender(inputBoxes[2].buffer)){showMessage("Error","Invalid gender. Enter M or F.",Screen::AdminAddPatient);}
                    else if (!Validator::isValidContact(inputBoxes[3].buffer)){showMessage("Error","Invalid contact.",Screen::AdminAddPatient);}
                    else if (!Validator::isValidPassword(inputBoxes[4].buffer)){showMessage("Error","Password too short.",Screen::AdminAddPatient);}
                    else if (!Validator::isPositiveFloat(inputBoxes[5].buffer)){showMessage("Error","Invalid balance.",Screen::AdminAddPatient);}
                    else
                    {
                        int maxId=0;
                        for (int i=0;i<patients->size();i++) if (patients->getAll()[i].getId()>maxId) maxId=patients->getAll()[i].getId();
                        int age=Validator::strToInt(inputBoxes[1].buffer); float bal=Validator::strToFloat(inputBoxes[5].buffer);
                        Patient p(maxId+1,inputBoxes[0].buffer,age,inputBoxes[2].buffer,inputBoxes[3].buffer,inputBoxes[4].buffer,bal);
                        patients->add(p); FileHandler::appendPatient(p);
                        char idBuf2[16]; Validator::intToStr(maxId+1,idBuf2);
                        char msg2[64]; msg2[0]='\0'; Validator::strConcat(msg2,"Patient added. ID: "); Validator::strConcat(msg2,idBuf2);
                        clearInputBoxes(); showMessage("Success",msg2,Screen::AdminMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::AdminRemovePatient)
        {
            drawBackground(); drawTopBar("Remove Patient");
            sf::Text lbl(font,"Enter Patient ID to remove:",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,180,45,font,"Remove"); buttons[0].rect.setFillColor(sf::Color(231,76,60));
            buttons[1].setup(640,420,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::AdminMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    int pId=Validator::strToInt(inputBoxes[0].buffer); bool hasPending=false,hasUnpaid=false;
                    for (int i=0;i<appointments->size();i++) if (appointments->getAll()[i].getPatientId()==pId&&Validator::strComp(appointments->getAll()[i].getStatus(),"pending")==0){hasPending=true;break;}
                    for (int i=0;i<bills->size();i++) if (bills->getAll()[i].getPatientId()==pId&&Validator::strComp(bills->getAll()[i].getStatus(),"unpaid")==0){hasUnpaid=true;break;}
                    if (hasPending){showMessage("Error","Cannot remove patient with pending appointments.",Screen::AdminRemovePatient);}
                    else if (hasUnpaid){showMessage("Error","Cannot remove patient with unpaid bills.",Screen::AdminRemovePatient);}
                    else
                    {
                        patients->removeById(pId); FileHandler::deletePatient(pId);
                        FileHandler::deleteAppointmentsByPatient(pId); FileHandler::deleteBillsByPatient(pId); FileHandler::deletePrescriptionsByPatient(pId);
                        clearInputBoxes(); showMessage("Success","Patient removed.",Screen::AdminMenu);
                    }
                }
            }
        }
        else if (currentScreen==Screen::AdminDischarge)
        {
            drawBackground(); drawTopBar("Discharge Patient");
            sf::Text lbl(font,"Enter Patient ID to discharge:",16); lbl.setFillColor(sf::Color::White); lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(440,320)); window.draw(lbl);
            inputBoxes[0].draw(window);
            sf::Vector2f mp2=window.mapPixelToCoords(sf::Mouse::getPosition(window));
            buttons[0].setup(440,420,180,45,font,"Discharge"); buttons[0].rect.setFillColor(sf::Color(155,89,182));
            buttons[1].setup(640,420,150,45,font,"Back"); buttons[1].rect.setFillColor(sf::Color(127,140,141));
            for (int i=0;i<2;i++){buttons[i].updateHover(mp2);buttons[i].draw(window);}
            if (clicked)
            {
                if (buttons[1].contains(mousePos)){currentScreen=Screen::AdminMenu;}
                else if (buttons[0].contains(mousePos))
                {
                    int pId=Validator::strToInt(inputBoxes[0].buffer); bool hasPending=false,hasUnpaid=false;
                    for (int i=0;i<bills->size();i++) if (bills->getAll()[i].getPatientId()==pId&&Validator::strComp(bills->getAll()[i].getStatus(),"unpaid")==0){hasUnpaid=true;break;}
                    for (int i=0;i<appointments->size();i++) if (appointments->getAll()[i].getPatientId()==pId&&Validator::strComp(appointments->getAll()[i].getStatus(),"pending")==0){hasPending=true;break;}
                    if (hasUnpaid){showMessage("Error","Cannot discharge patient with unpaid bills.",Screen::AdminDischarge);}
                    else if (hasPending){showMessage("Error","Cannot discharge patient with pending appointments.",Screen::AdminDischarge);}
                    else
                    {
                        FileHandler::archivePatient(pId,*patients,*appointments,*bills,*prescriptions);
                        patients->removeById(pId); FileHandler::deletePatient(pId);
                        FileHandler::deleteAppointmentsByPatient(pId); FileHandler::deleteBillsByPatient(pId); FileHandler::deletePrescriptionsByPatient(pId);
                        clearInputBoxes(); showMessage("Success","Patient discharged and archived successfully.",Screen::AdminMenu);
                    }
                }
            }
        }

        window.display();
    }
}
