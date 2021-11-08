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

#define BUTTON_UP_PIN A1
#define BUTTON_DOWN_PIN A2
#define LCD_CATHODE_PIN 6

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte up_icon_1[8] = {
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00000
};
byte up_icon_2[8] = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00100,
  0b01010,
  0b10001
};
byte down_icon_1[8] = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
  0b10001,
  0b01010,
  0b00100
};
byte down_icon_2[8] = {
  0b10001,
  0b01010,
  0b00100,
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b00000
};

OneButton button_up(BUTTON_UP_PIN, true, true);
OneButton button_down(BUTTON_DOWN_PIN, true, true);

SleepyDisplay sleepy_display(lcd, LCD_CATHODE_PIN);

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.createChar(0, up_icon_1);
  lcd.createChar(1, up_icon_2);
  lcd.createChar(2, down_icon_1);
  lcd.createChar(3, down_icon_2);
}

void loop() {
  button_up.tick();
  button_down.tick();

  button_up.setPressTicks(0);
  button_down.setPressTicks(0);

  button_up.attachDuringLongPress(animateArrowUp);
  button_up.attachLongPressStop([](){
    clearAnimations();
    sleepy_display.idle(); 
  });

  button_down.attachDuringLongPress(animateArrowDown);
  button_down.attachLongPressStop([](){
    clearAnimations();
    sleepy_display.idle();
  });

  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);

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

static void animateArrowUp() {
  animateArrow(true);
}

static void animateArrowDown() {
  animateArrow(false);
}

static void clearAnimations() {
  lcd.setCursor(0, 0);
  lcd.write(" ");
}
