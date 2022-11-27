#include <Arduino.h>
// Marcus Vinícius -> refatorando e concertando bugs do código

#include "LoginDataManager.hpp"
#include "Oled.hpp"

#include <Keypad.h>
#include "SPIFFS.h"
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_SH1106.h>
#include <Adafruit_GFX.h>

#define SENSOR_PORTA 32
#define RELE 12

const String MASTERKEY = "117811";
const String SEC_KEY = "#9*7#02*";
const int LENGTH_OF_PASSWORD_SEC = 8;

const unsigned int hibernation_time = 20000;
const unsigned long int restart_time = 1210000000; // Duas semanas -> reset para garantir estabilidade
unsigned long tempCompare = 0;
unsigned long tempCompare_door = 0;
unsigned long timeDoorOpen = 0;

String conc_password;
int count_key = 0;
int count_invalidPassword = 0;

const byte linhas = 4;
const byte colunas = 4;
char matrizteclado[linhas][colunas] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte pinoslinhas[linhas] = {4, 18, 19, 33};
byte pinoscolunas[colunas] = {25, 26, 27, 13};

Adafruit_SH1106 oled(21, 22);
Keypad teclado = Keypad(makeKeymap(matrizteclado), pinoslinhas, pinoscolunas, linhas, colunas);
// DFRobotDFPlayerMini myDFPlayer;

LoginDataManager manager;
RTC_DS3231 rtc;

//Display Screen;

void displayDraw_time(uint8_t hour, uint8_t minute, uint8_t second)
{
  char buffer[10];
  oled.setTextSize(2);
  oled.setTextWrap(false);
  oled.setTextColor(WHITE);
  oled.setCursor(35, 50);
  sprintf(buffer, "%02d:%02d", hour, minute);
  oled.print(buffer);
  oled.setCursor(11, 20);
  oled.print("LAB E-117");
  oled.setTextSize(1);
  oled.setCursor(95, 57);
  sprintf(buffer, "%02d", second);
  oled.print(buffer);
}

void displayDraw_date(uint8_t day, uint8_t month, int year)
{
  char buffer[15] = "";
  oled.setTextSize(1);
  oled.setCursor(35, 0);
  sprintf(buffer, "%02d/%02d/%04d", day, month, year);
  oled.print(buffer);
}

void MasterConfig()
{
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setTextWrap(false);
  oled.setCursor(17, 20);
  oled.print("SETTINGS");
  oled.display();
  delay(1500);
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.print("1-Data/Hora");
  oled.setCursor(0, 16);
  oled.print("2-Validade");
  oled.setCursor(0, 32);
  oled.print("3-New User");
  oled.setCursor(0, 48);
  oled.print("");
  oled.display();

  bool isDigit = true;
  char _key = ' ';
  while (isDigit)
  {
    _key = teclado.getKey();
    if (_key)
      isDigit = !isDigit;
  }
  if (_key == '1') // A biblioteca keypad retorna 48+(numero da tecla) -> logo se digitado 1 o valor é 48 + 1 = 49
  {
    bool isFinalProcess = true;
    int i_year = 0,
        i_month = 0,
        i_day = 0,
        i_hour = 0,
        i_minute = 0;
    String s_year = "",
           s_month = "",
           s_day = "",
           s_hour = "",
           s_minute = "";
    char conc[] = "";
    int count = 0;

    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setCursor(4, 30);
    oled.print("00/00/0000");
    oled.display();
    while (isFinalProcess)
    {
      char buffer[15] = "";
      _key = teclado.getKey();
      if (_key)
      {
        if (count < 2)
        {
          s_day += _key;
          count++;
        }
        else if (count < 4)
        {
          s_month += _key;
          count++;
        }
        else if (count < 8)
        {
          s_year += _key;
          count++;
          if (count == 8)
            isFinalProcess = !isFinalProcess;
        }
        i_day = s_day.toInt();
        i_month = s_month.toInt();
        i_year = s_year.toInt();

        i_month = min(i_month, 12);
        i_day = min(i_day, 31);

        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setCursor(4, 30);
        sprintf(buffer, "%02d/%02d/%04d", i_day, i_month, i_year);
        oled.print(buffer);
        oled.display();
      }
    }

    delay(1000);
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setCursor(35, 30);
    oled.print("00:00");
    oled.display();
    isFinalProcess = true;
    count = 0;

    while (isFinalProcess)
    {
      char buffer[8] = "";

      _key = teclado.getKey();
      if (_key)
      {
        if (count < 2)
        {
          s_hour += _key;
          count++;
        }
        else if (count < 4)
        {
          s_minute += _key;
          count++;

          if (count == 4)
            isFinalProcess = !isFinalProcess;
        }
        i_hour = s_hour.toInt();
        i_minute = s_minute.toInt();

        i_hour = min(i_hour, 23);
        i_minute = min(i_minute, 59);

        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setCursor(35, 30);
        sprintf(buffer, "%02d:%02d", i_hour, i_minute);
        oled.print(buffer);
        oled.display();
      }
    }

    delay(1000);
    /*i_month = min(i_month, 12);
      i_day = min(i_day, 31);
      i_hour = min(i_hour, 23);
      i_minute = min(i_minute, 59);*/
    rtc.adjust(DateTime(i_year, i_month, i_day, i_hour, i_minute, 0));
  }
  if (_key == '2')
  {
    int number_users = manager.getNumberOfUsers();
    String user[number_users];
    bool isConfigured = true;

    int ID_user = 0;
    int ID_userbuffer = -1;

    for (int index_users = 0; index_users < number_users; index_users++)
    {
      user[index_users] = manager.getNameUser(index_users);
    }

    // oled.setTextColor(WHITE);
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextWrap(false);
    oled.setCursor(16, 0);
    oled.print("CONTROLES");
    oled.setCursor(10, 16);
    oled.print("2 - Cima");
    oled.setCursor(10, 32);
    oled.print("# - OK");
    oled.setCursor(10, 48);
    oled.print("8 - Baixo");
    oled.display();

    delay(5000);

    while (isConfigured)
    {
      _key = teclado.getKey();

      if (_key == '2')
      {
        if (ID_user > 0)
          ID_user--;
      }
      if (_key == '*')
      {
        isConfigured = !isConfigured;
      }

      if (_key == '8')
      {
        if (ID_user < (number_users - 1))
          ID_user++;
      }

      if (_key == '#')
      {

        int count = 0;
        String number = "";

        int Val = manager.getValidityUser(ID_user);
        char buffer[5] = "";
        bool isSet = true;

        while (isSet)
        {

          _key = teclado.getKey();

          oled.clearDisplay();
          oled.setTextSize(2);
          oled.setTextWrap(false);
          oled.setTextColor(WHITE);
          oled.setCursor(17, 0);
          oled.print("VALIDADE");
          oled.setCursor(10, 30);
          sprintf(buffer, "   %03d    ", Val);
          oled.print(buffer);
          oled.setTextSize(1);
          oled.setCursor(2, 57);
          oled.display();
          if (_key == '#')
          {
            isSet = !isSet;
          }
          if (_key)
          {
            if (_key != '#' && _key != '*')
            {

              if (count < 3)
              {
                count++;
                number += _key;
                Val = number.toInt();
              }
              else
              {
                count = 0;
                number = "";
                Val = number.toInt();
              }
            }
          }
          manager.setValidityUser(ID_user, Val);
          ID_userbuffer = -1;
        }
      }
      int number_users = number_users; // Número total de páginas
      bool Open_user;
      int users_shows_buffer;
      int xcursor = 1;
      int users_shows = 0;

      if (ID_userbuffer != ID_user)
      {
        oled.clearDisplay();
        if (ID_user > 3)
          users_shows = ID_user - 3;
        if (((users_shows - users_shows_buffer) < 0) && ((users_shows_buffer - users_shows) <= 3) && ID_user > 3)
          users_shows = users_shows_buffer;
        for (int i = users_shows; i < users_shows + 4; i++)
        {
          if (i == ID_user)
          {
            oled.fillRect(0, xcursor - 1, 128, 16, WHITE);
            oled.setTextSize(2);
            oled.setCursor(2, xcursor);
            oled.setTextColor(BLACK);
            oled.print(user[i]);
          }
          else
          {
            oled.setTextSize(2);
            oled.setCursor(2, xcursor);
            oled.setTextColor(WHITE);
            oled.print(user[i]);
          }
          xcursor += 16;
        }
        oled.display();
        ID_userbuffer = ID_user;
        users_shows_buffer = users_shows;
      }
    }
  }
  if (_key == '3')
  {
    int number_character = 27;
    int line_size = 9;
    int pixels_character = 10;
    int count_key = 0;
    char alphabet[number_character] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
        'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
        's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '<'};
    bool isSetName = true;

    int cursor = 0;
    int cursor_x = 0;
    int cursor_y = 0;
    int cursor_yfill = 0;
    int cursor_xfill = 0;

    String name = "";
    int count_string = 0;

    while (isSetName)
    {
      _key = teclado.getKey();

      if (_key == '2')
      {
        cursor -= line_size;
        cursor = max(cursor, 0);
      }
      if (_key == '4')
      {
        cursor--;
      }
      if (_key == '6')
      {
        cursor++;
      }
      if (_key == '8')
      {
        cursor += line_size;
        cursor = min(cursor, number_character - 1);
      }
      if (_key == '5')
      {
        if (alphabet[cursor] == '<')
        {
          name.remove(count_string);
          count_string--;
        }
        else
        {
          count_string++;
          name += alphabet[cursor];
        }
      }
      if (_key == '#')
      {
        bool isSetPassword = true;
        bool isSetValidity = true;

        String password = "";
        String validity_s = "";

        int count_password = 0;
        int count_validity = 0;

        int validity_i = 0;
        while (isSetPassword)
        {
          _key = teclado.getKey();
          if (_key)
          {
            if (_key == '*')
            {
              password.remove(count_password);
              count_password--;
              count_password = max(count_password, 0);
            }

            if (_key != '*' && _key != '#')
            {
              password += _key;
              count_password++;
            }
            if (count_password == LENGTH_OF_PASSWORD)
            {
              isSetPassword = !isSetPassword;
            }
          }

          oled.clearDisplay();
          oled.setTextSize(2);
          oled.setTextWrap(false);
          oled.setTextColor(WHITE);
          oled.setCursor(30, 0);
          oled.print("Senha:");
          oled.setCursor(30, 30);
          oled.print(password);
          oled.setTextSize(1);
          oled.setCursor(2, 57);
          oled.print("(*)BackSpace   (#)OK");
          oled.display();
        }

        while (isSetValidity)
        {
          _key = teclado.getKey();
          if (_key)
          {
            if (_key == '*')
            {
              validity_s.remove(count_validity);
              count_validity--;
              count_validity = max(count_validity, 0);
            }

            if (_key != '*' && _key != '#')
            {
              validity_s += _key;
              count_validity++;
            }
            if (_key == '#')
            {
              validity_i = validity_s.toInt();
              manager.setNewUser(name, password, validity_i);
              isSetValidity = !isSetValidity;
              return;
            }
          }
          oled.setTextColor(WHITE);
          oled.clearDisplay();
          oled.setTextSize(2);
          oled.setTextWrap(false);
          oled.setCursor(20, 0);
          oled.print("Validade:");
          oled.setCursor(40, 30);
          oled.print(validity_s);
          oled.setTextSize(1);
          oled.setCursor(2, 57);
          oled.print("(*)backspace   (#)OK");
          oled.display();
        }
      }

      oled.clearDisplay();
      oled.setCursor(0, 0);
      oled.setTextSize(2);
      oled.setTextColor(BLACK, WHITE);
      oled.print(name);

      for (int i = 0; i < number_character; i++)
      {
        if (cursor_x > 112)
          cursor_x = 0;

        if (i < 9)
        {
          cursor_y = 16;
          oled.setCursor(cursor_x, cursor_y);
        }
        else if (i < 18)
        {
          cursor_y = 32;
          oled.setCursor(cursor_x, cursor_y);
        }
        else if (i >= 18)
        {
          cursor_y = 48;
          oled.setCursor(cursor_x, cursor_y);
        }

        oled.setTextSize(2);
        if (i == cursor)
        {
          oled.setTextColor(BLACK, WHITE);
        }
        else
        {
          oled.setTextColor(WHITE);
        }

        oled.print(alphabet[i]);
        cursor_x += 14;
      }
      oled.display();
    }
  }
}

void setup()
{
  oled.begin(SH1106_SWITCHCAPVCC, 0x3C);

  //Screen.begin();
  //Screen.showLogo();

  Serial.begin(115200);
  // oled.clearDisplay();
  // oled.drawBitmap(18, 18, logo_ifmt, 93, 27, 1);
  // oled.display();
  // delay(2000);

  if (!manager.begin())
  {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(5, 20);
    oled.print("ROM FALHOU");
    oled.display();
    while (1);
  }
  manager.showUsers(); // remover no código principal

  if (!rtc.begin())
  {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(5, 20);
    oled.print("RTC FALHOU");
    oled.display();
    while (1)
      ;
  }
  /*if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }*/

  rtc.disable32K();

  pinMode(RELE, OUTPUT);
  pinMode(SENSOR_PORTA, INPUT);

  /*oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
    oled.setCursor(10, 20);
    oled.print("FECHADURA");
    oled.display();*/

  DateTime now = rtc.now();
  uint8_t horas = now.hour();
  uint8_t minutos = now.minute();
  uint8_t segundos = now.second();

  /*
    PlayerMP3.begin(9600, SERIAL_8N1, 3, 1);
    if (!myDFPlayer.begin(PlayerMP3))
    {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(10, 10);
    oled.print("Falha Player MP3");
    oled.display();
    delay(3000);
    } else {
    //Definicoes iniciais MP3
    myDFPlayer.setTimeOut(500); //Timeout serial 500ms
    myDFPlayer.volume(30); //Volume
    myDFPlayer.EQ(0); //Equalizacao normal
    myDFPlayer.stop();
    }

    myDFPlayer.play(1);

    oled.clearDisplay();
    oled.display();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(10, 15);
    oled.print("Iniciando o sistema");
    oled.display();
    delay(1000);
  */
}
void loop()
{
  DateTime now = rtc.now();
  uint8_t horas = now.hour();
  uint8_t minutos = now.minute();
  uint8_t segundos = now.second();
  uint16_t ano = now.year();
  uint8_t mes = now.month();
  uint8_t dia = now.day();
  unsigned long int Global_time = millis();

  if (Global_time >= restart_time)
    ESP.restart();

  manager.validityHandle(horas, minutos, segundos, dia);

  char Key = teclado.getKey();
  if (Key)
  {
    if (Key == '#' || Key == '*')
      return;
    tempCompare = Global_time;

    if (count_key < LENGTH_OF_PASSWORD)
    {
      conc_password += Key;
      count_key++;
    }
    if (count_key == LENGTH_OF_PASSWORD)
    {

      if (conc_password == MASTERKEY)
      {
        MasterConfig();
        conc_password = "";
        count_key = 0;
        tempCompare = 0;
        return;
      }
      int feedback = manager.comparePasswords(conc_password);
      if (feedback != -1)
      {
        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setTextColor(WHITE);
        oled.setCursor(15, 10);
        oled.print("Bem Vindo");
        oled.setTextWrap(false);
        String buffer = "";
        buffer = manager.getNameUser(feedback);
        buffer.trim();
        int length = buffer.length();
        int maxCharacter_line = 11;
        // oled.setCursor(60, 50);
        // oled.print(length);
        if (length > maxCharacter_line)
        {
          oled.setCursor(0, 27);
        }
        else
        {
          int pixelsCharacter = 12;
          int centerPosition;
          centerPosition = ((11 - length) * pixelsCharacter) / 2;
          oled.setCursor(centerPosition, 27);
        }
        oled.print(buffer);
        oled.display();
        oled.setTextWrap(false);
        digitalWrite(RELE, HIGH);
        delay(1500);
        digitalWrite(RELE, LOW);
        count_invalidPassword = 0;
      }
      else
      {
        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setTextColor(WHITE);
        oled.setCursor(35, 15);
        oled.print("SENHA");
        oled.setCursor(15, 32);
        oled.print("INVALIDA");
        oled.display();
        delay(1000);
        count_invalidPassword++;
      }
      conc_password = "";
      count_key = 0;
      tempCompare = 0;

      if (count_invalidPassword == 5)
      {
        bool isRecovery = true;
        while (isRecovery)
        {
          Key = teclado.getKey();
          if (Key)
          {
            if (count_key < LENGTH_OF_PASSWORD_SEC)
            {
              conc_password += Key;
              count_key++;
            }
            else
            { //(count_key == LENGTH_OF_PASSWORD_SEC)
              conc_password = "";
              count_key = 0;
            }
          }
          oled.clearDisplay();
          oled.setTextSize(2);
          oled.setTextWrap(false);
          oled.setTextColor(WHITE);
          oled.setCursor(15, 10);
          oled.print("BLOQUEADO");
          oled.setCursor(0, 27);
          oled.print("Key Segura:");
          oled.setTextSize(2);
          oled.setTextColor(WHITE);
          oled.setCursor(17, 50);
          // for (int i = 0; i < count_key; i++) {
          oled.print(conc_password);
          //}
          oled.display();
          if (conc_password == SEC_KEY)
          {
            isRecovery = !isRecovery;
            count_invalidPassword = 0;
          }
        }
      }
    }
  }

  if (digitalRead(SENSOR_PORTA) == LOW)
  {
    tempCompare_door = Global_time;
    if (tempCompare_door - timeDoorOpen >= 30000)
    {
      oled.clearDisplay();
      oled.setTextSize(2);
      oled.setTextColor(WHITE);
      oled.setCursor(35, 15);
      oled.print("PORTA");
      oled.setCursor(25, 32);
      oled.print("ABERTA!");
      oled.display();
      delay(1000);
      while (digitalRead(SENSOR_PORTA) == LOW)
      {
        delay(1);
      }
    }
  }
  else
  {
    timeDoorOpen = Global_time;
  }

  if (Global_time - tempCompare >= hibernation_time)
  {
    count_key = 0;
    conc_password = "";
    oled.clearDisplay();
    displayDraw_date(dia, mes, ano);
    displayDraw_time(horas, minutos, segundos);
    oled.display();
  }
  else
  {
    oled.setTextSize(2);
    oled.clearDisplay();
    oled.setTextColor(WHITE);
    oled.setCursor(0, 25);
    oled.print("KEY:");
    conc_password.trim();
    int length = conc_password.length();
    for (int i = 0; i < length; i++)
      oled.print("*");
    oled.display();
  }
}