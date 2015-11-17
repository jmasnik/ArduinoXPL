#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27

#define ADF_LIMIT_LOW 190
#define ADF_LIMIT_HIGH 535
#define NAV_LIMIT_LOW 108
#define NAV_LIMIT_HIGH 117
#define COM_LIMIT_LOW 118
#define COM_LIMIT_HIGH 136

#define BUTTON_BLOCK_INT 500

typedef struct {
  uint8_t pin;
  uint16_t block;
} tlacitko;

tlacitko tlacitko_list[8];

LiquidCrystal_I2C	lcd(I2C_ADDR, 20, 4);

// aktualni zobrazovana obrazovka (1=radia)
uint8_t actual_screen;

uint8_t sipka_left_pos;
uint8_t sipka_left_dir;
uint8_t sipka_right_pos;
uint8_t sipka_right_dir;

long cntr = 0;

int val = 0;

int a, b, c, d;
int a_prev, b_prev, c_prev, d_prev;

// hodnoty
int val_hdg = 0;
int val_crs = 0;
int val_ad1 = 448;
int val_ad2 = 448;
uint8_t val_nav1_a = 110;
uint8_t val_nav1_b = 50;
uint8_t val_nav2_a = 110;
uint8_t val_nav2_b = 50;
uint8_t val_com1_a = 120;
uint8_t val_com1_b = 0;
uint8_t val_com2_a = 120;
uint8_t val_com2_b = 0;

const char char_sipka_r[2] = {126,0};
const char char_sipka_l[2] = {127,0};

void setup(){
  int i;

  // seriak
  Serial.begin(9600);
  while (!Serial) { }

  // definice tlacitak
  tlacitko_list[0].pin = 4;    tlacitko_list[0].block = BUTTON_BLOCK_INT;   // leve mackaci tocitko
  tlacitko_list[1].pin = 5;    tlacitko_list[1].block = BUTTON_BLOCK_INT;   // leve dolni
  tlacitko_list[2].pin = 6;    tlacitko_list[2].block = BUTTON_BLOCK_INT;   // leve horni
  tlacitko_list[3].pin = 7;    tlacitko_list[3].block = BUTTON_BLOCK_INT;   // horni levy
  tlacitko_list[4].pin = 8;    tlacitko_list[4].block = BUTTON_BLOCK_INT;   // horni pravy
  tlacitko_list[5].pin = 9;    tlacitko_list[5].block = BUTTON_BLOCK_INT;   // pravy dolni
  tlacitko_list[6].pin = 10;   tlacitko_list[6].block = BUTTON_BLOCK_INT;   // prvay horni
  tlacitko_list[7].pin = 11;   tlacitko_list[7].block = BUTTON_BLOCK_INT;   // pravy mackaci tocitko

  // nastaveni pinu
  for(i = 0; i < 8; i++){
    pinMode(tlacitko_list[i].pin ,INPUT_PULLUP);  
  }

  // piny k rotacnim
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);

  // sipky na radio obrazovce
  sipka_left_pos = 1;
  sipka_right_pos = 1;
  sipka_left_dir = 1;
  sipka_right_dir = 1;

  // LCD
  lcd.begin();  
  lcd.backlight();

  // aktualni obrazovka
  actual_screen = 1;  
  redraw1();
}

void loop() {
  int i;
  int butt_state;
  
  a_prev = a;
  b_prev = b;
  c_prev = c;
  d_prev = d;
  
  a = digitalRead(2);
  b = digitalRead(3);
  c = digitalRead(12);
  d = digitalRead(13);

  // tlacitka
  for(i = 0; i < 8; i++){
    // nacteni stavu
    butt_state = digitalRead(tlacitko_list[i].pin);
    if(butt_state == HIGH && tlacitko_list[i].block == 0){
      // neco budeme delat - je to nove stisknuty
      tlacitko_list[i].block = BUTTON_BLOCK_INT;

      // prepinani obrazovek -
      if(i == 3){
        if(actual_screen == 1){
          actual_screen = 2;
        } else {
          actual_screen--;
        }
        switch_screen();
      }
      // prepinani obrazovek +
      if(i == 4){
        if(actual_screen == 2){
          actual_screen = 1;
        } else {
          actual_screen++;
        }
        switch_screen();
      }

      // radio stranka
      if(actual_screen == 1){
        if(i == 0){
          if(sipka_left_dir == 1){
            sipka_left_dir = 2;
          } else {
            sipka_left_dir = 1;
          }
          redraw1_sipka();
        }
        if(i == 1){
          if(sipka_left_pos < 4){
            sipka_left_pos++;
            redraw1_sipka();
          }
        }
        if(i == 2){
          if(sipka_left_pos > 1){
            sipka_left_pos--;
            redraw1_sipka();
          }
        }
        if(i == 5){
          if(sipka_right_pos < 4){
            sipka_right_pos++;
            redraw1_sipka();
          }
        }
        if(i == 6){
          if(sipka_right_pos > 1){
            sipka_right_pos--;
            redraw1_sipka();
          }
        }
        if(i == 7){
          if(sipka_right_dir == 1){
            sipka_right_dir = 2;
          } else {
            sipka_right_dir = 1;
          }
          redraw1_sipka();
        }        
      }
    }
    if(butt_state == LOW && tlacitko_list[i].block > 0){
      tlacitko_list[i].block--;
    }
  }

  // poreseni leveho tocitka
  if(a_prev == LOW && b_prev == LOW){

    // zvysovani
    if(a == LOW && b == HIGH){
      /*
      if(sipka_left_pos == 1){
        val_hdg[0]++;
        if(val_hdg[0] >= 360){
          val_hdg[0] = val_hdg[0] % 360;
        }
      }
      if(sipka_left_pos == 2){
        val_crs[0]++;
        if(val_crs[0] >= 360){
          val_crs[0] = 0;
        }
      }      
      if(sipka_left_pos == 3){
        val_ad1[0]++;
        if(val_ad1[0] > ADF_LIMIT_HIGH){
          val_ad1[0] = ADF_LIMIT_LOW;
        }
      }            
      if(sipka_left_pos == 4){
        val_ad2[0]++;
        if(val_ad2[0] > ADF_LIMIT_HIGH){
          val_ad2[0] = ADF_LIMIT_LOW;
        }
      }
      */
    }

    // snizovani
    if(a == HIGH && b == LOW){
      /*
      if(sipka_left_pos == 1){      
        val_hdg[0]--;
        if(val_hdg[0] < 0){
          val_hdg[0] = 359;
        }
      }      
      if(sipka_left_pos == 2){      
        val_crs[0]--;
        if(val_crs[0] < 0){
          val_crs[0] = 359;
        }
      }
      if(sipka_left_pos == 3){
        val_ad1[0]--;
        if(val_ad1[0] < ADF_LIMIT_LOW){
          val_ad1[0] = ADF_LIMIT_HIGH;
        }
      }
      if(sipka_left_pos == 4){
        val_ad2[0]--;
        if(val_ad2[0] < ADF_LIMIT_LOW){
          val_ad2[0] = ADF_LIMIT_HIGH;
        }
      }      
      */
    }    
  }

  // poreseni praveho tocitka
  if(c_prev == LOW && d_prev == LOW){

    // zvysovani
    if(c == LOW && d == HIGH){
      if(actual_screen == 1){

        // NAV1 - zvysovani
        if(sipka_right_pos == 1){
          if(sipka_right_dir == 1){
            if(val_nav1_b == 95){
              val_nav1_b = 0;
            } else {
              val_nav1_b = val_nav1_b + 5;
            }
          }
          if(sipka_right_dir == 2){
            if(val_nav1_a < NAV_LIMIT_HIGH){
              val_nav1_a++;
            }
          }
          redraw1_val(5);
        }

        // NAV2 - zvysovani
        if(sipka_right_pos == 2){
          if(sipka_right_dir == 1){
            if(val_nav2_b == 95){
              val_nav2_b = 0;
            } else {
              val_nav2_b = val_nav2_b + 5;
            }
          }
          if(sipka_right_dir == 2){
            if(val_nav2_a < NAV_LIMIT_HIGH){
              val_nav2_a++;
            }
          }
          redraw1_val(6);
        }        
        
      }
    }

    // snizovani
    if(c == HIGH && d == LOW){
      if(actual_screen == 1){
        
        // NAV1 - snizovani
        if(sipka_right_pos == 1){
          if(sipka_right_dir == 1){
            if(val_nav1_b == 0){
              val_nav1_b = 95;
            } else {
              val_nav1_b = val_nav1_b - 5;
            }
          }
          if(sipka_right_dir == 2){
            if(val_nav1_a > NAV_LIMIT_LOW){
              val_nav1_a--;
            }
          }
          redraw1_val(5);
        }

        // NAV2 - snizovani
        if(sipka_right_pos == 2){
          if(sipka_right_dir == 1){
            if(val_nav2_b == 0){
              val_nav2_b = 95;
            } else {
              val_nav2_b = val_nav2_b - 5;
            }
          }
          if(sipka_right_dir == 2){
            if(val_nav2_a > NAV_LIMIT_LOW){
              val_nav2_a--;
            }
          }
          redraw1_val(6);
        }
        
      }
    }    
  }

  /*
  // seriak
  if(val_hdg[0] != val_hdg[2]){
    Serial.print("HDG_");
    Serial.println(val_hdg[0]);
    val_hdg[2] = val_hdg[0];
  }
  if(val_crs[0] != val_crs[2]){
    Serial.print("CRS_");
    Serial.println(val_crs[0]);
    val_crs[2] = val_crs[0];
  }
  if(val_ad1[0] != val_ad1[2]){
    Serial.print("AD1_");
    Serial.println(val_ad1[0]);
    val_ad1[2] = val_ad1[0];
  }
  if(val_ad2[0] != val_ad2[2]){
    Serial.print("AD2_");
    Serial.println(val_ad2[0]);
    val_ad2[2] = val_ad2[0];
  }
*/
}

/**
 * Prepnuti obrazovek
 */
void switch_screen(){
  if(actual_screen == 1) redraw1();
  if(actual_screen == 2) redraw2();
}

/**
 * Vykresleni komplet stranky 1
 */
void redraw1(){
  uint8_t i;
  
  redraw1_static();
  redraw1_sipka();

  for(i = 1; i <= 8; i++){
    redraw1_val(i);
  }
}

/**
 * Prekresleni hodnoty na strance 1
 */
void redraw1_val(uint8_t var){
  if(var == 1){
    lcd.setCursor (4,0);
    if(val_hdg < 10) lcd.print("0");
    if(val_hdg < 100) lcd.print("0");
    lcd.print(val_hdg);
  }
  if(var == 2){
    lcd.setCursor (4,1);
    if(val_crs < 10) lcd.print("0");
    if(val_crs < 100) lcd.print("0");
    lcd.print(val_crs);
  }
  if(var == 3){
    lcd.setCursor (4,2);
    lcd.print(val_ad1);
  }
  if(var == 4){
    lcd.setCursor (4,3);
    lcd.print(val_ad2);
  }
  if(var == 5){
    lcd.setCursor (14,0);
    lcd.print(val_nav1_a);
    lcd.setCursor (18,0);
    if(val_nav1_b < 10){
      lcd.print("0");
    }
    lcd.print(val_nav1_b);
  }
  if(var == 6){
    lcd.setCursor (14,1);
    lcd.print(val_nav2_a);
    lcd.setCursor (18,1);
    if(val_nav2_b < 10){
      lcd.print("0");
    }
    lcd.print(val_nav2_b);
  }  
  if(var == 7){
    lcd.setCursor (14,2);
    lcd.print(val_com1_a);
    lcd.setCursor (18,2);
    if(val_com1_b < 10){
      lcd.print("0");
    }
    lcd.print(val_com1_b);
  }
  if(var == 8){
    lcd.setCursor (14,3);
    lcd.print(val_com2_a);
    lcd.setCursor (18,3);
    if(val_com2_b < 10){
      lcd.print("0");
    }
    lcd.print(val_com2_b);
  }    
}

/**
 * Prekreslovani sipek na radio strance
 */
void redraw1_sipka(){
    lcd.setCursor (3,0);
    if(sipka_left_pos == 1) lcd.print(sipka_left_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");
    lcd.setCursor (3,1);
    if(sipka_left_pos == 2) lcd.print(sipka_left_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");    
    lcd.setCursor (3,2);
    if(sipka_left_pos == 3) lcd.print(sipka_left_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");        
    lcd.setCursor (3,3);
    if(sipka_left_pos == 4) lcd.print(sipka_left_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");  

    lcd.setCursor (13,0);
    if(sipka_right_pos == 1) lcd.print(sipka_right_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");
    lcd.setCursor (13,1);
    if(sipka_right_pos == 2) lcd.print(sipka_right_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");    
    lcd.setCursor (13,2);
    if(sipka_right_pos == 3) lcd.print(sipka_right_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");        
    lcd.setCursor (13,3);
    if(sipka_right_pos == 4) lcd.print(sipka_right_dir == 1 ? char_sipka_r : char_sipka_l); else lcd.print(" ");
}

/**
 * Vykresleni statickych veci na radio strance
 */
void redraw1_static(){
  lcd.setCursor (0,0);
  lcd.print("HDG      NAV1    .  ");
  lcd.setCursor (0,1);
  lcd.print("CRS      NAV2    .  ");
  lcd.setCursor (0,2);
  lcd.print("AD1      COM1    .  ");
  lcd.setCursor (0,3);
  lcd.print("AD2      COM2    .  ");
}

/**
 * Komplet prekresleni druhe obrazovky
 */
void redraw2(){
  redraw2_static();
}

/**
 * Staticka druha obrazovka
 */
void redraw2_static(){
  lcd.setCursor (0,0);
  lcd.print("SCREEN 2            ");
  lcd.setCursor (0,1);
  lcd.print("                    ");
  lcd.setCursor (0,2);
  lcd.print("                    ");
  lcd.setCursor (0,3);
  lcd.print("                    ");
}

