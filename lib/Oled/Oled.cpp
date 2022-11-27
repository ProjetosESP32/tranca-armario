/**
 * @file Oled.cpp
 * @author Marcus Vinícius (marcaovini07@gmail.com)
 * @brief Facilita a exibição de informações no display
 * @version 0.1
 * @date 2022-24-11
 */
 

#include "Oled.hpp"

bool Oled::begin()
{
    Oled.begin(SH1106_SWITCHCAPVCC, 0x3C);
    return true;
}
void Oled::showLogo()
{
    Oled.clearDisplay();
    Oled.drawBitmap(18, 18, logoIfmt, 93, 27, 1);
    Oled.display();
    delay(2000);
}
void Oled::showTime(uint8_t second, uint8_t minute, uint8_t hour)
{
    
}
void Oled::showDate(uint8_t day, uint8_t month, uint16_t year)
{
}
void Oled::homeScreen(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year, String labName)
{
}
void Oled::showMenuNames(uint8_t numberUsers, uint8_t idUserShow)
{
}
void Oled::showMenuConfig()
{
}
void Oled::showConfigDate()
{
}
