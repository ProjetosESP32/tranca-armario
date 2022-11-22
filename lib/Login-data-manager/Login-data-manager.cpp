/**
   @file Login-data-manager.cpp
   @author Marcus Vinícius (marcaovini07@gmail.com)
   @brief Arquivo da gerência de logins fechadura
   @version 0.1
   @date 2022-30-10

   @copyright Copyright (c) 2022

*/

#include "Login-data-manager.hpp"
#include "SPIFFS.h"

void LoginData::spiffs_save() {
  File file = SPIFFS.open("/logins.txt", FILE_WRITE);
  for (uint8_t i = 0; i < USERS_LIMIT ; i++) {
    String user_login = user_password[i];
    user_login += ';';
    user_login += String(LoginData::user_validity[i]);
    user_login += ';';
    user_login += LoginData::user_name[i];
    if (user_login != ";0;") {
      file.println(user_login);
      //Serial.println(user_login);
    } else {
      i = USERS_LIMIT;
    }
  }
  file.close();
}
int LoginData::spiffs_read() {
  String buffer_read;
  uint16_t _NUMBER_OF_USERS = 0;

  File file = SPIFFS.open("/logins.txt", FILE_READ);
  while (file.available()) {
    LoginData::user_password[_NUMBER_OF_USERS] = file.readStringUntil(';');
    buffer_read = file.readStringUntil(';');
    LoginData::user_validity[_NUMBER_OF_USERS] = buffer_read.toInt();
    LoginData::user_name[_NUMBER_OF_USERS] = file.readStringUntil('\n');
    _NUMBER_OF_USERS++;
  }
  file.close();
  return _NUMBER_OF_USERS;
}
int LoginData::comparePasswords(String password_typed) {
  int password_number = -1; //retorna -1 se nenhuma senha for encontrada

  for (uint8_t i = 0; i < LoginData::NUMBER_OF_USERS ; i++) {
    if (password_typed.equals(LoginData::user_password[i])) {
      password_number = i;//caso a senha seja encontrada, retorna o id do usário da senha
    }
  }
  return password_number; // retorna o id do usuário se a senha vor válida
}

void LoginData::remove_user(uint8_t id_remove) {
  for (int i = 0; i < LoginData::NUMBER_OF_USERS; i++) {
    if (i >= id_remove) {
      LoginData::user_password[i] = LoginData::user_password[i + 1];
      LoginData::user_validity[i] = LoginData::user_validity[i + 1];
      LoginData::user_name[i] = LoginData::user_name[i + 1];
    }
  }
  NUMBER_OF_USERS--;
}

bool LoginData::managerBegin() {
  bool feedback = true;
  if (!SPIFFS.begin()) {
    feedback = false;
  }
  LoginData::NUMBER_OF_USERS = LoginData::spiffs_read();
  return feedback;
}
int LoginData::getNumberUsers() {
  return NUMBER_OF_USERS;
}
String LoginData::getNameUser(int idUser) {
  return LoginData::user_name[idUser];
}


void LoginData::showUsers() {
  for (int i =0; i < NUMBER_OF_USERS; i++) {
    Serial.print("id user:");
    Serial.print(i);
    Serial.print(" name:");
    Serial.print(LoginData::user_name[i]);
    Serial.print(" Validity:");
    Serial.print(LoginData::user_validity[i]);
    Serial.println();
  }
}

void LoginData::validityHandle(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t day) {
  static int daybefore = -1;
  if (hour == 4 && minutes == 0 && seconds == 0 && day != daybefore) {
    Serial.println("Passou");
    daybefore = day;
    for (int i = 0; i < LoginData::NUMBER_OF_USERS; i++) {
      LoginData::user_validity[i]--;
      if (LoginData::user_validity[i] < 0) {
        LoginData::remove_user(i);
      }
    }
    spiffs_save();
  }
}
int LoginData::getValidityUser(int idUser) {
  return LoginData::user_validity[idUser];
}

bool LoginData::setValidityUser(int idUser, int newVal) {
  LoginData::user_validity[idUser] = newVal;
  spiffs_save();
  return true;
}

bool LoginData::setNewUser(String Name, String Password, int Validity) {
  LoginData::user_name[NUMBER_OF_USERS] = Name;
  LoginData::user_password[NUMBER_OF_USERS] = Password;
  LoginData::user_validity[NUMBER_OF_USERS] = Validity;
  NUMBER_OF_USERS++;
  spiffs_save();
  return true;
}
