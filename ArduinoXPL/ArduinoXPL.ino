#include <Wire.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>
//#include <LiquidCrystal.h>

#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

#define ADF_LIMIT_LOW 190
#define ADF_LIMIT_HIGH 535

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
uint8_t sipka_right_pos;

long cntr = 0;

int val = 0;

int a;
int b;
int a_prev;
int b_prev;
int del = 0;

int b1 = LOW;
int b1_block = 0;

int sel = 0;

// hodnoty: 0 - hodnota nastavena tocitkem, 1 - hodnota LCD, 2 - hodnota poslana
int val_hdg[3];
int val_crs[3];
int val_ad1[3];
int val_ad2[3];

char char_sipka[2] = {126,0};
//char special_char[4] = {127,255,126,0};

void setup(){
  int i;

  // seriak
  Serial.begin(9600);
  while (!Serial) {
  }

  // definice tlacitak
  tlacitko_list[0].pin = 4;    tlacitko_list[0].block = 0;   // leve mackaci tocitko
  tlacitko_list[1].pin = 5;    tlacitko_list[1].block = 0;   // leve dolni
  tlacitko_list[2].pin = 6;    tlacitko_list[2].block = 0;   // leve horni
  tlacitko_list[3].pin = 7;    tlacitko_list[3].block = 0;   // ?
  tlacitko_list[4].pin = 8;    tlacitko_list[4].block = 0;   // ?
  tlacitko_list[5].pin = 9;    tlacitko_list[5].block = 0;   // ?
  tlacitko_list[6].pin = 10;   tlacitko_list[6].block = 0;   // ?
  tlacitko_list[7].pin = 11;   tlacitko_list[7].block = 0;   // ?

  // nastaveni pinu
  for(i = 0; i < 8; i++){
    pinMode(tlacitko_list[i].pin ,INPUT_PULLUP);  
  }

  // piny k rotacnim
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);

  // aktualni obrazovka
  actual_screen = 1;
  sipka_left_pos = 1;
  sipka_right_pos = 1;

  // LCD
  lcd.begin();  
  lcd.backlight();


  lcd.setCursor (0,0);
  lcd.print("HDG");
  lcd.setCursor (0,1);
  lcd.print("CRS");
  lcd.setCursor (0,2);
  lcd.print("AD1");
  lcd.setCursor (0,3);
  lcd.print("AD2");
  
  lcd.setCursor (9,0);
  lcd.print("NAV1 120.55");
  lcd.setCursor (9,1);
  lcd.print("NAV2 150.55");
  lcd.setCursor (9,2);
  lcd.print("COM1 120.45");
  lcd.setCursor (9,3);
  lcd.print("COM2 140.63");
  
  val_hdg[0] = 0;
  val_hdg[1] = 1;
  val_hdg[2] = 1;
  
  val_crs[0] = 0;
  val_crs[1] = 1;
  val_crs[2] = 1;  
  
  val_ad1[0] = 448;
  val_ad1[1] = 0;
  val_ad1[2] = 0;
  
  val_ad2[0] = 448;
  val_ad2[1] = 0;
  val_ad2[2] = 0;  

  redraw1_sipka();
}

void loop() {
  int i;
  int butt_state;
  
  del++;
  
  a_prev = a;
  b_prev = b;
  
  a = digitalRead(2);
  b = digitalRead(3);

  // tlacitka
  for(i = 0; i < 8; i++){
    // nacteni stavu
    butt_state = digitalRead(tlacitko_list[i].pin);
    if(butt_state == HIGH && tlacitko_list[i].block == 0){
      // neco budeme delat - je to nove stisknuty
      tlacitko_list[i].block = BUTTON_BLOCK_INT;

      // radio stranka
      if(actual_screen == 1){
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
      }
    }
    if(butt_state == LOW && tlacitko_list[i].block > 0){
      tlacitko_list[i].block--;
    }
  }

  // poreseni tocitka
  if(a_prev == LOW && b_prev == LOW){

    // zvysovani
    if(a == LOW && b == HIGH){
      if(sipka_left_pos == 1){
        val_hdg[0]++;
        if(val_hdg[0] >= 360){
          val_hdg[0] = 0;
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
    }

    // snizovani
    if(a == HIGH && b == LOW){
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
      
    }    
  }
  
  // displej  
  if(val_hdg[0] != val_hdg[1]){
    val = val_hdg[0];
    lcd.setCursor (4,0);
    if(val_hdg[0] < 10) lcd.print("0");
    if(val_hdg[0] < 100) lcd.print("0");
    lcd.print(val_hdg[0]);
    val_hdg[1] = val_hdg[0];
  }
  if(val_crs[0] != val_crs[1]){
    val = val_crs[0];
    lcd.setCursor (4,1);
    if(val_crs[0] < 10) lcd.print("0");
    if(val_crs[0] < 100) lcd.print("0");
    lcd.print(val_crs[0]);
    val_crs[1] = val_crs[0];
  }  
  if(val_ad1[0] != val_ad1[1]){
    val = val_crs[0];
    lcd.setCursor (4,2);
    if(val_ad1[0] < 10) lcd.print("0");
    if(val_ad1[0] < 100) lcd.print("0");
    lcd.print(val_ad1[0]);
    val_ad1[1] = val_ad1[0];
  }
  if(val_ad2[0] != val_ad2[1]){
    val = val_crs[0];
    lcd.setCursor (4,3);
    if(val_ad2[0] < 10) lcd.print("0");
    if(val_ad2[0] < 100) lcd.print("0");
    lcd.print(val_ad2[0]);
    val_ad2[1] = val_ad2[0];
  }
  
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

}

/**
 * Prekreslovani sipek na radio strance
 */
void redraw1_sipka(){
    lcd.setCursor (3,0);
    if(sipka_left_pos == 1) lcd.print(char_sipka); else lcd.print(" ");
    lcd.setCursor (3,1);
    if(sipka_left_pos == 2) lcd.print(char_sipka); else lcd.print(" ");    
    lcd.setCursor (3,2);
    if(sipka_left_pos == 3) lcd.print(char_sipka); else lcd.print(" ");        
    lcd.setCursor (3,3);
    if(sipka_left_pos == 4) lcd.print(char_sipka); else lcd.print(" ");  

    lcd.setCursor (13,0);
    if(sipka_right_pos == 1) lcd.print(char_sipka); else lcd.print(" ");
    lcd.setCursor (13,1);
    if(sipka_right_pos == 2) lcd.print(char_sipka); else lcd.print(" ");    
    lcd.setCursor (13,2);
    if(sipka_right_pos == 3) lcd.print(char_sipka); else lcd.print(" ");        
    lcd.setCursor (13,3);
    if(sipka_right_pos == 4) lcd.print(char_sipka); else lcd.print(" ");      
}

