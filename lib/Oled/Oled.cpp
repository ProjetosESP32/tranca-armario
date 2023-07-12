/**
 * @file Oled.cpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Facilita a exibição de informações no display
 * @version 0.1
 * @date 2022-24-11
 */

#include <Adafruit_SH1106.h>
#include <Adafruit_GFX.h>
#include "Oled.hpp"


LoginDataManager data;
Adafruit_SH1106 Display(21, 22);

bool Oled::begin(LoginDataManager * data)
{
  Display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  this-> data = data;
  return true;
}
void Oled::showLogo()
{
  Display.clearDisplay();
  Display.setTextColor(WHITE);
  Display.drawBitmap(18, 18, logoIfmt, 93, 27, 1);
  Display.display();
}

int Oled::xCenterStringSize2(String stringCenter)
{

  Display.setTextSize(2);

  stringCenter.trim();
  uint8_t xCenterPosition = 0;
  int length = stringCenter.length();
  int maxCharacterInOneLine = 11;
  if (length > maxCharacterInOneLine)
  {
    xCenterPosition = 0;
  }
  else
  {
    uint8_t pixelsCharacter = 12;
    xCenterPosition = ((11 - length) * pixelsCharacter) / 2;
  }
  return xCenterPosition;
}

void Oled::showSpiffsFail()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(0, 25);
  Display.print("SPIFFS FAIL");

  Display.display();
}

void Oled::showRtcFail()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);

  Display.setCursor(20, 25);
  Display.print("RTC FAIL");

  Display.display();
}

void Oled::showMP3Fail()
{

  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);

  Display.setCursor(20, 25);
  Display.print("MP3 FAIL");

  Display.display();


}

void Oled::showTypedKey(String typedPassword = "")
{
  uint8_t lastCharacterIndex = typedPassword.length() - 1;
  if (lastCharacterIndex >= 0 && lastCharacterIndex <= 6)
  {
    Display.clearDisplay();
    Display.setTextSize(2);
    Display.setTextColor(WHITE);
    Display.setTextWrap(false);

    Display.setCursor(32, 30);

    for (int i = 0; i < lastCharacterIndex; i++)
    {
      Display.print("*");
    }

    Display.print(typedPassword.charAt(lastCharacterIndex));

    Display.display();
    return;
  }
}

void Oled::showTime(uint8_t hour, uint8_t minute, uint8_t second)
{
  char buffer[10];

  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(35, 50);
  sprintf(buffer, "%02d:%02d", hour, minute);
  Display.print(buffer);

  Display.setTextSize(1);
  Display.setCursor(95, 57);
  sprintf(buffer, "%02d", second);
  Display.print(buffer);
}

void Oled::showDate(uint8_t day, uint8_t month, uint16_t year)
{
  Display.setTextColor(WHITE);
  Display.setTextSize(1);
  Display.setTextWrap(false);

  char buffer[15] = "";
  Display.setCursor(35, 0);
  sprintf(buffer, "%02d/%02d/%04d", day, month, year);
  Display.print(buffer);
}

void Oled::showHomeScreen(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint16_t year, String labName)
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  uint8_t xCoordinate = xCenterStringSize2(labName);

  Display.setCursor(xCoordinate, 27);
  Display.print(labName);

  showTime(hour, minute, second);

  showDate(day, month, year);

  Display.display();
}

void Oled::showMenuNames(uint8_t idUserShow, bool resetBuffer)
{
  static int idUserShowBuffer = -1;
  static int startOfVisibleUserBuffer = 0;

  if (resetBuffer)
  {
    idUserShowBuffer = -1;
    startOfVisibleUserBuffer = 0;
    return;
  }
  int startOfVisibleUser = 0;
  int xcursor = 1;

  if (idUserShowBuffer != idUserShow)
  {
    Display.clearDisplay();
    if (idUserShow > 3)
      startOfVisibleUser = idUserShow - 3;
    if (((startOfVisibleUser - startOfVisibleUserBuffer) < 0) && ((startOfVisibleUserBuffer - startOfVisibleUser) <= 3) && idUserShow > 3)
      startOfVisibleUser = startOfVisibleUserBuffer;
    for (int i = startOfVisibleUser; i < startOfVisibleUser + 4; i++)
    {
      if (i == idUserShow)
      {
        Display.fillRect(0, xcursor - 1, 128, 16, WHITE);
        Display.setTextSize(2);
        Display.setCursor(2, xcursor);
        Display.setTextColor(BLACK);
        Display.print((*data).getNameUser(i)); // mudar método
      }
      else
      {
        Display.setTextSize(2);
        Display.setCursor(2, xcursor);
        Display.setTextColor(WHITE);
        Display.print((*data).getNameUser(i)); // mudar método
      }
      xcursor += 16;
    }
    Display.display();
    idUserShowBuffer = idUserShow;
    startOfVisibleUserBuffer = startOfVisibleUser;
  }
}

void Oled::showTypedPassword(String typedPassword)
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  uint8_t lastCharacterIndex = typedPassword.length() - 1;

  uint8_t xPosition = xCenterStringSize2(typedPassword);
  Display.setCursor(xPosition, 30);
  for (int i = 0; i < lastCharacterIndex; i++)
  {
    Display.print("*");
  }
  Display.print(typedPassword.charAt(lastCharacterIndex));

  Display.display();
}

void Oled::showLoginAccepted(String User)
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(15, 10);
  Display.print("Bem Vindo");

  uint8_t xPosition = xCenterStringSize2(User);

  Display.setCursor(xPosition, 27);
  Display.print(User);

  Display.display();
}

void Oled::showLockedLock(String typedPassword = "")
{
  uint8_t lastCharacterIndex = typedPassword.length() - 1;
  if (lastCharacterIndex >= 0 && lastCharacterIndex <= 8)
  {

    Display.clearDisplay();
    Display.setTextSize(2);
    Display.setTextColor(WHITE);
    Display.setTextWrap(false);

    Display.setCursor(0, 10);
    Display.print("!BLOQUEADO!");

    Display.setCursor(0, 27);
    Display.print("Key Segura:");

    Display.setCursor(17, 50);

    for (int i = 0; i < lastCharacterIndex; i++)
    {
      Display.print("*");
    }

    Display.print(typedPassword.charAt(lastCharacterIndex));

    Display.display();
  }
}

void Oled::showLoginDenied()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(35, 15);
  Display.print("SENHA");

  Display.setCursor(15, 32);
  Display.print("INVALIDA");

  Display.display();
}

void Oled::showOpenDoor()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(35, 15);
  Display.print("PORTA");

  Display.setCursor(25, 32);
  Display.print("ABERTA!");

  Display.display();
}

void Oled::showSettingsTitle()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(17, 20);
  Display.print("SETTINGS");
  Display.display();
}

void Oled::showMenuConfig()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(0, 0);
  Display.print("1-Data/Hora");

  Display.setCursor(0, 16);
  Display.print("2-Validade");

  Display.setCursor(0, 32);
  //Display.print("3-New User");
  Display.print("3-Novo Usu");

  Display.setCursor(0, 48);
  //Display.print("4-Del User");
  Display.print("4-Del Usua");

  Display.display();
}
void Oled::showControls()
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(10, 0);
  Display.print("CONTROLES");

  Display.setCursor(10, 16);
  Display.print("2 - Cima");

  Display.setCursor(10, 32);
  Display.print("# - OK");

  Display.setCursor(10, 48);
  Display.print("8 - Baixo");

  Display.display();
}

void Oled::showConfigDate(uint8_t day, uint8_t month, uint16_t year)
{
  char buffer[15] = "";

  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(4, 25);
  sprintf(buffer, "%02d/%02d/%04d", day, month, year);
  Display.print(buffer);

  Display.display();
}

void Oled::showConfigTime(uint8_t hour, uint8_t minute)
{
  char buffer[10] = "";

  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(35, 30);
  sprintf(buffer, "%02d:%02d", hour, minute);
  Display.print(buffer);

  Display.display();
}

void Oled::wrongData()
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  Display.setCursor(0, 30);
  Display.print("Dados Inval");

  Display.display();
}

void Oled::showVirtualKeyboardForSetName(String typedName, uint8_t highLightedKey)
{
  uint8_t sizeLine = 9;
  int xCursor = 0; // coordenada X pra cada caracter
  int yCursor = 0; // coordenada X pra cada caracter

  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(BLACK, WHITE);
  Display.setTextWrap(false);

  Display.setCursor(0, 0);
  Display.print(typedName);

  for (int i = 0; i < NUMBER_OF_CHARACTERS; i++)
  {
    if (xCursor > 112)
      xCursor = 0;

    if (i < sizeLine)
    {
      yCursor = 16;
      Display.setCursor(xCursor, yCursor);
    }
    else if (i < sizeLine * 2)
    {
      yCursor = 32;
      Display.setCursor(xCursor, yCursor);
    }
    else if (i >= sizeLine * 2)
    {
      yCursor = 48;
      Display.setCursor(xCursor, yCursor);
    }

    Display.setTextSize(2);
    if (i == highLightedKey)
    {
      Display.setTextColor(BLACK, WHITE);
    }
    else
    {
      Display.setTextColor(WHITE);
    }

    Display.print(alphabet[i]);
    xCursor += 14;
  }
  Display.display();
}

char Oled::getAlphabetCharacter(uint8_t characterId)
{
  char character = alphabet[characterId];
  return character;
}

void Oled::showSetPassword(String password)
{
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextColor(WHITE);
  Display.setTextWrap(false);

  Display.setCursor(30, 0);
  Display.print("Senha:");

  uint8_t xPosition = xCenterStringSize2(password);
  Display.setCursor(xPosition, 30);
  Display.print(password);

  Display.setTextSize(1);
  Display.setCursor(2, 57);
  Display.print("(*)BackSpace   (#)OK");

  Display.display();
}

void Oled::showSetExpirationDate(String expirationDate)
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  Display.setCursor(10, 0);
  Display.print("Validade:");

  uint8_t xPosition = xCenterStringSize2(expirationDate);
  Display.setCursor(xPosition, 30);
  Display.print(expirationDate);

  Display.setTextSize(1);
  Display.setCursor(2, 57);
  Display.print("(*)backspace   (#)OK");

  Display.display();
}

void Oled::showSetPasswordConfirmation(String password)
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  Display.setCursor(15, 0);
  Display.print("Confirm?");

  uint8_t xPosition = xCenterStringSize2(password);
  Display.setCursor(xPosition, 30);
  Display.print(password);

  Display.setTextSize(1);
  Display.setCursor(2, 57);
  Display.print("(*)Cancelar    (#)OK");

  Display.display();
}

void Oled::showSetExpirationDateConfirmation(String expirationDate)
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  Display.setCursor(15, 0);
  Display.print("Confirm?");

  uint8_t xPosition = xCenterStringSize2(expirationDate);
  Display.setCursor(xPosition, 30);
  Display.print(expirationDate);

  Display.setTextSize(1);
  Display.setCursor(2, 57);
  Display.print("(*)Cancelar    (#)OK");

  Display.display();
}

void Oled::confirmationDeleteUser(String nameUser)
{
  Display.setTextColor(WHITE);
  Display.clearDisplay();
  Display.setTextSize(2);
  Display.setTextWrap(false);

  Display.setCursor(0, 0);
  Display.print("->Deletar<-");

  uint8_t xPosition = xCenterStringSize2(nameUser);
  Display.setCursor(xPosition, 16);
  Display.print(nameUser);

  Display.setCursor(0, 34);
  Display.print("   ?????   ");

  Display.setTextSize(1);
  Display.setCursor(2, 57);
  Display.print("(*)Cancelar    (#)OK");

  Display.display();
}