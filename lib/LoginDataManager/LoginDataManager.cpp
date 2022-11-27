/**
 *  @file LoginDataManager.cpp
 *  @author Marcus Vinícius (marcaovini07@gmail.com)
 *  @brief Arquivo para gerência de logins
 *  @version 0.1
 *  @date 2022-30-10
 *
 */

#include "LoginDataManager.hpp"
#include "SPIFFS.h"

void LoginDataManager::spiffsSave()
{
  File file = SPIFFS.open("/logins.txt", FILE_WRITE);
  for (uint8_t i = 0; i < USERS_LIMIT; i++)
  {
    String userLogin = userPassword[i];
    userLogin += ';';
    userLogin += String(LoginDataManager::userValidity[i]);
    userLogin += ';';
    userLogin += LoginDataManager::userName[i];
    if (userLogin != ";0;")
    {
      file.println(userLogin);
      // Serial.println(userLogin);
    }
    else
    {
      i = USERS_LIMIT;
    }
  }
  file.close();
}
int LoginDataManager::spiffsRead()
{
  String buffer_read;
  uint16_t _numberOfUsers = 0;

  File file = SPIFFS.open("/logins.txt", FILE_READ);
  while (file.available())
  {
    LoginDataManager::userPassword[_numberOfUsers] = file.readStringUntil(';');
    buffer_read = file.readStringUntil(';');
    LoginDataManager::userValidity[_numberOfUsers] = buffer_read.toInt();
    LoginDataManager::userName[_numberOfUsers] = file.readStringUntil('\n');
    _numberOfUsers++;
  }
  file.close();
  return _numberOfUsers;
}
int LoginDataManager::comparePasswords(String passwordTyped)
{
  int passwordNumber = -1; // retorna -1 se nenhuma senha for encontrada

  for (uint8_t i = 0; i < LoginDataManager::numberOfUsers; i++)
  {
    if (passwordTyped.equals(LoginDataManager::userPassword[i]))
    {
      passwordNumber = i; // caso a senha seja encontrada, retorna o id do usário da senha
    }
  }
  return passwordNumber; // retorna o id do usuário se a senha vor válida
}

void LoginDataManager::removeUser(uint8_t idRemove)
{
  for (int i = 0; i < LoginDataManager::numberOfUsers; i++)
  {
    if (i >= idRemove)
    {
      LoginDataManager::userPassword[i] = LoginDataManager::userPassword[i + 1];
      LoginDataManager::userValidity[i] = LoginDataManager::userValidity[i + 1];
      LoginDataManager::userName[i] = LoginDataManager::userName[i + 1];
    }
  }
  numberOfUsers--;
}

bool LoginDataManager::begin()
{
  bool feedback = true;
  if (!SPIFFS.begin())
  {
    feedback = false;
  }
  LoginDataManager::numberOfUsers = LoginDataManager::spiffsRead();
  return feedback;
}
int LoginDataManager::getNumberOfUsers()
{
  return numberOfUsers;
}
String LoginDataManager::getNameUser(int idUser)
{
  return LoginDataManager::userName[idUser];
}

void LoginDataManager::showUsers()
{
  for (int i = 0; i < numberOfUsers; i++)
  {
    Serial.print("id user:");
    Serial.print(i);
    Serial.print(" name:");
    Serial.print(LoginDataManager::userName[i]);
    Serial.print(" Validity:");
    Serial.print(LoginDataManager::userValidity[i]);
    Serial.println();
  }
}

void LoginDataManager::validityHandle(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t day)
{
  static int daybefore = -1;
  if (hour == 4 && minutes == 0 && seconds == 0 && day != daybefore)
  {
    Serial.println("Passou");
    daybefore = day;
    for (int i = 0; i < LoginDataManager::numberOfUsers; i++)
    {
      LoginDataManager::userValidity[i]--;
      if (LoginDataManager::userValidity[i] < 0)
      {
        LoginDataManager::removeUser(i);
      }
    }
    spiffsSave();
  }
}
int LoginDataManager::getValidityUser(int idUser)
{
  return LoginDataManager::userValidity[idUser];
}

bool LoginDataManager::setValidityUser(int idUser, int newVal)
{
  LoginDataManager::userValidity[idUser] = newVal;
  spiffsSave();
  return true;
}

bool LoginDataManager::setNewUser(String Name, String Password, int Validity)
{
  LoginDataManager::userName[numberOfUsers] = Name;
  LoginDataManager::userPassword[numberOfUsers] = Password;
  LoginDataManager::userValidity[numberOfUsers] = Validity;
  numberOfUsers++;
  spiffsSave();
  return true;
}
