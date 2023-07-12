#include <Arduino.h>

#include "LoginDataManager.hpp"
#include "Oled.hpp"
#include "Settings.hpp"

#include <Keypad.h>
#include "SPIFFS.h"
#include <Wire.h>
#include "RTClib.h"
#include "DFRobotDFPlayerMini.h"

const int SENSOR_PORTA = 32;
const int RELE = 12;

const String MASTERKEY = "117811";
const String SEC_KEY = "#9*7#02*";
const int LENGTH_OF_PASSWORD_SEC = 8;

const unsigned int hibernationTime = 20000;
const unsigned long int restartTime = 1210000000; // Duas semanas -> reset para garantir estabilidade
const unsigned int maxOpenDoorTime = 30000;
const unsigned int updateTimeInterval = 500;

unsigned long int timeWithoutActivity = -(hibernationTime); //= 0 // Para começar na tela de hibernação
unsigned long int currentProgramTime = 0;
unsigned long int timeDoorWasOpened = 0;
unsigned long int lastCloseDoorTime = 0;
unsigned long int alertDoorTime = -8000;

uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
uint16_t year = 0;
uint8_t month = 0;
uint8_t day = 0;

String typedPassword = "";
int typedCharacterCounter = 0;
int wrongPasswordCounter = 0;

bool isDoorOpenAlert = false;
bool isHibernation = false;

const byte rows = 4;
const byte columns = 4;
char matrixKeyBoard[rows][columns] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[rows] = {4, 18, 19, 33};
byte columnPins[columns] = {25, 26, 27, 13};

HardwareSerial mySoftwareSerial(0);

Keypad KeyBoard = Keypad(makeKeymap(matrixKeyBoard), rowPins, columnPins, rows, columns);
RTC_DS3231 Rtc;
DateTime now;
DFRobotDFPlayerMini myDFPlayer;

LoginDataManager Manager;
Oled Screen;
Settings DeviceSettings;

void handleTime()
{
  static unsigned int timeAtualization = 0;

  if (currentProgramTime - timeAtualization < updateTimeInterval)
  {
    return;
  }

  timeAtualization = currentProgramTime;
  now = Rtc.now();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
  year = now.year();
  month = now.month();
  day = now.day();
}

void wrongPasswordManyTimes()
{
  bool isRecovery = true;

  while (isRecovery)
  {
    char typedCharacter = KeyBoard.getKey();

    if (typedCharacter)
    {
      if (typedCharacterCounter < LENGTH_OF_PASSWORD_SEC)
      {
        typedPassword += typedCharacter;
        typedCharacterCounter++;
      }
      else
      {
        typedPassword = "";
        typedCharacterCounter = 0;
      }
    }

    Screen.showLockedLock(typedPassword);

    if (typedPassword == SEC_KEY)
    {
      isRecovery = false;
      wrongPasswordCounter = 0;
    }
  }
}

void setup()
{
  DeviceSettings.init(&KeyBoard, &Manager, &Rtc, &Screen);

  Screen.begin(&Manager);

  // Serial.begin(115200);

  Screen.showLogo();
  delay(2000);

  if (!Manager.begin())
  {
    Screen.showSpiffsFail();
    while (1)
      ;
  }

  if (!Rtc.begin())
  {
    Screen.showRtcFail();
    while (1)
      ;
  }

  Rtc.disable32K();

  KeyBoard.setDebounceTime(10);
  KeyBoard.setHoldTime(400);

  pinMode(RELE, OUTPUT);
  pinMode(SENSOR_PORTA, INPUT);

  mySoftwareSerial.begin(9600, SERIAL_8N1, 3, 1);
  
  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    Screen.showMP3Fail();
    while (1);
  }
  else
  {
    // Definicoes iniciais MP3
    myDFPlayer.setTimeOut(500); // Timeout serial 500ms
    myDFPlayer.volume(30);      // Volume
    myDFPlayer.EQ(0);           // Equalizacao normal
    myDFPlayer.stop();
  }

  myDFPlayer.play(1);
}

void loop()
{
  handleTime();

  Manager.validityHandle(hour, minute, second, day);

  currentProgramTime = millis();

  char typedCharacter = KeyBoard.getKey();

  if (currentProgramTime - timeWithoutActivity >= hibernationTime)
  {
    isHibernation = true;
  }
  else
  {
    isHibernation = false;
  }

  if (digitalRead(SENSOR_PORTA) == LOW)
  {
    if (currentProgramTime - lastCloseDoorTime >= maxOpenDoorTime)
    {
      isDoorOpenAlert = true;
    }
  }
  else
  {
    lastCloseDoorTime = currentProgramTime;
    isDoorOpenAlert = false;
  }

  if (!isDoorOpenAlert)
  {
    if (isHibernation)
    {
      typedCharacterCounter = 0;
      typedPassword = "";
      static uint8_t lastSecond;

      if (second != lastSecond)
      {
        Screen.showHomeScreen(hour, minute, second, day, month, year, "LAB E-117");
        lastSecond = second;
      }
    }
    else
    {
      Screen.showTypedKey(typedPassword);
    }
  }
  else
  {
    Screen.showOpenDoor();

    if (currentProgramTime - alertDoorTime >= 8000)
    {
      myDFPlayer.play(4);
      alertDoorTime = currentProgramTime;
    }
  }

  if (typedCharacter)
  {

    if (typedCharacter == '#')
    {
      return;
    }

    if (typedCharacter == '*')
    {
      typedPassword = "";
      typedCharacterCounter = 0;
      timeWithoutActivity = -(hibernationTime);
      return;
    }

    timeWithoutActivity = currentProgramTime;

    if (typedCharacterCounter < LENGTH_OF_PASSWORD)
    {
      typedPassword += typedCharacter;
      typedCharacterCounter++;
    }

    if (typedCharacterCounter != LENGTH_OF_PASSWORD)
    {
      return;
    }

    if (typedPassword == MASTERKEY)
    {
      digitalWrite(RELE, HIGH);
      delay(700);
      digitalWrite(RELE, LOW);

      DeviceSettings.settings();

      typedPassword = "";
      typedCharacterCounter = 0;
      timeWithoutActivity = -(hibernationTime);

      return;
    }

    int feedback = Manager.comparePasswords(typedPassword);

    if (feedback != -1)
    {
      String userAccepted = Manager.getNameUser(feedback);

      Screen.showLoginAccepted(userAccepted);

      digitalWrite(RELE, HIGH);
      myDFPlayer.play(2);
      delay(1000);
      digitalWrite(RELE, LOW);

      wrongPasswordCounter = 0;
      typedPassword = "";
      typedCharacterCounter = 0;
      timeWithoutActivity = -(hibernationTime);
      return;
    }

    Screen.showLoginDenied();
    delay(1000);

    wrongPasswordCounter++;
    myDFPlayer.play(3);

    if (wrongPasswordCounter == 5)
    {
      wrongPasswordManyTimes();
    }

    typedPassword = "";
    typedCharacterCounter = 0;
    timeWithoutActivity = -(hibernationTime);
  }
}