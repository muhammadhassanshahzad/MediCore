#include "../include/Validator.h"
#include <ctime>
#include <cctype>
using namespace std;

int Validator::strLen(const char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        i++;
    }
    return i;
}

void Validator::strCopy(char* dest, const char* src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int Validator::strComp(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return a[i] - b[i];
        }
        i++;
    }
    return a[i] - b[i];
}

void Validator::strConcat(char* dest, const char* src)
{
    int i = strLen(dest);
    int j = 0;
    while (src[j] != '\0')
    {
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0';
}

void Validator::toLowerStr(char* dest, const char* src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

bool Validator::strContains(const char* haystack, const char* needle)
{
    int hLen = strLen(haystack);
    int nLen = strLen(needle);
    for (int i = 0; i <= hLen - nLen; i++)
    {
        bool found = true;
        for (int j = 0; j < nLen; j++)
        {
            if (haystack[i + j] != needle[j])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return true;
        }
    }
    return false;
}

float Validator::strToFloat(const char* str)
{
    float result = 0.0f;
    float decimal = 0.0f;
    bool negative = false;
    bool inDecimal = false;
    float decimalPlace = 0.1f;
    int i = 0;
    if (str[i] == '-')
    {
        negative = true;
        i++;
    }
    while (str[i] != '\0')
    {
        if (str[i] == '.')
        {
            inDecimal = true;
        }
        else if (str[i] >= '0' && str[i] <= '9')
        {
            if (inDecimal)
            {
                decimal += (str[i] - '0') * decimalPlace;
                decimalPlace *= 0.1f;
            }
            else
            {
                result = result * 10 + (str[i] - '0');
            }
        }
        i++;
    }
    result += decimal;
    if (negative)
    {
        result = -result;
    }
    return result;
}

int Validator::strToInt(const char* str)
{
    int result = 0;
    bool negative = false;
    int i = 0;
    if (str[i] == '-')
    {
        negative = true;
        i++;
    }
    while (str[i] != '\0')
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + (str[i] - '0');
        }
        i++;
    }
    if (negative)
    {
        result = -result;
    }
    return result;
}

void Validator::intToStr(int val, char* buf)
{
    if (val == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    char tmp[32];
    int i = 0;
    bool negative = false;
    if (val < 0)
    {
        negative = true;
        val = -val;
    }
    while (val > 0)
    {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    if (negative)
    {
        tmp[i++] = '-';
    }
    int len = i;
    for (int j = 0; j < len; j++)
    {
        buf[j] = tmp[len - 1 - j];
    }
    buf[len] = '\0';
}

void Validator::floatToStr(float val, char* buf, int decimals)
{
    if (val < 0)
    {
        buf[0] = '-';
        val = -val;
        floatToStr(val, buf + 1, decimals);
        return;
    }
    int intPart = (int)val;
    float fracPart = val - intPart;
    char intBuf[32];
    intToStr(intPart, intBuf);
    int i = 0;
    while (intBuf[i] != '\0')
    {
        buf[i] = intBuf[i];
        i++;
    }
    buf[i++] = '.';
    for (int d = 0; d < decimals; d++)
    {
        fracPart *= 10;
        int digit = (int)fracPart;
        buf[i++] = '0' + digit;
        fracPart -= digit;
    }
    buf[i] = '\0';
}

bool Validator::isPositiveFloat(const char* str)
{
    int i = 0;
    bool hasDot = false;
    bool hasDigit = false;
    if (str[i] == '\0')
    {
        return false;
    }
    while (str[i] != '\0')
    {
        if (str[i] == '.')
        {
            if (hasDot)
            {
                return false;
            }
            hasDot = true;
        }
        else if (str[i] >= '0' && str[i] <= '9')
        {
            hasDigit = true;
        }
        else
        {
            return false;
        }
        i++;
    }
    if (!hasDigit)
    {
        return false;
    }
    float val = strToFloat(str);
    return val > 0.0f;
}

bool Validator::isPositiveInt(const char* str)
{
    int i = 0;
    if (str[i] == '\0')
    {
        return false;
    }
    while (str[i] != '\0')
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return false;
        }
        i++;
    }
    return strToInt(str) > 0;
}

bool Validator::isValidDate(const char* date)
{
    if (strLen(date) != 10)
    {
        return false;
    }
    if (date[2] != '-' || date[5] != '-')
    {
        return false;
    }
    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
        {
            continue;
        }
        if (date[i] < '0' || date[i] > '9')
        {
            return false;
        }
    }
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');
    if (day < 1 || day > 31)
    {
        return false;
    }
    if (month < 1 || month > 12)
    {
        return false;
    }
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    int currentYear = t->tm_year + 1900;
    if (year < currentYear)
    {
        return false;
    }
    return true;
}

bool Validator::isValidTimeSlot(const char* slot)
{
    const char* validSlots[] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00" };
    for (int i = 0; i < 8; i++)
    {
        if (strComp(slot, validSlots[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool Validator::isValidContact(const char* contact)
{
    if (strLen(contact) != 11)
    {
        return false;
    }
    for (int i = 0; i < 11; i++)
    {
        if (contact[i] < '0' || contact[i] > '9')
        {
            return false;
        }
    }
    return true;
}

bool Validator::isValidPassword(const char* password)
{
    return strLen(password) >= 6;
}

bool Validator::isValidGender(const char* gender)
{
    return (strLen(gender) == 1 && (gender[0] == 'M' || gender[0] == 'F'));
}

bool Validator::isValidMenuChoice(int choice, int min, int max)
{
    return choice >= min && choice <= max;
}