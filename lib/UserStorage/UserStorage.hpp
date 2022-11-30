#pragma once

#include <stddef.h>

typedef struct user_data
{
  int id;
  char username[25];
  char password[7];
  unsigned long validAt;
  bool isSync;
} UserData;

constexpr size_t SIZE_OF_USERDATA = sizeof(UserData);
constexpr char FILENAME[] = "user_data.bin";

class UserStorage
{
private:
  size_t getNumberOfItems(fs::File &file);

public:
  void setup();
  size_t getNumberOfUsers();
  bool getUserByIndex(size_t index, UserData *data);
  bool getUserById(int id, UserData *data);
  void saveUser(const UserData &user);
  void deleteUserById(int id);
  bool login(const char *username, const char *password);
};
