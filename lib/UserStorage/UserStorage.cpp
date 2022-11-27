#include "UserStorage.hpp"
#include <SPIFFS.h>

void UserStorage::saveUser(const UserData &user)
{
  auto file = SPIFFS.open(FILENAME, "rwb");
  auto fileSize = file.size();

  if (fileSize > 0)
  {
    bool isPositionSet = false;
    auto numberOfItens = fileSize / SIZE_OF_USERDATA;

    for (int i = 0; i < numberOfItens; i++)
    {
      UserData data;
      file.readBytes((char *)&data, SIZE_OF_USERDATA);

      if (data.id == -1 || data.id == user.id)
      {
        file.seek(i * SIZE_OF_USERDATA, SeekSet);
        isPositionSet = true;
        break;
      }
    }

    if (!isPositionSet)
    {
      file.seek(0, SeekEnd);
    }
  }

  file.write((uint8_t *)std::addressof(user), SIZE_OF_USERDATA);
  file.close();
}

void UserStorage::deleteUserById(const int id)
{
  auto file = SPIFFS.open(FILENAME, "rwb");
  auto fileSize = file.size();

  if (!file.available() || fileSize == 0)
  {
    file.close();
    return;
  }

  UserData cleanData = {.id = -1};
  auto numberOfItems = fileSize / SIZE_OF_USERDATA;

  for (int i = 0; i < numberOfItems; i++)
  {
    UserData readData;
    file.readBytes((char *)&readData, SIZE_OF_USERDATA);

    if (readData.id == id)
    {
      file.seek(i * SIZE_OF_USERDATA);
      file.write((uint8_t *)&cleanData, SIZE_OF_USERDATA);
      break;
    }
  }

  file.close();
}

bool UserStorage::getUserById(const int id, UserData *_data)
{
  auto file = SPIFFS.open(FILENAME, "rb");
  auto fileSize = file.size();

  if (!file.available() || fileSize == 0)
  {
    file.close();
    return false;
  };

  UserData data;
  auto numberOfItems = fileSize / SIZE_OF_USERDATA;

  for (int i = 0; i < numberOfItems; i++)
  {
    file.readBytes((char *)&data, SIZE_OF_USERDATA);

    if (data.id == id)
    {
      *_data = data;
      file.close();
      return true;
    }
  }

  file.close();
  return false;
}
