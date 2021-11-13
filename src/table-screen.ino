/*
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/
#include <LiquidCrystal.h>
#include <OneButton.h>
#include "SleepyDisplay/SleepyDisplay.h"
#include "emoji.h"

#define BUTTON_UP_PIN A1 //Пин кнопки вверх
#define BUTTON_DOWN_PIN A2 //Пин кнопки вниз
#define BUTTON_PROFILE_PIN A3 //Пин кнопки смены профиля
#define LCD_CATHODE_PIN 6 //Пин катода LCD дисплея

#define TABLE_HEIGHT_MIN 70 //Минимальная высота стола
#define TABLE_HEIGHT_MAX 120 //Максимальная высота стола

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

OneButton button_up(BUTTON_UP_PIN, true, true);
OneButton button_down(BUTTON_DOWN_PIN, true, true);
OneButton button_profile(BUTTON_PROFILE_PIN, true, true);

SleepyDisplay sleepy_display(lcd, LCD_CATHODE_PIN);

enum Modes { Idle, Up, Down, Profile };
byte current_mode = Modes::Idle; //Текущий статус
byte current_profile = 0; //Текущий профиль
byte current_height = 75; //Текущая высота

const int PROFILES [2] = { 70, 120 }; //Профили первого положения

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.createChar(0, up_icon_1);
  lcd.createChar(1, up_icon_2);
  lcd.createChar(2, down_icon_1);
  lcd.createChar(3, down_icon_2);
}

void loop() {
  delay(100);

  button_up.tick();
  button_down.tick();
  button_profile.tick();

  button_up.setPressTicks(0);
  button_down.setPressTicks(0);

  button_up.attachDuringLongPress(holdButtonUp);
  button_up.attachLongPressStop(stopActions);

  button_down.attachDuringLongPress(holdButtonDown);
  button_down.attachLongPressStop(stopActions);

  button_profile.attachClick(switchProfile);

  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
  lcd.print(" Mode: ");
  lcd.print(current_mode);

  if (current_mode == Modes::Profile) {
    followProfile();
  }

  sleepy_display.tick();
}

static void animateArrow(bool up = false) {
  static bool first = true;
  static unsigned long time;

  sleepy_display.action();

  if (millis() - time >= 500) {
    time = millis();
    lcd.setCursor(0, 0);
    if (first) {
      lcd.write(up ? byte(0) : byte(2));
    } else {
      lcd.write(up ? byte(1) : byte(3));
    }
    first = !first;
  }
}

static void moveTable(bool up = false) {
  static unsigned long time = millis();
  animateArrow(up);

  if (millis() - time >= 50) { //Костыль с millis на время разработки без моторчика
      time = millis();
      
      if (up && TABLE_HEIGHT_MAX > current_height) { //Обязательно определяем границы сверху и снизу
        current_height++;
      } else if (TABLE_HEIGHT_MIN < current_height) {
        current_height--;
      }
  }

  char str[7];
  snprintf (str, 7, "%3d", current_height);

  lcd.setCursor(5, 0);
  lcd.print("H");
  lcd.print(str);
}

static void holdButtonUp() {
  current_mode = Modes::Up;
  moveTable(true);
}

static void holdButtonDown() {
  current_mode = Modes::Down;
  moveTable(false);
}

static void clearAnimations() {
  lcd.setCursor(0, 0);
  lcd.write(" ");
}

static void stopActions() {
  clearAnimations();
  sleepy_display.idle();
  current_mode = Modes::Idle;
}

static void switchProfile() {
  sleepy_display.action();

  if (current_profile == sizeof(PROFILES)/sizeof(PROFILES[0]) - 1) { //Если мы уже на последнем профиле
    current_profile = 0;
  } else {
    current_profile += 1;
  }
  lcd.setCursor(2, 0);
  lcd.print("P");
  lcd.print(current_profile);

  current_mode = Modes::Profile;
}

static void followProfile() {
  if (current_height < PROFILES[current_profile]) { //Если мы сейчас ниже нужного профиля - поднимаемся
    moveTable(true);
  } else if (current_height > PROFILES[current_profile]) { //Если мы сейчас выше нужного профиля - опускаемся
    moveTable(false);
  } else { //Если всё окей, то выключаем режим выравнивания в профиль
    stopActions();
  }
}