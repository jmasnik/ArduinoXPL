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

#define SERIAL_BUFF_SIZE 30

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

uint8_t sipka_ap_left_pos;

long cntr = 0;

int val = 0;

int a, b, c, d;
int a_prev, b_prev, c_prev, d_prev;
int A_block, B_block;

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
uint8_t val_avionics_on = 1;
uint8_t val_low_voltage = 0;

uint8_t val_autopilot_mode = 0;
uint8_t val_autopilot_heading_mode = 0;
uint8_t val_autopilot_hnav_armed = 0;
uint8_t val_autopilot_altitude_hold_armed = 0;
uint8_t val_autopilot_altitude_mode = 0;

char val_gps_to[10] = "";
double val_gps_distance = 0;
int val_gps_gs = 0;
int val_gps_ete = 0;
int val_gps_dtk = 0;
int val_gps_brg = 0;
int val_gps_trk = 0;

const char char_sipka_r[2] = {126,0};
const char char_sipka_l[2] = {127,0};
const char char_ctverec_on[2] = {255,0};
const char char_ctverec_off[2] = {219,0};

char serial_buff[SERIAL_BUFF_SIZE];
int serial_cntr = 0;

void setup(){
  int i;

  // seriak
  Serial.begin(115200);
  while (!Serial) { }

  // definice tlacitak
  tlacitko_list[0].pin = 4;    tlacitko_list[0].block = 0;   // leve mackaci tocitko
  tlacitko_list[1].pin = 5;    tlacitko_list[1].block = 0;   // leve dolni
  tlacitko_list[2].pin = 6;    tlacitko_list[2].block = 0;   // leve horni
  tlacitko_list[3].pin = 7;    tlacitko_list[3].block = 0;   // horni levy
  tlacitko_list[4].pin = 8;    tlacitko_list[4].block = 0;   // horni pravy
  tlacitko_list[5].pin = 9;    tlacitko_list[5].block = 0;   // pravy dolni
  tlacitko_list[6].pin = 10;   tlacitko_list[6].block = 0;   // prvay horni
  tlacitko_list[7].pin = 11;   tlacitko_list[7].block = 0;   // pravy mackaci tocitko

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

  // smer sipky - 1=vpravo, 2=vlevo
  sipka_left_dir = 1;
  sipka_right_dir = 1;

  // sipka na AP obrazovce
  sipka_ap_left_pos = 1;

  // LCD
  lcd.begin();  
  lcd.backlight();

  // aktualni obrazovka
  actual_screen = 1;  
  redraw1();

  // uvodni odeslani udaju
  for(i = 1; i <= 8; i++){
    sendValSerial(i);
  }

  A_block = 0;
  B_block = 0;

  displayOnOff();
}

void loop() {
  int i;
  int butt_state;
  char serial_ch;

  // prijem dat po seriaku
  if(Serial.available() > 0) {
    serial_ch = Serial.read();
    if(serial_ch == '|' && serial_cntr > 0){
      if(serial_buff[3] == '_'){
        if(strncmp(serial_buff, "GP1", 3) == 0){
          strcpy(val_gps_to, &serial_buff[4]);
          redraw2_val(1);
        }
        if(strncmp(serial_buff, "GP2", 3) == 0){
          val_gps_distance = atof(&serial_buff[4]);
          redraw2_val(2);
        }
        if(strncmp(serial_buff, "GP5", 3) == 0){
          val_gps_gs = atoi(&serial_buff[4]);
          redraw2_val(3);
        }
        if(strncmp(serial_buff, "GP4", 3) == 0){
          val_gps_ete = atoi(&serial_buff[4]);
          redraw2_val(4);
        }
        if(strncmp(serial_buff, "GP3", 3) == 0){
          val_gps_dtk = atof(&serial_buff[4]);
          redraw2_val(5);
        }
        if(strncmp(serial_buff, "GP6", 3) == 0){
          val_gps_brg = atoi(&serial_buff[4]);
          redraw2_val(6);
        }
        if(strncmp(serial_buff, "GP7", 3) == 0){
          val_gps_trk = atoi(&serial_buff[4]);
          redraw2_val(7);
        }
        if(strncmp(serial_buff, "AVO", 3) == 0){
          val_avionics_on = atoi(&serial_buff[4]);
          displayOnOff();
        }        
        if(strncmp(serial_buff, "LOV", 3) == 0){
          val_low_voltage = atoi(&serial_buff[4]);
          displayOnOff();
        }
        if(strncmp(serial_buff, "APM", 3) == 0){
          val_autopilot_mode = atoi(&serial_buff[4]);
          redraw3_val(1);
        }
        if(strncmp(serial_buff, "APH", 3) == 0){
          val_autopilot_heading_mode = atoi(&serial_buff[4]);
          redraw3_val(2);
        }
        if(strncmp(serial_buff, "APN", 3) == 0){
          val_autopilot_hnav_armed = atoi(&serial_buff[4]);
          redraw3_val(3);
        }
        if(strncmp(serial_buff, "APA", 3) == 0){
          val_autopilot_altitude_hold_armed = atoi(&serial_buff[4]);
          redraw3_val(4);
        }
        if(strncmp(serial_buff, "APB", 3) == 0){
          val_autopilot_altitude_mode = atoi(&serial_buff[4]);
          redraw3_val(4);
        }
      }
      serial_cntr = 0;
    } else {
      if(serial_cntr < SERIAL_BUFF_SIZE - 1 && serial_ch != '|'){
        serial_buff[serial_cntr] = serial_ch;
        serial_cntr++;
        serial_buff[serial_cntr] = '\0';
      }
    }
  }
  
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
    
    if(butt_state == LOW && tlacitko_list[i].block == 0){
      // neco budeme delat - je to nove stisknuty
      tlacitko_list[i].block = BUTTON_BLOCK_INT;

      // prepinani obrazovek -
      if(i == 3){
        if(actual_screen == 1){
          actual_screen = 4;
        } else {
          actual_screen--;
        }
        switch_screen();
      }
      // prepinani obrazovek +
      if(i == 4){
        if(actual_screen == 4){
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

      // GPS stranka - tlacitka
      if(actual_screen == 2){
        // leve dolni
        if(i == 1) sendCmdSerial("GPS2");
        // leve horni
        if(i == 2) sendCmdSerial("GPS1");
        // leve mackaci tocitko
        if(i == 0) sendCmdSerial("GPS3");
        // prave dolni
        if(i == 5) sendCmdSerial("GPS5");
        // prave horni
        if(i == 6) sendCmdSerial("GPS4");
        // prave mackaci tocitko
        if(i == 7) sendCmdSerial("GPS6");        
      }

      // AP stranka
      if(actual_screen == 3){
         if(i == 0){
            // zmacknuti leveho tocitka - volba funkce
            if(sipka_ap_left_pos == 1) sendValSerial(15);
            if(sipka_ap_left_pos == 2) sendValSerial(16);
            if(sipka_ap_left_pos == 3) sendValSerial(17);
            if(sipka_ap_left_pos == 4) sendValSerial(18);
         }

         if(i == 1){
            // leve dolni tlacitko - posun leve sipky dolu
            if(sipka_ap_left_pos < 4) sipka_ap_left_pos++;
            redraw3();
         }         
         if(i == 2){
            // leve horni tlacitko - posun leve sipky nahoru
            if(sipka_ap_left_pos > 1) sipka_ap_left_pos--;
            redraw3();
         }
      }
      
    }
    
    if(butt_state == HIGH && tlacitko_list[i].block > 0){
      tlacitko_list[i].block--;
    }
  }

  // poreseni leveho tocitka
  if(a_prev == LOW && b_prev == LOW){

    // zvysovani
    if(a == LOW && b == HIGH && A_block == 0){
      A_block = BUTTON_BLOCK_INT;
      
      if(actual_screen == 1){
        
        // HDG - zvyseni
        if(sipka_left_pos == 1){
          if(sipka_left_dir == 1){
            val_hdg++;
          }
          if(sipka_left_dir == 2){
            val_hdg += 10;
          }
          if(val_hdg >= 360){
            val_hdg = val_hdg % 360;
          }
          redraw1_val(1);
          sendValSerial(1);
        }

        // CRS - zvyseni
        if(sipka_left_pos == 2){
          if(sipka_left_dir == 1){
            val_crs++;
          }
          if(sipka_left_dir == 2){
            val_crs += 10;
          }
          if(val_crs >= 360){
            val_crs = val_crs % 360;
          }
          redraw1_val(2);
          sendValSerial(2);
        }

        // ADF1 - zvyseni
        if(sipka_left_pos == 3){
          if(sipka_left_dir == 1){
            val_ad1++;
          }
          if(sipka_left_dir == 2){
            val_ad1 += 10;
          }
          if(val_ad1 > ADF_LIMIT_HIGH){
            val_ad1 = ADF_LIMIT_LOW;
          }
          redraw1_val(3);
          sendValSerial(3);
        }            

        // ADF2 - zvyseni
        if(sipka_left_pos == 4){
          if(sipka_left_dir == 1){
            val_ad2++;
          }
          if(sipka_left_dir == 2){
            val_ad2 += 10;
          }
          if(val_ad2 > ADF_LIMIT_HIGH){
            val_ad2 = ADF_LIMIT_LOW;
          }
          redraw1_val(4);
          sendValSerial(4);
        }
      }
      if(actual_screen == 2){
        sendValSerial(11);
      }
      /*
      if(actual_screen == 3){
        if(sipka_ap_left_pos < 4){
          sipka_ap_left_pos++;
          redraw3();
        }
      }
      */
    }

    // snizovani
    if(a == HIGH && b == LOW && A_block == 0){
      A_block = BUTTON_BLOCK_INT;
      
      if(actual_screen == 1){

        // HDG - snizeni
        if(sipka_left_pos == 1){      
          if(sipka_left_dir == 1){
            val_hdg--;
          }
          if(sipka_left_dir == 2){
            val_hdg -= 10;
          }
          if(val_hdg < 0){
            val_hdg = 360 + val_hdg;
          }
          redraw1_val(1);
          sendValSerial(1);
        }      

        // CRS - snizeni
        if(sipka_left_pos == 2){      
          if(sipka_left_dir == 1){
            val_crs--;
          }
          if(sipka_left_dir == 2){
            val_crs -= 10;
          }
          if(val_crs < 0){
            val_crs = 360 + val_crs;
          }
          redraw1_val(2);
          sendValSerial(2);
        }

        // ADF1 - snizeni
        if(sipka_left_pos == 3){
          if(sipka_left_dir == 1){
            val_ad1--;
          }
          if(sipka_left_dir == 2){
            val_ad1 -= 10;
          }
          if(val_ad1 < ADF_LIMIT_LOW){
            val_ad1 = ADF_LIMIT_HIGH;
          }
          redraw1_val(3);
          sendValSerial(3);
        }

        // ADF2 - snizeni
        if(sipka_left_pos == 4){
          if(sipka_left_dir == 1){
            val_ad2--;
          }
          if(sipka_left_dir == 2){
            val_ad2 -= 10;
          }
          if(val_ad2 < ADF_LIMIT_LOW){
            val_ad2 = ADF_LIMIT_HIGH;
          }
          redraw1_val(4);
          sendValSerial(4);
        }
              
      }
      if(actual_screen == 2){
        sendValSerial(12);
      }
      /*   
      if(actual_screen == 3){
        if(sipka_ap_left_pos > 1){
          sipka_ap_left_pos--;
          redraw3();
        }
      }
      */   
    }    
  }

  // poreseni praveho tocitka
  if(c_prev == LOW && d_prev == LOW){

    // zvysovani
    if(c == LOW && d == HIGH && B_block == 0){
      B_block = BUTTON_BLOCK_INT;
      
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
          sendValSerial(5);
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
          sendValSerial(6);
        }        

        // COM1 - zvysovani
        if(sipka_right_pos == 3){
          if(sipka_right_dir == 1){
            if(val_com1_b == 97){
              val_com1_b = 0;
            } else {
              val_com1_b = val_com1_b + (val_com1_b % 5 == 0 ? 2 : 3);
            }
          }
          if(sipka_right_dir == 2){
            if(val_com1_a < COM_LIMIT_HIGH){
              val_com1_a++;
            }
          }
          redraw1_val(7);
          sendValSerial(7);
        }

        // COM2 - zvysovani
        if(sipka_right_pos == 4){
          if(sipka_right_dir == 1){
            if(val_com2_b == 97){
              val_com2_b = 0;
            } else {
              val_com2_b = val_com2_b + (val_com2_b % 5 == 0 ? 2 : 3);
            }
          }
          if(sipka_right_dir == 2){
            if(val_com2_a < COM_LIMIT_HIGH){
              val_com2_a++;
            }
          }
          redraw1_val(8);
          sendValSerial(8);
        }
        
      }
      if(actual_screen == 2){
        sendValSerial(13);
      }
    }

    // snizovani
    if(c == HIGH && d == LOW && B_block == 0){
      B_block = BUTTON_BLOCK_INT;
      
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
          sendValSerial(5);
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
          sendValSerial(6);
        }

        // COM1 - snizovani
        if(sipka_right_pos == 3){
          if(sipka_right_dir == 1){
            if(val_com1_b == 0){
              val_com1_b = 97;
            } else {
              val_com1_b = val_com1_b - (val_com1_b % 5 == 0 ? 3 : 2);
            }
          }
          if(sipka_right_dir == 2){
            if(val_com1_a > COM_LIMIT_LOW){
              val_com1_a--;
            }
          }
          redraw1_val(7);
          sendValSerial(7);
        }

        // COM2 - snizovani
        if(sipka_right_pos == 4){
          if(sipka_right_dir == 1){
            if(val_com2_b == 0){
              val_com2_b = 97;
            } else {
              val_com2_b = val_com2_b - (val_com2_b % 5 == 0 ? 3 : 2);
            }
          }
          if(sipka_right_dir == 2){
            if(val_com2_a > COM_LIMIT_LOW){
              val_com2_a--;
            }
          }
          redraw1_val(8);
          sendValSerial(8);
        }
        
      }
      if(actual_screen == 2){
        sendValSerial(14);
      }      
    }    
  }

  if(A_block > 0) A_block--;
  if(B_block > 0) B_block--;
}

/**
 * Poslani prikazu na seriak - jen string, zadna hodnota
 */
void sendCmdSerial(char *var){
  Serial.println(var);
}

/**
 * Poslani hodnoty na seriak
 */
void sendValSerial(uint8_t var){
  if(var == 1){
    Serial.print("HDG_");
    Serial.println(val_hdg);
  }
  if(var == 2){
    Serial.print("CRS_");
    Serial.println(val_crs);
  }
  if(var == 3){
    Serial.print("AD1_");
    Serial.println(val_ad1);    
  }
  if(var == 4){
    Serial.print("AD2_");
    Serial.println(val_ad2);    
  }
  if(var == 5){
    Serial.print("NA1_");
    Serial.print(val_nav1_a);
    if(val_nav1_b < 10){
      Serial.print("0");  
    }
    Serial.println(val_nav1_b);
  }
  if(var == 6){
    Serial.print("NA2_");
    Serial.print(val_nav2_a);
    if(val_nav2_b < 10){
      Serial.print("0");  
    }
    Serial.println(val_nav2_b);
  }  
  if(var == 7){
    Serial.print("CO1_");
    Serial.print(val_com1_a);
    if(val_com1_b < 10){
      Serial.print("0");  
    }
    Serial.println(val_com1_b);
  }
  if(var == 8){
    Serial.print("CO2_");
    Serial.print(val_com2_a);
    if(val_com2_b < 10){
      Serial.print("0");  
    }
    Serial.println(val_com2_b);
  }    
  if(var == 9){
    Serial.println("GP1");
  }
  if(var == 10){
    Serial.println("GP2");
  }  
  if(var == 11){
    Serial.println("GP3");
  }
  if(var == 12){
    Serial.println("GP4");
  }
  if(var == 13){
    Serial.println("GP5");
  }
  if(var == 14){
    Serial.println("GP6");
  }

  // AP prikazy
  if(var == 15) Serial.println("APM");
  if(var == 16) Serial.println("APH");
  if(var == 17) Serial.println("APN");
  if(var == 18) Serial.println("APA");
}

/**
 * Prepnuti obrazovek
 */
void switch_screen(){
  if(actual_screen == 1) redraw1();
  if(actual_screen == 2) redraw2();
  if(actual_screen == 3) redraw3();
  if(actual_screen == 4) redraw4();
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

  if(actual_screen != 1){
    return;
  }
  
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

void redraw2_val(uint8_t var){
  int ete;
  uint8_t ete_h;
  uint8_t ete_m;
  uint8_t ete_s;
  
  if(actual_screen != 2){
    return;
  }
  if(var == 1){    
    lcd.setCursor (4,0);
    lcd.print("                ");
    lcd.setCursor (4,0);
    lcd.print(val_gps_to);
  }  
  if(var == 2){    
    lcd.setCursor (4,1);
    lcd.print("         ");
    lcd.setCursor (4,1);
    lcd.print(val_gps_distance);
  }
  if(var == 3){    
    lcd.setCursor (4,2);
    lcd.print("         ");
    lcd.setCursor (4,2);
    lcd.print(val_gps_gs);
  }  
  if(var == 4){    
    lcd.setCursor (4,3);
    lcd.print("         ");
    lcd.setCursor (4,3);

    ete = val_gps_ete;
    ete_s = ete % 60;
    ete = (ete - ete_s) / 60;
    ete_m = ete % 60;
    ete_h = (ete - ete_m) / 60;

    if(ete_h > 0){
      // jsou tam nejake hodiny
      lcd.print(ete_h);
      lcd.print(":");
      if(ete_m < 10){
        lcd.print("0");  
      }
      lcd.print(ete_m);
    } else {
      // jen minuty a vteriny
      lcd.print(ete_m);
      lcd.print(":");
      if(ete_s < 10){
        lcd.print("0");  
      }
      lcd.print(ete_s);
    }
    
  }
  if(var == 5){    
    lcd.setCursor (17,1);
    lcd.print("   ");
    lcd.setCursor (17,1);
    lcd.print(val_gps_dtk);
  }  
  if(var == 6){    
    lcd.setCursor (17,2);
    lcd.print("   ");
    lcd.setCursor (17,2);
    lcd.print(val_gps_brg);
  }
  if(var == 7){    
    lcd.setCursor (17,3);
    lcd.print("   ");
    lcd.setCursor (17,3);
    lcd.print(val_gps_trk);
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
  uint8_t i;
  redraw2_static();
  for(i = 1; i <= 7; i++){
    redraw2_val(i);
  }  
}

/**
 * Staticka druha obrazovka
 */
void redraw2_static(){
  lcd.setCursor (0,0);
  lcd.print("TO                  ");
  lcd.setCursor (0,1);
  lcd.print("DIS          DTK    ");
  lcd.setCursor (0,2);
  lcd.print("GS           BRG    ");
  lcd.setCursor (0,3);
  lcd.print("ETE          TRK    ");
}

/**
 * Komplet prekresleni AP obrazovky
 */
void redraw3(){
  uint8_t i;
  redraw3_static();
  for(i = 1; i <= 4; i++){
    redraw3_val(i);
  }  
}

/**
 * Staticka AP obrazovka
 */
void redraw3_static(){
  lcd.setCursor (0,0);
  lcd.print("AP                  ");
  lcd.setCursor (0,1);
  lcd.print("HDG                 ");
  lcd.setCursor (0,2);
  lcd.print("NAV                 ");
  lcd.setCursor (0,3);
  lcd.print("ALT                 ");
}

/**
 * Dynamicke veci na AP obrazovce
 */
void redraw3_val(uint8_t i){

  if(actual_screen != 3){
    return;
  }
  
  if(i == 1){
    lcd.setCursor (3,0);
    lcd.print(sipka_ap_left_pos == 1 ? char_sipka_r : " ");
    lcd.print(val_autopilot_mode == 2 ? char_ctverec_on : char_ctverec_off);
  }
  if(i == 2){
    lcd.setCursor (3,1);
    lcd.print(sipka_ap_left_pos == 2 ? char_sipka_r : " ");
    lcd.print(val_autopilot_heading_mode == 1 ? char_ctverec_on :char_ctverec_off);
  }
  if(i == 3){
    lcd.setCursor (3,2);
    lcd.print(sipka_ap_left_pos == 3 ? char_sipka_r : " ");
    lcd.print(val_autopilot_hnav_armed == 1 ? char_ctverec_on :char_ctverec_off);
  }
  if(i == 4){
    lcd.setCursor (3,3);
    lcd.print(sipka_ap_left_pos == 4 ? char_sipka_r : " ");
    lcd.print(val_autopilot_altitude_hold_armed == 1 ? char_ctverec_on :char_ctverec_off);
  }
}

void redraw4(){
  redraw4_static();
}

void redraw4_static(){
  lcd.setCursor (0,0);
  lcd.print("SQ/TM               ");
  lcd.setCursor (0,1);
  lcd.print("                    ");
  lcd.setCursor (0,2);
  lcd.print("                    ");
  lcd.setCursor (0,3);
  lcd.print("                    ");
}

/**
 * Funkce, ktera resi, jestli displej sviti/ukazuje
 */
void displayOnOff(){
  if(val_low_voltage == 0 && val_avionics_on == 1){
      lcd.backlight();
      lcd.display();
  } else {
      lcd.noBacklight();
      lcd.noDisplay();  
  }
}


