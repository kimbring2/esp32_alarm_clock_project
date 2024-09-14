#ifndef CLOCK_H
#define CLOCK_H

#include <cstdint>
#include <Arduino.h>
#include "tm1637.h"
#include "alarm_tone.h"

enum ButtonType
{
  BUTTON_MENU,
  BUTTON_PLUS,
  BUTTON_MINUS,
  BUTTON_OK,
};

enum ClockState
{
  SET_1,
  SET_2,
  SET_3,
  SET_4,
  AL_SET_1,
  AL_SET_2,
  AL_SET_3,
  SHOW_TIME,
  AL_ACTIVE,
  SHOW_OFF,
};

class Clock
{
private:
    // 7-segment Display
    TM1637* display = NULL;

    // Timer variable to count time
    hw_timer_t *timer = NULL;

    uint8_t buzzer_pin = NULL;

    // TODO: Add other private variables here
    ClockState clock_state = SHOW_TIME; 

    uint8_t step = 0; 

    uint8_t cur_hour = 0; 
    uint8_t cur_minutes = 0;
    uint8_t cur_seconds = 0;

    bool alarm_state = false;
    uint8_t alarm_hour = 0; 
    uint8_t alarm_minutes = 0;
    uint8_t show_off_seconds = 0;
    uint8_t alarm_on_seconds = 0; 

    bool alarm_off_force = false;
    
public:
    // Constructor
    Clock();

    // Init function
    void init(TM1637* display, uint8_t buzzer_pin);
    
    // Set time and alarm time
    void set_time(uint8_t hour, uint8_t minutes, uint8_t seconds);
    void set_alarm(uint8_t hour, uint8_t minutes);
    void set_step(uint8_t step);

    // Button pressed function
    void button_pressed(ButtonType type);
    
    // Alarm functions
    void turn_alarm(bool on_off);
    void check_alarm();

    // Clock functions 
    void show();
    void run();

    // TODO: Add other public variables/functions here
    hw_timer_t * get_timer() { return timer; };
    ClockState get_clock_state() { return clock_state; }

    uint8_t get_hour() { return cur_hour; }
    uint8_t get_minutes() { return cur_minutes; }
    uint8_t get_seconds() { return cur_seconds; }
    uint8_t get_step() { return step; }

    bool get_alarm_state() { return alarm_state; }
    uint8_t get_alarm_hour() { return alarm_hour; }
    uint8_t get_alarm_minutes() { return alarm_minutes; }

    void show_time();
    void show_alarm();
};

#endif