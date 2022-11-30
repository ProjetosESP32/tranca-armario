#include "UserStorage.hpp"
#include <SPIFFS.h>
#include <stddef.h>
#include <string.h>

size_t UserStorage::getNumberOfItems(fs::File &file)
{
  auto fileSize = file.size();

  return fileSize / SIZE_OF_USERDATA;
}

void UserStorage::setup()
{
  SPIFFS.begin(true);
}

size_t UserStorage::getNumberOfUsers()
{
  auto file = SPIFFS.open(FILENAME, "rb");

  return getNumberOfItems(file);
}

bool UserStorage::getUserByIndex(size_t index, UserData *_data)
{
  auto file = SPIFFS.open(FILENAME, "rb");
  auto numberOfItems = this->getNumberOfItems(file);
  auto expectedMinSize = (index + 1) * SIZE_OF_USERDATA;

  if (!file.available() || numberOfItems == 0 || numberOfItems < expectedMinSize)
  {
    file.close();
    return false;
  };

  UserData data;

  file.seek(index * SIZE_OF_USERDATA, SeekSet);
  file.readBytes((char *)&data, SIZE_OF_USERDATA);

  if (data.id == -1)
  {
    file.close();
    return false;
  }

  *_data = data;
  file.close();
  return true;
}

bool UserStorage::getUserById(int id, UserData *_data)
{
  auto file = SPIFFS.open(FILENAME, "rb");
  auto numberOfItems = this->getNumberOfItems(file);

  if (!file.available() || numberOfItems == 0)
  {
    file.close();
    return false;
  };

  UserData data;

  for (size_t i = 0; i < numberOfItems; i++)
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

void UserStorage::saveUser(const UserData &user)
{
  auto file = SPIFFS.open(FILENAME, "rwb", true);
  auto numberOfItems = this->getNumberOfItems(file);

  if (numberOfItems > 0)
  {
    bool isPositionSet = false;
    UserData data;

    for (size_t i = 0; i < numberOfItems; i++)
    {
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

void UserStorage::deleteUserById(int id)
{
  auto file = SPIFFS.open(FILENAME, "rwb");
  auto numberOfItems = this->getNumberOfItems(file);

  if (!file.available() || numberOfItems == 0)
  {
    file.close();
    return;
  }

  UserData cleanData = {.id = -1};

  for (size_t i = 0; i < numberOfItems; i++)
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

bool UserStorage::login(const char *username, const char *password)
{
  auto file = SPIFFS.open(FILENAME, "rb");
  auto numberOfItems = this->getNumberOfItems(file);

  if (!file.available() || numberOfItems == 0)
  {
    file.close();
    return false;
  };

  UserData data;

  for (size_t i = 0; i < numberOfItems; i++)
  {
    file.readBytes((char *)&data, SIZE_OF_USERDATA);

    bool isValid = strcmp(username, data.username) == 0 && strcmp(password, data.password);

    if (isValid)
    {
      file.close();
      return true;
    }
  }

  file.close();
  return false;
}
