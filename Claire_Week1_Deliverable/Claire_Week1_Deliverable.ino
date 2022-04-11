/**
Make sure that the Keypad library is installed. Look for Keypad library by Mark Stanly, Alexander Brevig, Version 3.1.0
**/
#include <Keypad.h>
#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <Wire.h>

uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.

char network[] = "MIT";
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
int correctPasscode[KEY_LENGTH] = {1,2,3,4};

int numCoded = 0;
int done = 0;
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

void postUpdate(int alarm_status){
  Serial.println("start post update");
  sprintf(body, "passcode=");
  Serial.println(body);
  char currPasscode[2];
  for(int i = 0;i < KEY_LENGTH; i++){
    sprintf(currPasscode,"%d",passcode[i]);
    strcat(body,currPasscode);
  }
  Serial.println(body);
  if(alarm_status == 1){
    strcat(body, "&alarm_status=1&is_active=1"); //generate body, posting temp, humidity to server
  }
  else if(alarm_status == 0){
    strcat(body, "&alarm_status=0&is_active=1"); //generate body, posting temp, humidity to server
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
}

void checkCode(){
  char key = keypad.getKey();

  if (key && key != 'A' && key != 'B' && key != 'C' && key != 'D' &&key != '*' && key != '#') {
    passcode[numCoded] = key-'0';
    if(numCoded == 3){
      done = 1;
      numCoded = 0;
    }
    else{
      numCoded += 1;
    }
  }
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
void playAlarm(int timeElapsed){
  alarmStart = millis();
  while(millis()-alarmStart < timeElapsed){
    //play tone
    playTone();
  }
  ledcWriteTone(AUDIO_PWM,0);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  alarm_on = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  checkCode();
  if(done == 1){
    bool correct = true;
    for(int i = 0;i < 4; i++){
      Serial.println(passcode[i]);
      if(passcode[i] != correctPasscode[i]){
        correct = false;
      }
    }
    if(correct == true){
      alarmStart = millis();
      alarm_on=1;
      postUpdate(1);
      playAlarm(5000);
      postUpdate(0);
      alarm_on = 0;
    }
    done = 0;
  }
}
