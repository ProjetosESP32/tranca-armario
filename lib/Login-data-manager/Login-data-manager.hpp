/**
 * @file Login-data-manager.cpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Arquivo da gerência de logins fechadura
 * @version 0.1
 * @date 2022-30-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Arduino.h>
#ifndef _LOGINDATAMANAGER_HPP_
#define _LOGINDATAMANAGER_HPP_

constexpr int USERS_LIMIT = 100;
#define LENGTH_OF_PASSWORD 6
class LoginData
{
  private:
    String user_name[100], user_password[100];
    int user_validity[100];
    uint8_t NUMBER_OF_USERS;
    uint8_t number_users_saved;
    
    int spiffs_read();
    void remove_user(uint8_t id_remove);

  public:
    bool managerBegin();
    void spiffs_save();
    void subtractorValidity();
    int comparePasswords(String password_typed);

    bool setNewUser(String Name, String Password, int Validity);
    bool setValidityUser(int idUser, int newVal);
    

    int getNumberUsers();
    String getNameUser(int idUser);
    int getValidityUser(int idUser);
    
    void showUsers();// debugging
    
    void validityHandle(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t day);
};
#endif
