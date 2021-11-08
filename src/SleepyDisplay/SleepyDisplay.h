#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>

class SleepyDisplay {
  public:
    SleepyDisplay(LiquidCrystal &lcd, int cathode);
    void tick();
    void action();
    void idle();
  protected:
    void _lcdLight(bool on);
    bool _actions;
    int _cathode;
    LiquidCrystal &_lcd;
};

SleepyDisplay::SleepyDisplay(LiquidCrystal &lcd, int cathode): _lcd(lcd), _cathode(cathode) {
  pinMode(cathode, OUTPUT);
}
void SleepyDisplay::tick() {
  static unsigned long time = millis();
  static bool timer = true;

  Serial.write("Actions: ");Serial.print(_actions);
  Serial.println();

  if (!_actions && !timer) {
    timer = true;
    time = millis();
  }
  if (_actions) timer = false;

  if (timer && millis() - time >= 3000) {
    _lcdLight(false);
  } else {
    _lcdLight(true);
  }
}

void SleepyDisplay::action() {
  _actions = true;
}
void SleepyDisplay::idle() {
  _actions = false;
}

void SleepyDisplay::_lcdLight(bool on = true) {
  digitalWrite(_cathode, !on);
}