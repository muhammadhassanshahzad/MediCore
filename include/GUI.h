#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
using namespace std;

struct UIButton
{
    sf::RectangleShape rect;
    std::optional<sf::Text> label;
    bool isHovered;

    UIButton()
    {
        isHovered = false;
    }

    void setup(float x, float y, float w, float h, const sf::Font& font, const char* text, unsigned int fontSize = 18)
    {
        rect.setPosition(sf::Vector2f(x, y));
        rect.setSize(sf::Vector2f(w, h));
        rect.setFillColor(sf::Color(52, 152, 219));
        rect.setOutlineThickness(2);
        rect.setOutlineColor(sf::Color(41, 128, 185));

        label.emplace(font, text, fontSize);
        label->setFillColor(sf::Color::White);
        sf::FloatRect tb = label->getLocalBounds();
        label->setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f));
        label->setPosition(sf::Vector2f(x + w / 2.f, y + h / 2.f));
    }

    bool contains(sf::Vector2f point)
    {
        return rect.getGlobalBounds().contains(point);
    }

    void updateHover(sf::Vector2f mousePos)
    {
        isHovered = contains(mousePos);
        if (isHovered)
            rect.setFillColor(sf::Color(41, 128, 185));
        else
            rect.setFillColor(sf::Color(52, 152, 219));
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(rect);
        if (label) window.draw(*label);
    }
};

struct UIInputBox
{
    sf::RectangleShape rect;
    std::optional<sf::Text> displayText;
    std::optional<sf::Text> placeholderText;
    char buffer[256];
    int bufLen;
    bool active;
    bool isPassword;

    UIInputBox()
    {
        buffer[0] = '\0';
        bufLen = 0;
        active = false;
        isPassword = false;
    }

    void setup(float x, float y, float w, float h, const sf::Font& font, const char* placeholder, bool password = false)
    {
        rect.setPosition(sf::Vector2f(x, y));
        rect.setSize(sf::Vector2f(w, h));
        rect.setFillColor(sf::Color(236, 240, 241));
        rect.setOutlineThickness(2);
        rect.setOutlineColor(sf::Color(189, 195, 199));

        displayText.emplace(font, "", 16);
        displayText->setFillColor(sf::Color(44, 62, 80));
        displayText->setPosition(sf::Vector2f(x + 10, y + h / 2.f - 10));

        placeholderText.emplace(font, placeholder, 16);
        placeholderText->setFillColor(sf::Color(149, 165, 166));
        placeholderText->setPosition(sf::Vector2f(x + 10, y + h / 2.f - 10));

        isPassword = password;
        rect.setOutlineColor(active ? sf::Color(52, 152, 219) : sf::Color(189, 195, 199));
    }

    void handleEvent(sf::Event& event)
    {
        if (!active) return;
        if (event.is<sf::Event::TextEntered>())
        {
            auto* te = event.getIf<sf::Event::TextEntered>();
            if (te)
            {
                if (te->unicode == 8)
                {
                    if (bufLen > 0) { bufLen--; buffer[bufLen] = '\0'; }
                }
                else if (te->unicode >= 32 && te->unicode < 127 && bufLen < 255)
                {
                    buffer[bufLen] = (char)te->unicode;
                    bufLen++;
                    buffer[bufLen] = '\0';
                }
            }
        }
    }

    void updateActive(sf::Vector2f mousePos, bool clicked)
    {
        if (clicked)
        {
            active = rect.getGlobalBounds().contains(mousePos);
            rect.setOutlineColor(active ? sf::Color(52, 152, 219) : sf::Color(189, 195, 199));
        }
    }

    void clear()
    {
        buffer[0] = '\0';
        bufLen = 0;
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(rect);
        if (bufLen == 0)
        {
            if (placeholderText) window.draw(*placeholderText);
        }
        else
        {
            if (displayText)
            {
                if (isPassword)
                {
                    char stars[256];
                    for (int i = 0; i < bufLen; i++) { stars[i] = '*'; }
                    stars[bufLen] = '\0';
                    displayText->setString(stars);
                }
                else
                {
                    displayText->setString(buffer);
                }
                window.draw(*displayText);
            }
        }
    }
};

struct UILabel
{
    std::optional<sf::Text> text;

    void setup(float x, float y, const sf::Font& font, const char* str, unsigned int size = 16, sf::Color color = sf::Color(44, 62, 80))
    {
        text.emplace(font, str, size);
        text->setFillColor(color);
        text->setPosition(sf::Vector2f(x, y));
    }

    void setString(const char* str)
    {
        if (text) text->setString(str);
    }

    void draw(sf::RenderWindow& window)
    {
        if (text) window.draw(*text);
    }
};

struct ScrollableList
{
    sf::RectangleShape background;
    sf::View listView;
    float scrollOffset;
    float maxScroll;
    float itemHeight;

    void setup(float x, float y, float w, float h)
    {
        background.setPosition(sf::Vector2f(x, y));
        background.setSize(sf::Vector2f(w, h));
        background.setFillColor(sf::Color(245, 246, 250));
        background.setOutlineThickness(1);
        background.setOutlineColor(sf::Color(189, 195, 199));
        listView.setViewport(sf::FloatRect(sf::Vector2f(x / 1280.f, y / 720.f), sf::Vector2f(w / 1280.f, h / 720.f)));
        listView.setSize(sf::Vector2f(w, h));
        listView.setCenter(sf::Vector2f(w / 2.f, h / 2.f));
        scrollOffset = 0.f;
        maxScroll = 0.f;
        itemHeight = 40.f;
    }

    void scroll(float delta)
    {
        scrollOffset -= delta * 30.f;
        if (scrollOffset < 0.f) { scrollOffset = 0.f; }
        if (scrollOffset > maxScroll) { scrollOffset = maxScroll; }
        listView.setCenter(sf::Vector2f(listView.getSize().x / 2.f, listView.getSize().y / 2.f + scrollOffset));
    }
};

enum class Screen
{
    MainLogin,
    PatientLogin,
    DoctorLogin,
    AdminLogin,
    PatientMenu,
    DoctorMenu,
    AdminMenu,
    BookAppointment,
    CancelAppointment,
    ViewAppointments,
    ViewMedicalRecords,
    ViewBills,
    PayBill,
    TopUpBalance,
    DoctorTodayAppts,
    DoctorMarkComplete,
    DoctorMarkNoShow,
    DoctorWritePrescription,
    DoctorViewHistory,
    AdminAddDoctor,
    AdminRemoveDoctor,
    AdminAddPatient,
    AdminRemovePatient,
    AdminViewPatients,
    AdminViewDoctors,
    AdminViewAppointments,
    AdminViewUnpaidBills,
    AdminDischarge,
    AdminSecurityLog,
    AdminDailyReport,
    MessageScreen
};

class GUI
{
private:
    sf::Texture bgTexture;
    optional<sf::Sprite> bgSprite;
    sf::RenderWindow window;
    sf::Font font;
    Screen currentScreen;
    Screen previousScreen;
    char messageText[512];
    char messageTitle[128];

    Patient* loggedPatient;
    Doctor* loggedDoctor;
    bool adminLogged;
    int loginAttempts;

    Storage<Patient>* patients;
    Storage<Doctor>* doctors;
    Storage<Appointment>* appointments;
    Storage<Bill>* bills;
    Storage<Prescription>* prescriptions;
    Admin* adminPtr;

    UIInputBox inputBoxes[6];
    UIButton buttons[16];
    UILabel labels[32];
    char listLines[100][256];
    int listLineCount;
    float listScrollY;

    char bookSpecInput[51];
    char bookDateInput[11];
    char bookSlotInput[6];
    int bookSelectedDoctorId;
    float bookSelectedDoctorFee;
    char bookAvailableSlots[8][6];
    int bookAvailableSlotCount;

    int selectedAppointmentId;
    int selectedBillId;
    char inputSingleBuffer[256];

    void drawBackground();
    void drawTopBar(const char* title);
    void drawMessageScreen();
    void drawMainLogin();
    void drawRoleLogin(const char* role);
    void drawPatientMenu();
    void drawDoctorMenu();
    void drawAdminMenu();
    void drawListScreen(const char* title, const char* backLabel = "Back");
    void drawSingleInputScreen(const char* title, const char* prompt, const char* btnLabel, UIInputBox& box, UIButton& confirmBtn, UIButton& backBtn);

    void handleMainLogin(sf::Event& event, sf::Vector2f mousePos, bool clicked);
    void handleRoleLogin(sf::Event& event, sf::Vector2f mousePos, bool clicked, const char* role);
    void handlePatientMenu(sf::Vector2f mousePos, bool clicked);
    void handleDoctorMenu(sf::Vector2f mousePos, bool clicked);
    void handleAdminMenu(sf::Vector2f mousePos, bool clicked);

    void doBookAppointment();
    void doCancelAppointment();
    void doViewAppointments();
    void doViewMedicalRecords();
    void doViewBills();
    void doPayBill();
    void doTopUp();
    void doDoctorToday();
    void doDoctorMarkComplete();
    void doDoctorMarkNoShow();
    void doDoctorViewHistory();
    void doAdminViewPatients();
    void doAdminViewDoctors();
    void doAdminViewAppointments();
    void doAdminViewUnpaidBills();
    void doAdminSecurityLog();
    void doAdminDailyReport();

    void showMessage(const char* title, const char* msg, Screen returnTo);
    void buildList();
    void clearInputBoxes();

    int compareDates(const char* d1, const char* d2);
    int dateDiffFromToday(const char* date);
    void sortAppointmentsByDateAsc(Appointment* arr, int n);
    void sortAppointmentsByDateDesc(Appointment* arr, int n);
    void sortPrescriptionsByDateDesc(Prescription* arr, int n);

public:
    GUI(Storage<Patient>& patients, Storage<Doctor>& doctors, Storage<Appointment>& appointments, Storage<Bill>& bills, Storage<Prescription>& prescriptions, Admin& admin);
    void run();
};