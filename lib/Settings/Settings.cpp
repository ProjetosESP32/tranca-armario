#include "Settings.hpp"

void Settings::init(Keypad *Keyboard, LoginDataManager *LoginManager, RTC_DS3231 *Rtc, Oled *Screen)
{
    this->Keyboard = Keyboard;
    this->LoginManager = LoginManager;
    this->Rtc = Rtc;
    this->Screen = Screen;
}

void Settings::settings()
{
    bool setting = true;

    char typedCharacter = '\0';

    (*Screen).showSettingsTitle();
    delay(1000);

    while (setting)
    {
        (*Screen).showMenuConfig();

        do
        {
            typedCharacter = (*Keyboard).getKey();
        } while (typedCharacter == '\0');

        switch (typedCharacter)
        {
        case '1':
            setDateAndTime();
            typedCharacter = '\0';
            break;

        case '2':
            changeExpirationTime();
            typedCharacter = '\0';
            break;

        case '3':
            setNewUser();
            typedCharacter = '\0';
            break;

        case '4':
            deleteUser();
            typedCharacter = '\0';
            break;

        case '\0':

        default:
            setting = false;
            break;
        }
    }
    return;
}

bool Settings::setName(newUserData *newUserDataName)
{
    bool isSetName = true;
    bool status = true;
    int cursor = 0;
    String name = "";
    int count_string = 0;
    int action = 0;

    while (isSetName)
    {
        char typedCharacter = (*Keyboard).getKey();

        if (typedCharacter)
        {

            if (typedCharacter == '*')
            {
                status = false;
                isSetName = false;
            }
            else if (typedCharacter == '2')
            {
                cursor -= NUMBER_OF_COLUMNS;
                cursor = max(cursor, 0);
            }
            else if (typedCharacter == '4')
            {
                if (cursor > 0)
                {
                    cursor--;
                }
                action = 4;
            }
            else if (typedCharacter == '6')
            {
                if (cursor < NUMBER_OF_CHARACTERS - 1)
                {
                    cursor++;
                }
                action = 6;
            }
            else if (typedCharacter == '8')
            {
                cursor += NUMBER_OF_COLUMNS;
                cursor = min(cursor, NUMBER_OF_CHARACTERS - 1);
                action = 0;
            }
            else if (typedCharacter == '5')
            {
                if ((*Screen).getAlphabetCharacter(cursor) == '<')
                {
                    name.remove(count_string - 1);
                    count_string--;
                }
                else
                {
                    count_string++;
                    name += (*Screen).getAlphabetCharacter(cursor);
                }
                action = 0;
            }
            else if (typedCharacter == '#')
            {
                (*newUserDataName).username = name;
                isSetName = false;
                action = 0;
            }
            else
            {
                action = 0;
            }
        }

        if ((*Keyboard).getState() == HOLD)
        {
            if (action == 4)
            {
                if (cursor > 0)
                {
                    cursor--;
                }
            }
            else if (action == 6)
            {
                if (cursor < NUMBER_OF_CHARACTERS - 1)
                {
                    cursor++;
                }
            }
            delayMicroseconds(20000);
        }
        (*Screen).showVirtualKeyboardForSetName(name, cursor);
    }

    return status;
}

void Settings::setPassword(newUserData *newUserDataPassword)
{
    bool isSetPassword = true;
    String password = "";
    int countDigits = 0;

    while (isSetPassword)
    {
        char typedCharacter = (*Keyboard).getKey();
        if (typedCharacter)
        {
            if (typedCharacter == '*')
            {
                password.remove(countDigits - 1);
                countDigits--;
                countDigits = max(countDigits, 0);
            }

            if (typedCharacter != '*' && typedCharacter != '#')
            {
                if (countDigits < LENGTH_OF_PASSWORD)
                {
                    password += typedCharacter;
                    countDigits++;
                }
            }

            if (typedCharacter == '#')
            {
                if (countDigits == LENGTH_OF_PASSWORD)
                {
                    (*newUserDataPassword).password = password;
                    isSetPassword = !isSetPassword;
                }
            }
        }

        (*Screen).showSetPassword(password);
    }
}

void Settings::setExpirationTime(newUserData *newUserDataExpirationTime)
{
    bool isSetExpirationTime = true;
    String sExpirationTime = "";
    int countDigits = 0;

    while (isSetExpirationTime)
    {
        char typedCharacter = (*Keyboard).getKey();

        if (typedCharacter)
        {
            if (typedCharacter == '*')
            {
                sExpirationTime.remove(countDigits - 1);
                countDigits--;
                countDigits = max(countDigits, 0);
            }

            if (typedCharacter != '*' && typedCharacter != '#')
            {
                sExpirationTime += typedCharacter;
                countDigits++;
            }

            if (typedCharacter == '#')
            {
                unsigned long ExpirationTime = sExpirationTime.toInt();
                (*newUserDataExpirationTime).ExpirationTime = ExpirationTime;
                isSetExpirationTime = !isSetExpirationTime;
            }
        }
        (*Screen).showSetExpirationDate(sExpirationTime);
    }
}

//----------------------------------------------------------------

bool Settings::setDate(dateAndTime *newDate)
{
    bool isSetDate = true;
    bool status = true;
    int iYear = 0,
        iMonth = 0,
        iDay = 0,
        iHour = 0,
        iMinute = 0;
    String year = "",
           month = "",
           day = "",
           hour = "",
           minute = "";
    int countDigits = 0;

    (*Screen).showConfigDate(0, 0, 0);

    while (isSetDate)
    {
        char typedCharacter = (*Keyboard).getKey();

        if (typedCharacter)
        {
            if (typedCharacter != '#')
            {

                if(typedCharacter == '*')
                {
                    status = false;
                    return status;
                }

                if (countDigits < 2) // 2 caracters for the day, 2 caracters for the month and 4 caracters for the year
                {
                    day += typedCharacter;
                    countDigits++;
                }
                else if (countDigits < 4)
                {
                    month += typedCharacter;
                    countDigits++;
                }
                else if (countDigits < 8)
                {
                    year += typedCharacter;
                    countDigits++;
                    if (countDigits == 8)
                    {
                        isSetDate = !isSetDate;
                    }
                }

                (*newDate).day = day.toInt();
                (*newDate).month = month.toInt();
                (*newDate).year = year.toInt();
            }

            (*Screen).showConfigDate((*newDate).day, (*newDate).month, (*newDate).year);
        }
    }

    delay(1000);
    return status;
}

bool Settings::setTime(dateAndTime *newTime)
{
    String hour = "",
           minute = "";

    int countDigits = 0;
    bool status = true;

    (*Screen).showConfigTime(0, 0);

    bool isSetTime = true;

    while (isSetTime)
    {
        char typedCharacter = (*Keyboard).getKey();

        if (typedCharacter)
        {
            if (typedCharacter != '#')
            {

                if(typedCharacter == '*')
                {
                    status = false;
                    return status;
                }

                if (countDigits < 2)
                {
                    hour += typedCharacter;
                    countDigits++;
                }
                else if (countDigits < 4)
                {
                    minute += typedCharacter;
                    countDigits++;

                    if (countDigits == 4)
                    {
                        isSetTime = !isSetTime;
                    }
                }

                (*newTime).hour = hour.toInt();
                (*newTime).min = minute.toInt();
            }

            (*Screen).showConfigTime((*newTime).hour, (*newTime).min);
        }
    }
    return status;
    delay(1000);
}

//----------------------------------------------------------------
void Settings::setNewUser()
{
    newUserData newUser;

    if (setName(&newUser) == false)
    {
        return;
    }
    
    setPassword(&newUser);

    setExpirationTime(&newUser);

    newUser.username.trim();
    newUser.password.trim();

    (*LoginManager).setNewUser(newUser.username, newUser.password, newUser.ExpirationTime);
}

void Settings::setDateAndTime()
{
    dateAndTime dateTime;

    if ( setDate(&dateTime) == false)
    {
        return;
    }

    if (setTime(&dateTime) == false)
    {
        return;
    }
    (*Rtc).adjust(DateTime(dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.min, 0));
}

void Settings::changeExpirationTime()
{
    int number_users = (*LoginManager).getNumberOfUsers();
    int cursorUserList = 0;
    int action = 0;

    bool isSetExpirationTime = true;

    (*Screen).showMenuNames(-1, true);

    (*Screen).showControls();
    delay(2000);

    while (isSetExpirationTime)
    {
        char typedCharacter = (*Keyboard).getKey();

        if (typedCharacter)
        {

            if (typedCharacter == '2')
            {
                if (cursorUserList > 0)
                {
                    cursorUserList--;
                }
                action = 2;
            }
            else if (typedCharacter == '*')
            {
                isSetExpirationTime = !isSetExpirationTime;
                action = 0;
            }
            else if (typedCharacter == '8')
            {
                if (cursorUserList < (number_users - 1))
                {
                    cursorUserList++;
                }
                action = 8;
            }
            else if (typedCharacter == '#')
            {

                int countDigits = 0;

                int expirationTimeSelectedUser = (*LoginManager).getValidityUser(cursorUserList);
                char buffer[5] = "";
                bool isSetExpirationTimeSelectedUser = true;

                while (isSetExpirationTimeSelectedUser)
                {

                    typedCharacter = (*Keyboard).getKey();

                    String number = String(expirationTimeSelectedUser);
                    (*Screen).showSetExpirationDate(number);

                    if (typedCharacter == '*')
                    {
                        isSetExpirationTimeSelectedUser = !isSetExpirationTimeSelectedUser;
                        (*Screen).showMenuNames(-1, true);
                    }

                    if (typedCharacter == '#')
                    {
                        (*LoginManager).setValidityUser(cursorUserList, expirationTimeSelectedUser);
                        isSetExpirationTimeSelectedUser = !isSetExpirationTimeSelectedUser;
                        (*Screen).showMenuNames(-1, true);
                    }

                    if (typedCharacter)
                    {
                        if (typedCharacter != '#' && typedCharacter != '*')
                        {
                            if (countDigits == 0)
                            {
                                number = "0";
                            }

                            if (countDigits < 3)
                            {
                                countDigits++;
                                number += typedCharacter;
                                expirationTimeSelectedUser = number.toInt();
                            }
                            else
                            {
                                countDigits = 0;
                                expirationTimeSelectedUser = number.toInt();
                                number = "";
                            }
                        }
                    }
                }
                action = 0;
            }
            else
            {
                action = 0;
            }
        }

        if ((*Keyboard).getState() == HOLD)
        {
            if (action == 8)
            {
                if (cursorUserList < (number_users - 1))
                {
                    cursorUserList++;
                }
            }
            else if (action == 2)
            {
                if (cursorUserList > 0)
                {
                    cursorUserList--;
                }
            }
            // delayMicroseconds(20000);
        }

        (*Screen).showMenuNames(cursorUserList, false);
    }
}

void Settings::deleteUser()
{
    int number_users = (*LoginManager).getNumberOfUsers();
    int cursorUserList = 0;
    int action = 0;
    bool isDeleteUser = true;

    (*Screen).showMenuNames(-1, true);

    (*Screen).showControls();
    delay(2000);

    while (isDeleteUser)
    {
        char typedCharacter = (*Keyboard).getKey();

        if (typedCharacter)
        {
            if (typedCharacter == '2')
            {
                if (cursorUserList > 0)
                {
                    cursorUserList--;
                }
                action = 2;
            }
            else if (typedCharacter == '*')
            {
                isDeleteUser = false;
                action = 0;
            }
            else if (typedCharacter == '8')
            {
                if (cursorUserList < (number_users - 1))
                {
                    cursorUserList++;
                }
                action = 8;
            }
            else if (typedCharacter == '#')
            {
                bool isConfirmationDeleteUser = true;
                (*Screen).confirmationDeleteUser((*LoginManager).getNameUser(cursorUserList));
                while (isConfirmationDeleteUser)
                {
                    typedCharacter = (*Keyboard).getKey();

                    if (typedCharacter == '#')
                    {
                        (*LoginManager).removeUser(cursorUserList);
                        if (cursorUserList > 0)
                        {
                            cursorUserList--;
                        }
                        number_users--;

                        if (number_users == 0)
                        {
                            break;
                        }

                        isConfirmationDeleteUser = false;
                        (*Screen).showMenuNames(-1, true);
                    }

                    if (typedCharacter == '*')
                    {
                        isConfirmationDeleteUser = false;
                        (*Screen).showMenuNames(-1, true);
                    }
                }
                action = 0;
            }
            else
            {
                action = 0;
            }
        }
        if ((*Keyboard).getState() == HOLD)
        {
            if (action == 8)
            {
                if (cursorUserList < (number_users - 1))
                {
                    cursorUserList++;
                }
            }
            else if (action == 2)
            {
                if (cursorUserList > 0)
                {
                    cursorUserList--;
                }
            }
            // delayMicroseconds(20000);
        }

        (*Screen).showMenuNames(cursorUserList, false);
    }
}
