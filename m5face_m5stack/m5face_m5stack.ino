//## Copyright (c) 2019 aNoken
// Licensed under the MIT license

#include <M5Stack.h>
#include "avator.h"
#include "AquesTalkTTS.h"
#include <WiFi.h>

unsigned long time_log = 0;
int okawari_on_flg = 1;
int acc_use = 0;
int delayval = 10; 
Avator *avator;
int count = 0;
float f = 0;
float last = 0;
long messageSentAt = 0;
int talk_mode = 0;
int talk_speed = 100;
float M5Stack_rad_stock = 0;

const Expression expressions[] = {
  Angry,
  Sad,
  Neutral
};
const int expressionsSize = sizeof(expressions) / sizeof(Expression);
int idx = 0;
bool flg = 0;

float theta_tate = 0;
float theta_yoko = 0;
int button_swich = 0;



static unsigned long shutdown_dt = 0;
static unsigned long shutdown_dt_old = 0;

static unsigned long start_dt = 0;
static unsigned long start_dt_old = 0;
static int initial_flg = 0;

double thresh_value = 0;

HardwareSerial serial_ext(2);

int face_proc(int face_cnt);

void drawLoop(void *args) {
  for (;;)  {
    int level = TTS.getLevel();
    float f = level / 10000.0;
    float open = 0.0 ;
    if (f < 2.0)  open = f;
    last = f;
    avator->setMouthOpen(open);
    avator->draw();
    delay(50);
  }
}

void saccade(void *args) {
  int c = 0;
  int c2 = 0;
  static int face_cnt = 0;

  for (;;)  {
    c = (c + 1) % 100;
    float eye_f = sin(c * 2.0 * PI / 10.0) * 2.0;
    avator->setGaze(eye_f, 0);

    eye_f = sin(c * 2.0 * PI / 100.0) * 1.0;
    if (eye_f > -0.75)    avator->setEyeOpen(1);
    else   avator->setEyeOpen(0);


    c2 = c2 + 1 % 100;
    float f = sin(c2 * 2 * PI / 50.0);
    avator->setBreath(f);

    if ( serial_ext.available() > 0 ) {
      String str = serial_ext.readStringUntil('\n');
      int value = str.toInt();

      unsigned long dtt =  millis() - time_log;
      time_log = millis();
      if ( value > thresh_value) avator->area_h =  240;
      else avator->area_h = 0;

      avator->value = value;
      avator->thresh = thresh_value;

      if (value < thresh_value) { 
        avator->anger_flg = 0;
        shutdown_dt += dtt;
        start_dt = 0;
      }
      else  if (value >= thresh_value) {   
        M5.Lcd.setBrightness(100);
        avator->anger_flg = 1;
        shutdown_dt = 0;
        start_dt += dtt;
      }
      Serial.print(value);
      Serial.print("_");
      Serial.print(thresh_value);
      Serial.print("_");
      Serial.println(avator->anger_flg );

      delay(10);
      M5.update();
      if (M5.BtnA.wasPressed())  {
        if (avator->speak_flg == 0) {
          avator->speak_flg = 1;
          Serial.println("speak_flg On");
        }
        else if (avator->speak_flg == 1) {
          avator->speak_flg = 0;
          avator->setExpression(expressions[idx]);
          idx = (idx + 1) % expressionsSize;
          Serial.println("speak_flg Off");
        }
        button_swich = 1;
      }
      if (M5.BtnB.wasPressed())  {
        face_proc(face_cnt);
        face_cnt++;
        if (face_cnt > 22)face_cnt = 0;
        button_swich = 2;

      }

      if (M5.BtnC.wasPressed())  {
        thresh_value += 50;
        if (thresh_value > 500)thresh_value = 50;
      }
    }
  }
}



void setup() {
  WiFi.disconnect(true);
  M5.begin();
  Wire.begin();
  serial_ext.begin(115200, SERIAL_8N1, 17, 16);
  thresh_value = 200;

  int iret = TTS.create("XXXXXXXXXXXXXXXX");
  if (iret) {
    Serial.print("ERR: TTS_createK():");
    Serial.println(iret);
  }

  talk_mode = 0;
  M5.Lcd.setBrightness(100);
  M5.Lcd.clear();

  if (acc_use) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(5);
    M5.Lcd.println("M5Stack start...");
  }
  M5.Lcd.clear();
  avator = new Avator();
  xTaskCreatePinnedToCore( drawLoop, "drawLoop", 4096, NULL,  1,  NULL,  0);
  xTaskCreatePinnedToCore( saccade,  "saccade", 4096,  NULL,  3,  NULL,  1);
  avator->speak_flg = 1;
}
void loop() {
  static unsigned long time_light = 0;
  if ((start_dt >= 1000) && (start_dt_old < 1000)) {
    if (avator->speak_flg ) {
      if (avator->anger_flg == 2 )
        TTS.play("kore,purin,to,tigau", 100);
      else {
        TTS.play("purin,gatorareta", 100);
      }
    }
    delay(3000);
  }
  else if (start_dt >= 1000)  {
    if (avator->speak_flg ) {
      if (avator->anger_flg == 2 )   TTS.play("kore,purin,to,tigau", 100);
      else        TTS.play("purin,kaeshitene", 100);
    }
    delay(3000);
  }
  if ((shutdown_dt >= 1000) && (shutdown_dt_old < 1000))  {
    if (avator->speak_flg ) {
      if (initial_flg != 0)  TTS.play("purin,gakaettekita", 100);
    }
    initial_flg++;
    delay(3000);
  }
  avator->M5Stack_rad = 0;
  float rot_deg = avator->M5Stack_rad  * 180.0 / PI;
  static float rot_deg_old = 0;
  rot_deg_old = rot_deg;
  start_dt_old = start_dt;
  shutdown_dt_old = shutdown_dt;
  delay(100);
}

int face_proc(int face_cnt) {
  if (face_cnt == 0)   {
    avator->PRIMARY_COLOR = TFT_BLACK;
    avator->SECONDARY_COLOR = TFT_RED;
  }
  else   if (face_cnt == 1) {
    avator->PRIMARY_COLOR = TFT_YELLOW;
    avator->SECONDARY_COLOR = TFT_BLACK;
  }
  else   if (face_cnt == 2) {
    avator->PRIMARY_COLOR = TFT_BLACK;
    avator->SECONDARY_COLOR = TFT_YELLOW;
  }
  else   if (face_cnt == 3) {
    avator->PRIMARY_COLOR = TFT_ORANGE;
    avator->SECONDARY_COLOR = TFT_BLACK;
  }
  else   if (face_cnt == 4) {
    avator->PRIMARY_COLOR = TFT_BLACK;
    avator->SECONDARY_COLOR = TFT_ORANGE;
  } 
}
