
#include "clock.h"
#include "stdio.h"

// Static function: Update time, show things on display
//                  and check alarm trigger
static void update_time(void* clock)
{
}

// Empty constructor
Clock::Clock() {
}

// Clock::init(): Initialize internal variables,
//                set display to use and buzzer pin
void Clock::init(TM1637* display, uint8_t buzzer_pin)
{ 
  this->display = display;
  this->buzzer_pin = buzzer_pin;
  this->timer = timerBegin(500000); // TIMEER 0-3,PRESCALR, COUNER UP ?
}

// Clock::set_time(): Set the time hour, minutes and seconds
//                    to internal binary representation
void Clock::set_time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  this->cur_hour = hour; 
  this->cur_minutes = minutes;
  this->cur_seconds = seconds;
}

// Clock::set_alarm(): Set the alarm hour, minutes and seconds
//                     to internal binary representation
void Clock::set_alarm(uint8_t hour, uint8_t minutes)
{
  this->alarm_hour = hour; 
  this->alarm_minutes = minutes;
}

void Clock::set_step(uint8_t step)
{
  this->step = step; 
}

// Clock::menu_pressed(): Tell the internal clock that a button was pressed
//                        type: The button that was pressed
void Clock::button_pressed(ButtonType type) 
{
  //Serial.println("button_pressed()");

  uint8_t cur_hour = get_hour();
  uint8_t cur_minutes = get_minutes();
  uint8_t cur_seconds = get_seconds();
  uint8_t cur_step = get_step();

  uint8_t cur_alarm_hour = get_alarm_hour();
  uint8_t cur_alarm_minutes = get_alarm_minutes();

  if (type == BUTTON_MENU) {
    if (clock_state == SHOW_TIME) {
      this->clock_state = SET_1;
    } else if (clock_state == SET_1) {
      this->clock_state = AL_SET_1;
    }
  } else if (type == BUTTON_OK) {
    if (clock_state == SET_1) {
      this->clock_state = SET_2;
    } else if (clock_state == SET_2) {
      this->clock_state = SET_3;
    } else if (clock_state == SET_3) {
      this->clock_state = SHOW_TIME;
    } else if (clock_state == AL_SET_1) {
      if (!get_alarm_state()) {
        this->clock_state = SHOW_OFF;
        show_off_seconds = 4;
      } else {
        this->clock_state = AL_SET_2;
      }
    } else if (clock_state == AL_SET_2) {
      this->clock_state = AL_SET_3;
    } else if (clock_state == AL_SET_3) {
      this->clock_state = SHOW_TIME;
    } else if (clock_state == AL_ACTIVE) {
      tone(buzzer_pin, 0);
      clock_state = SHOW_TIME;
      alarm_off_force = true;
    }
  } else if (type == BUTTON_PLUS) {
    if (clock_state == SET_2) {
      if (cur_hour < 24) {
        set_time(cur_hour + 1, cur_minutes, cur_seconds);
      } else {
        set_time(0, cur_minutes, cur_seconds);
      }
    } else if (clock_state == SET_3) {
      if (cur_minutes < 60) {
        set_time(cur_hour, cur_minutes + 1, cur_seconds);
      } else {
        set_time(cur_hour, 0, cur_seconds);
      }
    } else if (clock_state == AL_SET_2) {
      if (cur_alarm_hour < 24) {
        set_alarm(cur_alarm_hour + 1, cur_alarm_minutes);
      } else {
        set_alarm(0, cur_alarm_minutes);
      }
    } else if (clock_state == AL_SET_3) {
      if (cur_alarm_minutes < 60) {
        set_alarm(cur_alarm_hour, cur_alarm_minutes + 1);
      } else {
        set_alarm(cur_alarm_hour, 0);
      }
    }
  } else if (type == BUTTON_MINUS) {
    if (clock_state == SET_2) {
      if (cur_hour > 0) {
        set_time(cur_hour - 1, cur_minutes, cur_seconds);
      } else {
        set_time(24, cur_minutes, cur_seconds);
      }
    } else if (clock_state == SET_2) {
      if (cur_minutes > 0) {
        set_time(cur_hour, cur_minutes - 1, cur_seconds);
      } else {
        set_time(cur_hour, 60, cur_seconds);
      }
    } else if (clock_state == AL_SET_2) {
      if (cur_alarm_hour > 0) {
        set_alarm(cur_alarm_hour - 1, cur_alarm_minutes);
      } else {
        set_alarm(24, cur_alarm_minutes);
      }
    } else if (clock_state == AL_SET_3) {
      if (cur_alarm_minutes > 0) {
        set_alarm(cur_alarm_hour, cur_alarm_minutes - 1);
      } else {
        set_alarm(cur_alarm_hour, 60);
      }
    }
  }
}

// Clock::turn_alarm(): Enable or disable alarm
void Clock::turn_alarm(bool on_off)
{
  //Serial.println("turn_alarm");
  alarm_state = on_off;
}

void Clock::show_time() {
  int8_t disp_data[4] = {'0','0','0','0'};
    if (cur_hour > 10) {
      disp_data[0] = cur_hour / 10 + '0';
      disp_data[1] = cur_hour % 10 + '0';
    } else {
      disp_data[0] = '0';
      disp_data[1] = cur_hour;
    }

    if (cur_minutes > 10) {
      disp_data[2] = cur_minutes / 10 + '0';
      disp_data[3] = cur_minutes % 10 + '0';
    } else {
      disp_data[2] = '0';
      disp_data[3] = cur_minutes;
    }

    this->display->display(disp_data);
}


void Clock::show_alarm() {
  int8_t disp_data[4] = {'0','0','0','0'};
    if (alarm_hour > 10) {
      disp_data[0] = alarm_hour / 10 + '0';
      disp_data[1] = alarm_hour % 10 + '0';
    } else {
      disp_data[0] = '0';
      disp_data[1] = alarm_hour;
    }

    if (alarm_minutes > 10) {
      disp_data[2] = alarm_minutes / 10 + '0';
      disp_data[3] = alarm_minutes % 10 + '0';
    } else {
      disp_data[2] = '0';
      disp_data[3] = alarm_minutes;
    }

    this->display->display(disp_data);
}


// Clock::show(): Show time or menu on display
void Clock::show()
{   
  if ( (clock_state == SHOW_TIME) or (clock_state == AL_ACTIVE) ) {
    show_time();

    if (step % 2 == 0) {
      this->display->point(true);
    } else {
      this->display->point(false);
    }
  } else if (clock_state == SET_1) {
    this->display->displayStr("SET");
  } else if (clock_state == SET_2) {
    this->display->point(true);
    if (step % 2 == 0) {
      show_time();
    } else {
      this->display->display(0x00, 0x7f);
      this->display->display(0x01, 0x7f);
    }
  } else if (clock_state == SET_3) {
    this->display->point(true);
    if (step % 2 == 0) {
      show_time();
    } else {
      this->display->display(0x02, 0x7f);
      this->display->display(0x03, 0x7f);
    }
  } else if (clock_state == SHOW_OFF) {
    this->display->displayStr("OFF");
  }
  
  else if (clock_state == AL_SET_1) {
    this->display->displayStr("AL");
  } else if (clock_state == AL_SET_2) {
    this->display->point(true);
    if (step % 2 == 0) {
      show_alarm();
    } else {
      this->display->display(0x00, 0x7f);
      this->display->display(0x01, 0x7f);
    }
  } else if (clock_state == AL_SET_3) {
    this->display->point(true);
    if (step % 2 == 0) {
      show_alarm();
    } else {
      this->display->display(0x02, 0x7f);
      this->display->display(0x03, 0x7f);
    }
  } 
}

// Clock::check_alarm(): Check if alarm needs to be triggered
void Clock::check_alarm()
{
  //Serial.print("clock_state: ");
  //Serial.print(clock_state);

  //Serial.print(", cur_hour: ");
  //Serial.print(cur_hour);

  //Serial.print(", cur_minutes: ");
  //Serial.print(cur_minutes);

  //Serial.print(", cur_seconds: ");
  //Serial.print(cur_seconds);

  //Serial.print(", alarm_state: ");
  //Serial.print(alarm_state);

  //Serial.print(", alarm_hour: ");
  //Serial.print(alarm_hour);

  //Serial.print(", alarm_minutes: ");
  //Serial.print(alarm_minutes);

  //Serial.print(", alarm_on_seconds: ");
  //Serial.println(alarm_on_seconds);

  if ( (cur_hour == alarm_hour) and (cur_minutes == alarm_minutes) ) {
    if ( (clock_state == SHOW_TIME) and (alarm_state == true) and (alarm_off_force == false) ) {
      //Serial.println("clock_state = AL_ACTIVE");
      clock_state = AL_ACTIVE;
      alarm_on_seconds = 5;
    }
  } else {
    alarm_off_force = false;
  }

  if (clock_state == AL_ACTIVE) {
    tone(buzzer_pin, 500);
  }

  //Serial.print(", show_off_seconds : ");
  //Serial.println(show_off_seconds);
  if (show_off_seconds > 1) { 
    show_off_seconds -= 1;
  } else if (show_off_seconds == 1) {
    clock_state = SHOW_TIME;
    show_off_seconds = 0;
  }

  if (alarm_on_seconds > 1) { 
    alarm_on_seconds -= 1;
  } else if (alarm_on_seconds == 1) {
    tone(buzzer_pin, 0);
    clock_state = SHOW_TIME;
    alarm_on_seconds = 0;
    alarm_off_force = true;
  }
}

// Clock::run(): Start running the clock
//               This function MUST not block, everything should be handled
//               by interrupts
void Clock::run()
{
}
