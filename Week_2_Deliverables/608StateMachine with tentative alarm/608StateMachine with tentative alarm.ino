#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
unsigned long button_timer;
const int BUTTON_TIMEOUT = 50;
const int input1 = 45; //entering numbers
const int input2 = 39; //pressure increase
const int input3 = 38; //pressure decrease, yes, correct check
const int input4 = 37; //no, incorrect check, unlock mode
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

char network[] = "MIT"; //change as needed
char password[] = "";

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM]      = {11, 10, 9, 8}; // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
byte pin_column[COLUMN_NUM] = {7, 6, 5, 4};   // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const int KEY_LENGTH = 4;

int passcode[KEY_LENGTH];
int correctPasscode[KEY_LENGTH];
int numCoded;

long lastRemoteCheck;
long timer;
long lastTone;
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

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}

/*----------------------------------
 * do_http_request Function:
 * Arguments:
 *    const char* host: null-terminated char-array containing host to connect to
 *    char* request: null-terminated char-arry containing properly formatted HTTP request
 *    char* response: char-array used as output for function to contain response
 *    uint16_t response_size: size of response buffer (in bytes)
 *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
 *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
 * Return value:
 *    void (none)
 */
void do_http_request(const char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}        

void postUpdate(int alarm_status, int is_active){
  Serial.println("start post update");
  sprintf(body, "passcode=");
  Serial.println(body);
  char currPasscode[2];
  for(int i = 0;i < KEY_LENGTH; i++){
    sprintf(currPasscode,"%d",correctPasscode[i]);
    strcat(body,currPasscode);
  }
  Serial.println(body);
  if(alarm_status == 1){
    if (is_active == 1) {
      strcat(body, "&alarm_status=1&is_active=1");
    } else {
      strcat(body, "&alarm_status=1&is_active=0");
    }
  }
  else if(alarm_status == 0){
    if (is_active == 1) {
      strcat(body, "&alarm_status=0&is_active=1");
    } else {
      strcat(body, "&alarm_status=0&is_active=0");
    }
  }
  Serial.println(body);
  int body_len = strlen(body); //calculate body length (for header reporting)
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/yechengz/packmat/request_handler.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer, "\r\n"); //new line from header to body
  strcat(request_buffer, body); //body
  strcat(request_buffer, "\r\n"); //new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer); //viewable in Serial Terminal
  const int capacity = 300;
  StaticJsonDocument<capacity> doc;
  deserializeJson(doc,response_buffer);
  int status = doc["alarm_status"];
  alarm_on = status;
}

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

bool comparePasscodes(){
  bool correct = true;
    for(int i = 0;i < 4; i++){
      Serial.println(passcode[i]);
      if(passcode[i] != correctPasscode[i]){
        correct = false;
      }
    }
  return correct;
}

int checkWebsiteAlarmStatus(){
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/yechengz/packmat/request_handler.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
  //Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  //Serial.println(response_buffer); //viewable in Serial Terminal
  const int capacity = 300;
  StaticJsonDocument<capacity> doc;
  deserializeJson(doc,response_buffer);
  int alarm_status = doc["alarm_status"];
  return alarm_status;
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
    LoadCell.setCalFactor(1.0); // user set calibration value (float), initial value 1.0 may be used for this sketch
    Serial.println("Startup is complete");
  }
  while (!LoadCell.update());
  calibrate(); //start calibration procedure

  //button set up
  tft.init();
  tft.setRotation(2);
  tft.print("Welcome to PackMat");
  Serial.println("Welcome to PackMat");
  Serial.begin(115200);

  state = REST;
  pinMode(input1, INPUT_PULLUP);
  pinMode(input2, INPUT_PULLUP);
  pinMode(input3, INPUT_PULLUP);
  pinMode(input4, INPUT_PULLUP);

  print_message("REST");
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
  }

  postUpdate(0, 0);
  alarm_on = 0;
  numCoded = 0;
  lastRemoteCheck = millis();
  timer = millis();
  lastTone = millis();
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
      //Serial.println(curr_weight);
      newDataReady = 0;
      t = millis();
    }
  }

  // receive command from serial terminal
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay(); //tare
    else if (inByte == 'r') calibrate(); //calibrate
    else if (inByte == 'c') changeSavedCalFactor(); //edit calibration value manually
  }

  // check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

  //end get weight
  packmat(digitalRead(input1), digitalRead(input2), digitalRead(input3), digitalRead(input4));

  //update old weight
  old_weight = curr_weight;

}

//1: entering numbers/unlock mode, 2: pressure increase, 3: pressure decrease/yes/correct, 4: no/incorrect
void packmat(int input1, int input2, int input3, int input4){
  char key = keypad.getKey();
  //Serial.println(curr_weight);
  switch(state){
    case REST:
      ledcWriteTone(AUDIO_PWM,0);
      numCoded = 0;
      if(update){ //so only prints once
        Serial.println("In Rest State"); 
        print_message("REST");
        update = 0;
        postUpdate(0, 0);
      }
      else{
        state = PC1;
        update = 1;
      }
      /*if(curr_weight - old_weight>THRESHOLD){//pressure increase
        Serial.println("Pressure increase detected.");
        Serial.println("Switching to package confirmation 1 state");
        //print_message("PC1");
        update = 1;
        //delay(150);
        state = PC1;
      }*/
      break;
    
    case PC1:
      if(update){ //so only prints once
        Serial.println("In PC1 State"); 
        print_message("PC1");
        update = 0;
      }
      if(!input3){//yes
        Serial.println("Package Confirmed");
        Serial.println("Switching to Program Mode 1");
        //print_message("PM1");
        update = 1;
        //delay(150);
        state = PM1;
      }else if(!input4){//no
        Serial.println("No Package");
        Serial.println("Switching to Rest");
        //print_message("Rest");
        update = 1;
        //delay(150);
        state = REST;
      }
      break;

    case PM1:
      if(update){ //so only prints once
        Serial.println("In PM1 State"); 
        print_message("PM1");
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("First number entered");
        Serial.println("Switching to Program Mode 2");
        //print_message("PM2");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        Serial.println(correctPasscode[numCoded]);
        numCoded += 1;
        update = 1;
        //delay(150);
        state = PM2;
      }
      break;

    case PM2:
      if(update){ //so only prints once
        Serial.println("In PM2 State"); 
        print_message("PM2");
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("Second number entered");
        Serial.println("Switching to Program Mode 3");
        //print_message("PM3");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        Serial.println(correctPasscode[numCoded]);
        numCoded += 1;
        update = 1;
        //delay(150);
        state = PM3;
      }
      break;

    case PM3:
      if(update){ //so only prints once
        Serial.println("In PM3 State"); 
        print_message("PM3");
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("Third number entered");
        Serial.println("Switching to Program Mode 4");
        //print_message("PM4");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        Serial.println(correctPasscode[numCoded]);
        numCoded += 1;
        update = 1;
        //delay(150);
        state = PM4;
      }
      break;

    case PM4:
      if(update){ //so only prints once
        Serial.println("In PM4 State"); 
        print_message("PM4");
        update = 0;
      }
      if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
        Serial.println("Fourth number entered");
        Serial.println("Switching to Locked");
        //print_message("LOCKED");
        Serial.println(key);
        correctPasscode[numCoded] = key -'0';
        Serial.println(correctPasscode[numCoded]);
        postUpdate(0, 1);
        numCoded = 0;
        update = 1;
        //delay(150);
        state = LOCKED;
      }
      break;

    case LOCKED:
      if(update){ //so only prints once
        Serial.println("In LOCKED State"); 
        print_message("LOCKED");
        postUpdate(0, 1);
        numCoded = 0;
        update = 0;
      }
      if((curr_weight - old_weight)>THRESHOLD){//pressure increase
        Serial.println("Pressure Increase Detected");
        Serial.println("Swithcing to Package Confirmation 2");
        //print_message("PC2");
        update = 1;
        //delay(150);
        state = PC2;
      }else if(old_weight - curr_weight>THRESHOLD){ //pressure decrease
        Serial.println("Pressure Decrease Detected");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        //delay(150);
        state = ALARM;
      }else if(!input4){//unlock mode
        Serial.println("Entering Unlock mode");
        Serial.println("Switching to unlock mode 1");
        //print_message("UM1");
        update = 1;
        //delay(150);
        state = UM1;
      }
      break;
    
    case PC2:
      if(update){ //so only prints once
        Serial.println("In PC2 State"); 
        print_message("PC2");
        update = 0;
      }
      if(!input3){//yes
        Serial.println("Package Confirmed");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        //delay(150);
        state = LOCKED;
      }else if(!input4){//no
        Serial.println("No Package");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        alarmStart = millis();
        alarm_on = 1;
        postUpdate(1, 1);
        playTone();
        update = 1;
        //delay(150);
        state = ALARM;
      }
      break;

    case ALARM:
      playTone();
      numCoded = 0;
      if(update){ //so only prints once
        Serial.println("In ALARM State"); 
        print_message("ALARM");
        postUpdate(1, 1);
        alarm_on=1;
        update = 0;
      }
      if (!input4){//stop alarm/unlock
        Serial.println("Attempting to turn off alarm");
        Serial.println("Switching to Alarm Stop 1");
        //print_message("AS1");
        update = 1;
        //delay(150);
        state = AS1;
      }
      break;
    
    case AS1:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 1 State"); 
        print_message("AS1");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("First number entered");
        Serial.println("Switching to Alarm Check 1");
        //print_message("AC1");
        passcode[numCoded] = key-'0';
        numCoded += 1;
        update = 1;
        ////delay(150);
        state = AC1;
      }
      
      break;

    case AC1:

      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 1 State"); 
        print_message("AC1");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(passcode[0] == correctPasscode[0]){ //correct
        Serial.println("Correct first number");
        Serial.println("Switching to Alarm Stop 2");
        //print_message("+AS2"); //+means correct
        update = 1;
        //delay(150);
        state = AS2;
      }else if(passcode[0] != correctPasscode[0]){//incorrect
        Serial.println("Incorrect first number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        //delay(150);
        state = ALARM;
      }
      break;

    case AS2:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 2 State"); 
        print_message("AS2");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("Second number entered");
        Serial.println("Switching to Alarm Check 2");
        //print_message("AC2");
        passcode[numCoded] = key-'0';
        numCoded += 1;
        update = 1;
        //delay(150);
        state = AC2;
      }
      break;

    case AC2:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 2 State"); 
        print_message("AC2");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(passcode[1] == correctPasscode[1]){ //correct
        Serial.println("Correct second number");
        Serial.println("Switching to AS3");
        //print_message("+AS3"); //+means correct
        update = 1;
        //delay(150);
        state = AS3;
      }else if(passcode[1] != correctPasscode[1]){//incorrect
        Serial.println("Incorrect second number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        //delay(150);
        state = ALARM;
      }
      break;

    case AS3:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 3 State"); 
        print_message("AS3");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("Third number entered");
        Serial.println("Switching to Alarm Check 3");
        //print_message("AC3");
        passcode[numCoded] = key-'0';
        numCoded += 1;
        update = 1;
        //delay(150);
        state = AC3;
      }
      break;

    case AC3:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 3 State"); 
        print_message("AC3");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(passcode[2] == correctPasscode[2]){ //correct
        Serial.println("Correct third number");
        Serial.println("Switching to AS4");
        //print_message("+AS4"); //+means correct
        update = 1;
        //delay(150);
        state = AS4;
      }else if(passcode[2] == correctPasscode[2]){//incorrect
        Serial.println("Incorrect third number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        //delay(150);
        state = ALARM;
      }
      break;

    case AS4:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 4 State"); 
        print_message("AS4");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(alarm_on == 0){
        state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
        Serial.println("Fourth number entered");
        Serial.println("Switching to Alarm Check 4");
        //print_message("AC4");
        passcode[numCoded] = key-'0';
        numCoded = 0;
        update = 1;
        //delay(150);
        state = AC4;
      }
      break;

    case AC4:
      if(millis()-lastTone >= 200){
        lastTone = millis();
        playTone();
      }
      if(update){ //so only prints once
        Serial.println("In Alarm Check 4 State"); 
        print_message("AC4");
        update = 0;
      }
      if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
        if(checkWebsiteAlarmStatus() == 0){
          //alarm turns off so switch to state REST
          state = REST;
          //reset the numCoded 
          numCoded = 0;
          break;
        }
        else{
          lastRemoteCheck = millis();
        }
      }
      if(passcode[3] == correctPasscode[3]){ //correct
        Serial.println("Correct fourth number");
        Serial.println("Switching to REST");
        //print_message("+REST"); //+means correct
        update = 1;
        //delay(150);
        alarm_on = 0;
        state = REST;
      }else if(passcode[3] != correctPasscode[3]){//incorrect
        Serial.println("Incorrect fourth number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        //delay(150);
        state = ALARM;
      }
      break;
    
    case UM1:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 1 State"); 
        print_message("UM1");
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("First number entered");
        Serial.println("Switching to Unlock Check 1");
        passcode[numCoded]=key-'0';
        numCoded += 1;
        //print_message("UC1");
        update = 1;
        //delay(150);
        state = UC1;
      }
      break;
    
    case UC1:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 1 State"); 
        print_message("UC1");
        update = 0;
      }
      if(passcode[0] == correctPasscode[0]){//correct
        Serial.println("Correct first number");
        Serial.println("Switching to Unlock Mode 2");
        //print_message("+UM2");
        update = 1;
        //delay(150);
        state = UM2;
      }else if(passcode[0] != correctPasscode[0]){//incorrect
        Serial.println("Incorrect first number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        //delay(150);
        state = LOCKED;
      }
      break;

    case UM2:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 2 State"); 
        print_message("UM2");
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("Second number entered");
        Serial.println("Switching to Unlock Check 2");
        passcode[numCoded] = key-'0';
        numCoded += 1;
        //print_message("UC2");
        update = 1;
        //delay(150);
        state = UC2;
      }
      break;
    
    case UC2:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 2 State"); 
        print_message("UC2");
        update = 0;
      }
      if(passcode[1] == correctPasscode[1]){//correct
        Serial.println("Correct second number");
        Serial.println("Switching to Unlock Mode 3");
        //print_message("+UM3");
        update = 1;
        //delay(150);
        state = UM3;
      }else if(passcode[1] != correctPasscode[1]){//incorrect
        Serial.println("Incorrect second number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        //delay(150);
        state = LOCKED;
      }
      break;

    case UM3:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 3 State"); 
        print_message("UM3");
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("Third number entered");
        Serial.println("Switching to Unlock Check 3");
        //print_message("UC3");
        passcode[numCoded] = key-'0';
        numCoded += 1;
        update = 1;
        //delay(150);
        state = UC3;
      }
      break;
    
    case UC3:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 3 State"); 
        print_message("UC3");
        update = 0;
      }
      if(passcode[2] == correctPasscode[2]){//correct
        Serial.println("Correct third number");
        Serial.println("Switching to Unlock Mode 4");
        //print_message("+UM4");
        update = 1;
        //delay(150);
        state = UM4;
      }else if(passcode[2] != correctPasscode[2]){//incorrect
        Serial.println("Incorrect third number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        //delay(150);
        state = LOCKED;
      }
      break;

    case UM4:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 4 State"); 
        print_message("UM4");
        update = 0;
      }
      if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
        Serial.println("Fourth number entered");
        Serial.println("Switching to Unlock Check 4");
        //print_message("UC4");
        passcode[numCoded] = key-'0';
        numCoded = 0;
        update = 1;
        //delay(150);
        state = UC4;
      }
      break;
    
    case UC4:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 4 State"); 
        print_message("UC4");
        update = 0;
      }
      if(passcode[3] == correctPasscode[3]){//correct
        Serial.println("Correct fourth number");
        Serial.println("Switching to REST");
        //print_message("+REST");
        update = 1;
        //delay(150);
        state = REST;
      }else if(passcode[3] != correctPasscode[3]){//incorrect
        Serial.println("Incorrect fourth number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        //delay(150);
        state = LOCKED;
      }
      break;

  }
}

void print_message(const char* message){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(4);
  tft.println(message);
}

void calibrate() {
  Serial.println("***");
  Serial.println("Start calibration:");
  Serial.println("Place the load cell an a level stable surface.");
  Serial.println("Remove any load applied to the load cell.");
  Serial.println("Send 't' from serial monitor to set the tare offset.");

  boolean _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte == 't') LoadCell.tareNoDelay();
      }
    }
    if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
      _resume = true;
    }
  }

  Serial.println("Now, place your known mass on the loadcell.");
  Serial.println("Then send the weight of this mass (i.e. 100.0) from serial monitor.");

  float known_mass = 0;
  _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }

  LoadCell.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass); //get the new calibration value

  Serial.print("New calibration value has been set to: ");
  Serial.print(newCalibrationValue);
  Serial.println(", use this as calibration value (calFactor) in your project sketch.");
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");

  _resume = false;
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);
#endif
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.commit();
#endif
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Value ");
        Serial.print(newCalibrationValue);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(calVal_eepromAdress);
        _resume = true;

      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        _resume = true;
      }
    }
  }

  Serial.println("End calibration");
  Serial.println("***");
  Serial.println("To re-calibrate, send 'r' from serial monitor.");
  Serial.println("For manual edit of the calibration value, send 'c' from serial monitor.");
  Serial.println("***");
}

void changeSavedCalFactor() {
  float oldCalibrationValue = LoadCell.getCalFactor();
  boolean _resume = false;
  Serial.println("***");
  Serial.print("Current value is: ");
  Serial.println(oldCalibrationValue);
  Serial.println("Now, send the new value from serial monitor, i.e. 696.0");
  float newCalibrationValue;
  while (_resume == false) {
    if (Serial.available() > 0) {
      newCalibrationValue = Serial.parseFloat();
      if (newCalibrationValue != 0) {
        Serial.print("New calibration value is: ");
        Serial.println(newCalibrationValue);
        LoadCell.setCalFactor(newCalibrationValue);
        _resume = true;
      }
    }
  }
  _resume = false;
  Serial.print("Save this value to EEPROM adress ");
  Serial.print(calVal_eepromAdress);
  Serial.println("? y/n");
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.begin(512);
#endif
        EEPROM.put(calVal_eepromAdress, newCalibrationValue);
#if defined(ESP8266)|| defined(ESP32)
        EEPROM.commit();
#endif
        EEPROM.get(calVal_eepromAdress, newCalibrationValue);
        Serial.print("Value ");
        Serial.print(newCalibrationValue);
        Serial.print(" saved to EEPROM address: ");
        Serial.println(calVal_eepromAdress);
        _resume = true;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        _resume = true;
      }
    }
  }
  Serial.println("End change calibration value");
  Serial.println("***");
}
