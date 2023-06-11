#include <Arduino.h>
#include "LoginDataManager.hpp"
#include "Oled.hpp"
#include <Keypad.h>
#include "RTClib.h"

#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_

class Settings
{
private:
    Keypad *Keyboard;
    LoginDataManager *LoginManager;
    RTC_DS3231 *Rtc;
    Oled *Screen;

    struct newUserData
    {
        String username;
        String password;
        unsigned long ExpirationTime;
    };

    struct dateAndTime
    {
        uint16_t year = 0;
        uint8_t month = 0;
        uint8_t day = 0;
        uint8_t hour = 0;
        uint8_t min = 0;
    };

    void setDateAndTime();

    void changeExpirationTime();

    void setNewUser();

    void deleteUser();

    bool setName(newUserData *);
    void setPassword(newUserData *);
    void setExpirationTime(newUserData *);

    bool setDate(dateAndTime *);
    bool setTime(dateAndTime *);

public:
    void init(Keypad *, LoginDataManager *, RTC_DS3231 *, Oled *);
    void settings();
};
#endif