#include <WiFi.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <string.h>
#include "Button.h"

TFT_eSPI tft = TFT_eSPI();

char network[] = "MIT GUEST";
char password[] = "";

const int RESPONSE_TIMEOUT = 6000;

const uint16_t IN_BUFFER_SIZE = 1000; 
const uint16_t OUT_BUFFER_SIZE = 1000; 
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];

char output[100];
char alarm_status[10];
char passcode[10];

const uint8_t PASSCODEBUTTON = 45;
const uint8_t ALARMBUTTON = 39;

Button passcode_button(PASSCODEBUTTON);
Button alarm_button(ALARMBUTTON);

void setup(){
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
      WiFi.localIP()[1], WiFi.localIP()[0], WiFi.macAddress().c_str() , WiFi.SSID().c_str());    
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  pinMode(PASSCODEBUTTON, INPUT_PULLUP);
  pinMode(ALARMBUTTON, INPUT_PULLUP);

  tft.init(); 
  tft.setRotation(2);
  tft.setTextSize(1); 
  tft.fillScreen(TFT_BLACK); 
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  
  sprintf(alarm_status, "OFF");
  sprintf(passcode, "01234");
  sprintf(output, "Passcode: %s\r\nAlarm Status: %s", passcode, alarm_status);
  tft.setCursor(0,10,1);
  tft.println(output);
}

void loop() {
  passcode_button.read_update();
  alarm_button.read_update();

  if (passcode_button.button_pressed()) {
    toggle_passcode();
  }

  if (alarm_button.button_pressed()) {
    toggle_alarm();
  }
}

void toggle_alarm() {
  char body[100];
  if (strcmp(alarm_status, "ON") == 0) {
    sprintf(alarm_status, "OFF");
    sprintf(body,"is_active=1&alarm_status=0&passcode=%s", passcode);
  } else {
    sprintf(alarm_status, "ON");
    sprintf(body,"is_active=1&alarm_status=1&passcode=%s", passcode);
  }

  make_post(body);
}

void toggle_passcode(){
  char body[100];

  int alarm_code = 0;
  if (strcmp(alarm_status, "ON") == 0) {
    alarm_code = 1;
  }

  if (strcmp(passcode, "01234") == 0) {
    sprintf(passcode, "54321");
    sprintf(body,"is_active=1&alarm_status=%d&passcode=54321", alarm_code);
  } else {
    sprintf(passcode, "01234");
    sprintf(body,"is_active=1&alarm_status=%d&passcode=01234", alarm_code);
  }

  make_post(body);
}

void make_post(char* body) {
  int body_len = strlen(body);
  sprintf(request_buffer,"POST /sandbox/sc/yechengz/packmat/request_handler.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len);
  strcat(request_buffer,"\r\n");
  strcat(request_buffer,body);
  strcat(request_buffer,"\r\n");
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  Serial.println(response_buffer);

  sprintf(output, "Passcode: %s\r\nAlarm Status: %s", passcode, alarm_status);
  tft.fillScreen(TFT_BLACK); 
  tft.setCursor(0,10,1);
  tft.println(output);
}


