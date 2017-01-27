#include "HID-Project.h"

#define SWITCH_COUNT 4
#define BUTTON_TMR_MAX 255

typedef struct {
  uint8_t pin;
  
  uint8_t button_a;
  uint8_t button_a_tmr;
  
  uint8_t button_b;
  uint8_t button_b_tmr;
  
  uint8_t actual_value;
} hwswitch;

hwswitch switch_list[SWITCH_COUNT];

const int pinLed = LED_BUILTIN;
const int pinButton = 2;

void setup() {
  uint8_t i;

  switch_list[0].pin = 2;
  switch_list[0].button_a = 1;
  switch_list[0].button_b = 2;

  switch_list[1].pin = 3;
  switch_list[1].button_a = 3;
  switch_list[1].button_b = 4;

  switch_list[2].pin = 4;
  switch_list[2].button_a = 5;
  switch_list[2].button_b = 6;

  switch_list[3].pin = 5;
  switch_list[3].button_a = 7;
  switch_list[3].button_b = 8;
  
  pinMode(pinLed, OUTPUT);

  for(i = 0; i < SWITCH_COUNT; i++){
    pinMode(switch_list[i].pin, INPUT_PULLUP);
  }

  for(i = 0; i < SWITCH_COUNT; i++){
    if(digitalRead(switch_list[i].pin) == HIGH){
      switch_list[i].actual_value = 1;
    } else {
      switch_list[i].actual_value = 0;
    }
  }

  Gamepad.begin();
}

void loop() {
  uint8_t i;

  for(i = 0; i < SWITCH_COUNT; i++){
    if(digitalRead(switch_list[i].pin) == HIGH && switch_list[i].actual_value == 0){
      // zmena na 1
      switch_list[i].actual_value = 1;
      switch_list[i].button_a_tmr = BUTTON_TMR_MAX;
    }
    if(digitalRead(switch_list[i].pin) == LOW && switch_list[i].actual_value == 1){
      // zmena na 0
      switch_list[i].actual_value = 0;
      switch_list[i].button_b_tmr = BUTTON_TMR_MAX;
    }
  }

  for(i = 0; i < SWITCH_COUNT; i++){
    if(switch_list[i].button_a_tmr > 0){
      if(switch_list[i].button_a_tmr == BUTTON_TMR_MAX) Gamepad.press(switch_list[i].button_a);
      if(switch_list[i].button_a_tmr == 1) Gamepad.release(switch_list[i].button_a);
      switch_list[i].button_a_tmr--;
    }
    if(switch_list[i].button_b_tmr > 0){
      if(switch_list[i].button_b_tmr == BUTTON_TMR_MAX) Gamepad.press(switch_list[i].button_b);
      if(switch_list[i].button_b_tmr == 1) Gamepad.release(switch_list[i].button_b);
      switch_list[i].button_b_tmr--;
    }    
  }

  Gamepad.write();
}
