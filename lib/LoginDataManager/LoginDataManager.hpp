/**
 * @file LogiDataManager.cpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Arquivo para gerência de logins
 * @version 0.1
 * @date 2022-22-11
 *
 */

#include <Arduino.h>
#ifndef _LOGINDATAMANAGER_HPP_
#define _LOGINDATAMANAGER_HPP_

constexpr int USERS_LIMIT = 100;
constexpr int LENGTH_OF_PASSWORD = 6;
class LoginDataManager
{
private:
  String userName[100], userPassword[100];
  int userValidity[100];
  uint8_t numberOfUsers;
  uint8_t numberOfUsersSaved;

  int spiffsRead();
  void removeUser(uint8_t id_remove);

public:
  bool begin();
  void spiffsSave();
  void subtractorValidity();
  int comparePasswords(String passwordTyped);

  bool setNewUser(String name, String password, int validity);
  bool setValidityUser(int idUser, int newVal);

  int getNumberOfUsers();
  String getNameUser(int idUser);
  int getValidityUser(int idUser);

  void showUsers(); // debugging

  void validityHandle(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t day);
};
#endif
