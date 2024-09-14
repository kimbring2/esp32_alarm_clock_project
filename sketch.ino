#include "clock.h"
#include <TM1637Display.h>

#define CLK  22 // The ESP32 pin GPIO22 connected to CLK
#define DIO  23 // The ESP32 pin GPIO23 connected to DIO

// create a display object of type TM1637Display
//TM1637Display display = TM1637Display(CLK, DIO);

// Hardware pins for buttons, alarm switch and buzzer pin
#define MENU_PIN   16
#define PLUS_PIN   4
#define MINUS_PIN  2
#define OK_PIN     0
#define ALARM_PIN  15
#define BUZZER_PIN 12

TM1637 display(5, 18);
Clock clk;

// ISRs for buttons
static void button_menu_pressed(void)
{
  // Tell clock menu was pressed
  clk.button_pressed(BUTTON_MENU);
}

static void button_ok_pressed(void)
{
  // Tell clock ok was pressed
  clk.button_pressed(BUTTON_OK);
}

static void button_plus_pressed(void)
{
  // Tell clock plus was pressed
  clk.button_pressed(BUTTON_PLUS);
}

static void button_minus_pressed(void)
{
  // Tell clock minus was pressed
  clk.button_pressed(BUTTON_MINUS);
}

static void alarm_status_changed(void)
{
  //Serial.println("alarm_status_changed()");
  // Tell clock alarm changed
  clk.turn_alarm(digitalRead(ALARM_PIN));
}

void IRAM_ATTR Timer1_ISR()
{
    //Serial.println("Timer1_ISR\n");

    uint8_t clk_hour = clk.get_hour();
    uint8_t clk_minutes = clk.get_minutes();
    uint8_t clk_seconds = clk.get_seconds();
    uint8_t clk_step = clk.get_step();
    bool alarm_state = clk.get_alarm_state();
    ClockState clock_state = clk.get_clock_state();

    //Serial.print("clock_state: ");
    //Serial.println(clock_state);

    //Serial.print("alarm_state: ");
    //Serial.println(alarm_state);
    //Serial.println("");

    if ( (clock_state != SET_2) and (clock_state != SET_3) ) {
      if (clk_seconds == 59) {
        if (clk_minutes != 60) {
          clk.set_time(clk_hour, clk_minutes + 1, 0);
        } else {
          clk.set_time(clk_hour + 1, 0, 0);
        }
      } else {
        clk.set_time(clk_hour, clk_minutes, clk_seconds + 1);
      }
    }

    if (clk_step <= 1000) {
      clk.set_step(clk_step + 1);
    } else {
      clk.set_step(0);
    }
}


void setup() {
  Serial.begin(115200); // Any baud rate should work
  //Serial.println("Hello Arduino\n");

  // Configure buttons as inputs with pull-up
  pinMode(MENU_PIN, INPUT_PULLUP);
  pinMode(PLUS_PIN, INPUT_PULLUP);
  pinMode(MINUS_PIN, INPUT_PULLUP);
  pinMode(OK_PIN, INPUT_PULLUP);
  pinMode(ALARM_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Attach interrupt for the buttons
  attachInterrupt(digitalPinToInterrupt(MENU_PIN), button_menu_pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(OK_PIN), button_ok_pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarm_status_changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PLUS_PIN), button_plus_pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(MINUS_PIN), button_minus_pressed, FALLING);

  display.init();
  display.set(BRIGHT_TYPICAL);
  
  // Clock class init
  clk.init(&display, BUZZER_PIN);
  clk.turn_alarm(digitalRead(ALARM_PIN));
  /* Uncomment the following lines to set the time 
     and alarm for testing, it will set it to 23:02:55 
     with alarm at 23:03. Remember to enable the alarm
     using the slide switch
  */
  clk.set_time(23, 02, 50);
  clk.set_alarm(23, 03);
  
  // Start the clock
  clk.run();

  timerAlarm(clk.get_timer(), 500000, true, 0);  // set alarm every 1 sec
  timerAttachInterrupt(clk.get_timer(), &Timer1_ISR);
}

void loop() {
  // Delay to help with simulation running
  clk.show();
  clk.check_alarm();
  //Serial.println("loop()\n");

  //tone(BUZZER_PIN, 500);
  //digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);
}