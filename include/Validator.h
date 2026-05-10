#pragma once
using namespace std;

class Validator
{
public:
    static bool isPositiveFloat(const char* str);
    static bool isPositiveInt(const char* str);
    static bool isValidDate(const char* date);
    static bool isValidTimeSlot(const char* slot);
    static bool isValidContact(const char* contact);
    static bool isValidPassword(const char* password);
    static bool isValidGender(const char* gender);
    static bool isValidMenuChoice(int choice, int min, int max);
    static int strLen(const char* str);
    static void strCopy(char* dest, const char* src);
    static int strComp(const char* a, const char* b);
    static void strConcat(char* dest, const char* src);
    static void toLowerStr(char* dest, const char* src);
    static bool strContains(const char* haystack, const char* needle);
    static float strToFloat(const char* str);
    static int strToInt(const char* str);
    static void intToStr(int val, char* buf);
    static void floatToStr(float val, char* buf, int decimals = 2);
};