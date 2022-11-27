#pragma once

#include <stddef.h>

typedef struct user_data
{
  int id;
  char username[25];
  char password[7];
  unsigned long validAt;
} UserData;

constexpr size_t SIZE_OF_USERDATA = sizeof(UserData);
constexpr char FILENAME[] = "user_data.bin";

class UserStorage
{
public:
  void saveUser(const UserData &user);
  bool getUserById(const int id, UserData *data);
  void deleteUserById(const int id);
};
