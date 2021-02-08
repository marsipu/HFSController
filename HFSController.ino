#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

unsigned long t_last_active;
bool sleeping;
unsigned int opt_idx = 0;  // Current selected option

const char *opt_arr[5] = {"Freq.[Hz]", "Len.[ms]", "Interval[s]", "NumRep", "Sleep[s]"};
unsigned int val_arr[5] = {1, 1000, 2, 100, 60};

void add_button_labels(){
  u8g2.drawDisc(82, 6, 6);
  u8g2.drawStr(70, 20, "Next");

  u8g2.drawDisc(112, 6, 6);
  u8g2.drawStr(97, 20, "Start");

  u8g2.drawDisc(82, 47, 6);
  u8g2.drawStr(80, 57, "-");
  
  u8g2.drawDisc(112, 47, 6);
  u8g2.drawStr(110, 57, "+");
}

void update_options(){
  u8g2.clearBuffer();
  u8g2.drawStr(0, 20, opt_arr[opt_idx]);
  String val_str = String(val_arr[opt_idx]);
  char val_char[val_str.length() + 1];
  val_str.toCharArray(val_char, val_str.length() + 1);
  u8g2.drawStr(16, 40, val_char);
  add_button_labels();
  u8g2.sendBuffer();
}

void setup() {
  // put your setup code here, to run once:

  // define pin-modes
  pinMode(A0, INPUT);  // Start-Button
  pinMode(A1, INPUT);  // Next-Button
  pinMode(2, INPUT);  // Minus-Button
  pinMode(3, INPUT);  // Plus-Button
  pinMode(11, OUTPUT);  // Trigger-Output (sturdy connection)
  pinMode(12, OUTPUT);  // Trigger-Output (loose connection)

  // setup OLED-Panel
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
  
  // Starting Screen
  u8g2.clearBuffer();
  u8g2.drawStr(12, 28, "HFS-Stim");
  add_button_labels();
  u8g2.sendBuffer();

  t_last_active = millis();

  // Deactivate HFS-Stimulator via loose connection (inverse logic)
  digitalWrite(12, HIGH);
  // Deactivate HFS-Stimulator via sturdy connection
  digitalWrite(11, LOW);
}

void loop(void) {
  // put your main code here, to run repeatedly:
  
  // ScreenSaver after reaching sleep_timer
  if(millis() - t_last_active > val_arr[4] * 1000 && sleeping==false){
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    sleeping = true;
  }

  if(digitalRead(2)==1){
    t_last_active = millis();
    u8g2.clearBuffer();
    add_button_labels();
    if(sleeping==false){
      val_arr[opt_idx] -= 1;
      update_options();
    }else{
      u8g2.drawStr(12, 28, "HFS-Stim");
      sleeping = false;
    }
    u8g2.sendBuffer();
  }

  if(digitalRead(3)==1){
    t_last_active = millis();
    u8g2.clearBuffer();
    add_button_labels();
    if(sleeping==false){
      val_arr[opt_idx] += 1;
      update_options();
    }else{
      u8g2.drawStr(12, 28, "HFS-Stim");
      sleeping = false;
    }
    u8g2.sendBuffer();
  }

   if(digitalRead(A0)==1){
    t_last_active = millis();
    u8g2.clearBuffer();
    add_button_labels();
    if(sleeping==false){
      u8g2.clearBuffer();
      u8g2.drawStr(30, 20, "Stimulating");
      u8g2.sendBuffer();
      delay(2000);
      for(int r=1; r<=val_arr[3]; r++){
        u8g2.clearBuffer();
        u8g2.drawStr(30, 20, "Stimulating");
        String val_str = String(r);
        char val_char[val_str.length() + 1];
        val_str.toCharArray(val_char, val_str.length() + 1);
        u8g2.drawStr(60, 40, val_char);
        u8g2.sendBuffer();

        // Stimulating with Frequency (val_arr[0]) for Length val_arr[1] with interval val_arr[2] for val_arr[3] times
        int count = val_arr[0] * (1000 / val_arr[1]);

        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
        delay(500);
        digitalWrite(11, LOW);
        digitalWrite(12, HIGH);
        delay(500);

        bool terminated = false;
        for(int t=0; t<1000; t++){
          if(digitalRead(A0)==1 || digitalRead(A1)==1 || digitalRead(2)==1 || digitalRead(3)==1){
            terminated = true;
            break;
          }
          // Wait interval
          delay(val_arr[2]);
        }
        if(terminated==true){
          u8g2.clearBuffer();
          u8g2.drawStr(60, 30, "Terminated");
          u8g2.sendBuffer();
          delay(1000);
          break;
        }
      }
       
      u8g2.clearBuffer();
      u8g2.drawStr(60, 30, "Finished");
      u8g2.sendBuffer();
      delay(1000);
      u8g2.clearBuffer();
      add_button_labels();
      u8g2.drawStr(12, 28, "HFS-Stim");
      t_last_active = millis();

    }else{
      u8g2.drawStr(12, 28, "HFS-Stim");
      sleeping = false;
    }
    u8g2.sendBuffer();
    delay(500);
  }

  if(digitalRead(A1)==1){
    t_last_active = millis();
    u8g2.clearBuffer();
    add_button_labels();
    if(sleeping==false){
      opt_idx += 1;
      if(opt_idx > 4){
        opt_idx = 0;
      }
      update_options();
    }else{
      u8g2.drawStr(12, 28, "HFS-Stim");
      sleeping = false;
    }
    u8g2.sendBuffer();
    delay(500);
  }
  
}
