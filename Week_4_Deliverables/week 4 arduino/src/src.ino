#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
unsigned long button_timer;
const int BUTTON_TIMEOUT = 50;
const int input1 = 45; //entering numbers
const int input2 = 39; //pressure increase
const int input3 = 37; //pressure decrease, yes, correct check
const int input4 = 38; //no, incorrect check, unlock mode
uint8_t state;
int update;
float old_weight;
float curr_weight;
int THRESHOLD = 5;

//calibration set up
#include <HX711_ADC.h>
#include <Keypad.h>
#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <Wire.h>
#include <ArduinoJson.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 21; //mcu > HX711 dout pin
const int HX711_sck = 20; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.

char network[] = "EECS_Labs"; //change as needed
char password[] = "";

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
char key;

byte pin_rows[ROW_NUM]      = {11, 10, 9, 8}; // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
byte pin_column[COLUMN_NUM] = {7, 6, 5, 4};   // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const int KEY_LENGTH = 4;

int passcode[KEY_LENGTH + 1];
int correctPasscode[KEY_LENGTH + 1];
int numCoded;

long lastRemoteCheck;
long timer;
long timer2;
long lastTone;
long display;
const int REMOTE_CHECK_PERIOD = 1000;

//states 
#define REST 0
#define PC1 1
#define PM1 2
#define PM2 3
#define PM3 4
#define PM4 5
#define LOCKED 6
#define PC2 7
#define ALARM 8
#define AS1 9
#define AC1 10
#define AS2 11
#define AC2 12
#define AS3 13
#define AC3 14
#define AS4 15
#define AC4 16
#define UM1 17
#define UC1 18
#define UM2 19
#define UC2 20
#define UM3 21
#define UC3 22
#define UM4 23
#define UC4 24
#define USERNAME 25
#define PASSWORD 26
#define LOGIN 27
#define BADLOGIN 28
#define WAIT 29

int alarmStart = -1;

uint8_t AUDIO_PWM = 1;
uint8_t AUDIO_TRANSDUCER = 14;


char body[100]; //for body
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //ms to wait between posting step


const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

int alarm_on;

//user is yechengz
//char userID[] = "PFILTZCSYTZQLKWWDOXCXXDFKQMIWB";
char user_id[50];
char username[50];
char loginPassword[50];
char* user;
char* pwd;
int prev_press;
int numPresses;
char message[200];

void playTone(){
  ledcWriteTone(AUDIO_PWM,350);
  long primary_timer = millis();
  while(millis()-primary_timer <100&&alarm_on==1);
  if(alarm_on == 1){
    ledcWriteTone(AUDIO_PWM, 100);
    primary_timer = millis();
    while(millis()-primary_timer <100&&alarm_on == 1);
  }
}

void setup() {
  // put your setup code here, to run once:
  //calibration set up 
  Serial.begin(115200); delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(20.68); // user set calibration value (float), initial value 1.0 may be used for this sketch
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());

  //button set up
  tft.init();
  tft.setRotation(2);
  resetScreen(tft);
  welcome(tft);
  Serial.println("Welcome to PackMat");
  Serial.begin(115200);

  state = USERNAME;
  pinMode(input1, INPUT_PULLUP);
  pinMode(input2, INPUT_PULLUP);
  pinMode(input3, INPUT_PULLUP);
  pinMode(input4, INPUT_PULLUP);

  update = 1;

  pinMode(AUDIO_TRANSDUCER, OUTPUT);

  //set up AUDIO_PWM which we will control in this lab for music:
  ledcSetup(AUDIO_PWM, 0, 12);//12 bits of PWM precision
  ledcWrite(AUDIO_PWM, 0); //0 is a 0% duty cycle for the NFET
  ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);

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
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  for (int i = 0; i < KEY_LENGTH; i++) {
    correctPasscode[i] = 0;
    correctPasscode[i+1] = '\0';
  }
  
  alarm_on = 0;
  numCoded = 0;
  lastRemoteCheck = millis();
  timer = millis();
  timer2 = millis();
  lastTone = millis();
  user = username;
  pwd = loginPassword;
  numPresses = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  //get weight:
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      curr_weight = LoadCell.getData();
      //Serial.print("Load_cell output val: ");
      Serial.println(curr_weight);
      newDataReady = 0;
      t = millis();
    }
  }

  //end get weight
  packmat(digitalRead(input1), digitalRead(input2), digitalRead(input3), digitalRead(input4));

  //update old weight
  old_weight = curr_weight;
}
