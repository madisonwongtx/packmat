// #include <TFT_eSPI.h>
// TFT_eSPI tft = TFT_eSPI();
// unsigned long button_timer;
// const int BUTTON_TIMEOUT = 50;
// const int input1 = 45; //entering numbers
// const int input2 = 39; //pressure increase
// const int input3 = 37; //pressure decrease, yes, correct check
// const int input4 = 38; //no, incorrect check, unlock mode
// uint8_t state;
// int update;
// float old_weight;
// float curr_weight;
// int THRESHOLD = 5;

// //calibration set up
// #include <HX711_ADC.h>
// #include <Keypad.h>
// #include <WiFi.h> //Connect to WiFi Network
// #include <SPI.h> //Used in support of TFT Display
// #include <string.h>  //used for some string handling and processing.
// #include <Wire.h>
// #include <ArduinoJson.h>
// #if defined(ESP8266)|| defined(ESP32) || defined(AVR)
// #include <EEPROM.h>
// #endif

// //pins:
// const int HX711_dout = 21; //mcu > HX711 dout pin
// const int HX711_sck = 20; //mcu > HX711 sck pin

// //HX711 constructor:
// HX711_ADC LoadCell(HX711_dout, HX711_sck);

// const int calVal_eepromAdress = 0;
// unsigned long t = 0;

// uint8_t channel = 1; //network channel on 2.4 GHz
// byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.

// char network[] = "MIT GUEST"; //change as needed
// char password[] = "";

// const int ROW_NUM = 4;
// const int COLUMN_NUM = 4;

// char keys[ROW_NUM][COLUMN_NUM] = {
//   {'1', '2', '3', 'A'},
//   {'4', '5', '6', 'B'},
//   {'7', '8', '9', 'C'},
//   {'*', '0', '#', 'D'}
// };

// byte pin_rows[ROW_NUM]      = {11, 10, 9, 8}; // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
// byte pin_column[COLUMN_NUM] = {7, 6, 5, 4};   // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

// Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// const int KEY_LENGTH = 4;

// int passcode[KEY_LENGTH];
// int correctPasscode[KEY_LENGTH];
// int numCoded;

// long lastRemoteCheck;
// long timer;
// long lastTone;
// long display;
// const int REMOTE_CHECK_PERIOD = 1000;

// //states 
// #define REST 0
// #define PC1 1
// #define PM1 2
// #define PM2 3
// #define PM3 4
// #define PM4 5
// #define LOCKED 6
// #define PC2 7
// #define ALARM 8
// #define AS1 9
// #define AC1 10
// #define AS2 11
// #define AC2 12
// #define AS3 13
// #define AC3 14
// #define AS4 15
// #define AC4 16
// #define UM1 17
// #define UC1 18
// #define UM2 19
// #define UC2 20
// #define UM3 21
// #define UC3 22
// #define UM4 23
// #define UC4 24
// #define USERNAME 25
// #define PASSWORD 26
// #define LOGIN 27

// int alarmStart = -1;

// uint8_t AUDIO_PWM = 1;
// uint8_t AUDIO_TRANSDUCER = 14;


// char body[100]; //for body
// const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
// const int POSTING_PERIOD = 6000; //ms to wait between posting step


// const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
// const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
// char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
// char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

// int alarm_on;

// //user is yechengz
// //char userID[] = "PFILTZCSYTZQLKWWDOXCXXDFKQMIWB";
// char* user_id;
// char username[50];
// char loginPassword[50];
// char* user;
// char* pwd;
// int prev_press;
// int numPresses;


// uint8_t char_append(char* buff, char c, uint16_t buff_size) {
//         int len = strlen(buff);
//         if (len>buff_size) return false;
//         buff[len] = c;
//         buff[len+1] = '\0';
//         return true;
// }

// /*----------------------------------
//  * do_http_request Function:
//  * Arguments:
//  *    const char* host: null-terminated char-array containing host to connect to
//  *    char* request: null-terminated char-arry containing properly formatted HTTP request
//  *    char* response: char-array used as output for function to contain response
//  *    uint16_t response_size: size of response buffer (in bytes)
//  *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
//  *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
//  * Return value:
//  *    void (none)
//  */
// void do_http_request(const char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
//   WiFiClient client; //instantiate a client object
//   if (client.connect(host, 80)) { //try to connect to host on port 80
//     if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
//     client.print(request);
//     memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
//     uint32_t count = millis();
//     while (client.connected()) { //while we remain connected read out data coming back
//       client.readBytesUntil('\n',response,response_size);
//       if (serial) Serial.println(response);
//       if (strcmp(response,"\r")==0) { //found a blank line!
//         break;
//       }
//       memset(response, 0, response_size);
//       if (millis()-count>response_timeout) break;
//     }
//     memset(response, 0, response_size);  
//     count = millis();
//     while (client.available()) { //read out remaining text (body of response)
//       char_append(response,client.read(),OUT_BUFFER_SIZE);
//     }
//     if (serial) Serial.println(response);
//     client.stop();
//     if (serial) Serial.println("-----------");  
//   }else{
//     if (serial) Serial.println("connection failed :/");
//     if (serial) Serial.println("wait 0.5 sec...");
//     client.stop();
//   }
// }       

// void passwordKeyCheck(){
//   char key = keypad.getKey();

//   if (key && key != 'A' && key != 'B' && key != 'C' && key != 'D' &&key != '*' && key != '#') {
//     if(numPresses == 0){
//       numPresses += 1;
//       prev_press = key - '0';
//       sprintf(pwd, "%d",key - '0');
//     }
//     else if(prev_press == key - '0'){
//       numPresses += 1;
//       if(prev_press == 2){
//         if(numPresses == 2){
//           *pwd = 'A';
//         }
//         else if (numPresses == 3){
//           *pwd = 'B';
//         }
//         else if(numPresses == 4){
//           *pwd = 'C';
//         }
//       }
//       else if(prev_press == 3){
//         if(numPresses == 2){
//           *pwd = 'D';
//         }
//         else if (numPresses == 3){
//           *pwd = 'E';
//         }
//         else if(numPresses == 4){
//           *pwd = 'F';
//         }
//       }
//       else if(prev_press == 4){
//         if(numPresses == 2){
//           *pwd = 'G';
//         }
//         else if (numPresses == 3){
//           *pwd = 'H';
//         }
//         else if(numPresses == 4){
//           *pwd = 'I';
//         }
//       }
//       else if(prev_press == 5){
//         if(numPresses == 2){
//           *pwd = 'J';
//         }
//         else if (numPresses == 3){
//           *pwd = 'K';
//         }
//         else if(numPresses == 4){
//           *pwd = 'L';
//         }
//       }
//       else if(prev_press == 6){
//         if(numPresses == 2){
//           *pwd = 'M';
//         }
//         else if (numPresses == 3){
//           *pwd = 'N';
//         }
//         else if(numPresses == 4){
//           *pwd = 'O';
//         }
//       }
//       else if(prev_press == 7){
//         if(numPresses == 2){
//           *pwd = 'P';
//         }
//         else if (numPresses == 3){
//           *pwd = 'Q';
//         }
//         else if(numPresses == 4){
//           *pwd = 'R';
//         }
//         else if(numPresses == 5){
//           *pwd = 'S';
//         }
//       }
//       else if(prev_press == 8){
//         if(numPresses == 2){
//           *pwd = 'T';
//         }
//         else if (numPresses == 3){
//           *pwd = 'U';
//         }
//         else if(numPresses == 4){
//           *pwd = 'V';
//         }
//       }
//       else if(prev_press == 9){
//         if(numPresses == 2){
//           *pwd = 'W';
//         }
//         else if (numPresses == 3){
//           *pwd = 'X';
//         }
//         else if(numPresses == 4){
//           *pwd = 'Y';
//         }
//         else if(numPresses == 5){
//           *pwd = 'Z';
//         }
//       }
//     }
//     Serial.println(loginPassword);
//     tft.setCursor(0,0);
//     tft.fillScreen(TFT_BLACK);
//     tft.println(loginPassword);
//   }
// }

// void userKeyCheck(){
//   char key = keypad.getKey();

//   if (key && key != 'A' && key != 'B' && key != 'C' && key != 'D' &&key != '*' && key != '#') {
//     if(numPresses == 0){
//       numPresses += 1;
//       prev_press = key - '0';
//       sprintf(user, "%d",key - '0');
//     }
//     else if(prev_press == key - '0'){
//       numPresses += 1;
//       if(prev_press == 2){
//         if(numPresses == 2){
//           *user = 'A';
//         }
//         else if (numPresses == 3){
//           *user = 'B';
//         }
//         else if(numPresses == 4){
//           *user = 'C';
//         }
//       }
//       else if(prev_press == 3){
//         if(numPresses == 2){
//           *user = 'D';
//         }
//         else if (numPresses == 3){
//           *user = 'E';
//         }
//         else if(numPresses == 4){
//           *user = 'F';
//         }
//       }
//       else if(prev_press == 4){
//         if(numPresses == 2){
//           *user = 'G';
//         }
//         else if (numPresses == 3){
//           *user = 'H';
//         }
//         else if(numPresses == 4){
//           *user = 'I';
//         }
//       }
//       else if(prev_press == 5){
//         if(numPresses == 2){
//           *user = 'J';
//         }
//         else if (numPresses == 3){
//           *user = 'K';
//         }
//         else if(numPresses == 4){
//           *user = 'L';
//         }
//       }
//       else if(prev_press == 6){
//         if(numPresses == 2){
//           *user = 'M';
//         }
//         else if (numPresses == 3){
//           *user = 'N';
//         }
//         else if(numPresses == 4){
//           *user = 'O';
//         }
//       }
//       else if(prev_press == 7){
//         if(numPresses == 2){
//           *user = 'P';
//         }
//         else if (numPresses == 3){
//           *user = 'Q';
//         }
//         else if(numPresses == 4){
//           *user = 'R';
//         }
//         else if(numPresses == 5){
//           *user = 'S';
//         }
//       }
//       else if(prev_press == 8){
//         if(numPresses == 2){
//           *user = 'T';
//         }
//         else if (numPresses == 3){
//           *user = 'U';
//         }
//         else if(numPresses == 4){
//           *user = 'V';
//         }
//       }
//       else if(prev_press == 9){
//         if(numPresses == 2){
//           *user = 'W';
//         }
//         else if (numPresses == 3){
//           *user = 'X';
//         }
//         else if(numPresses == 4){
//           *user = 'Y';
//         }
//         else if(numPresses == 5){
//           *user = 'Z';
//         }
//       }
//     }
//     Serial.println(username);
//     tft.fillScreen(TFT_BLACK);
//     tft.setCursor(0,0);
//     tft.println(username);
//   }
// }

// void login(){
//   Serial.println("start post update");
//   sprintf(body, "username=");
//   strcat(body, username);
//   strcat(body, "&password=");
//   strcat(body,loginPassword);
//   Serial.println(body);
//   int body_len = strlen(body); //calculate body length (for header reporting)
//   sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/yechengz/packmat2/login.py HTTP/1.1\r\n");
//   strcat(request_buffer, "Host: 608dev-2.net\r\n");
//   strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
//   sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
//   strcat(request_buffer, "\r\n"); //new line from header to body
//   strcat(request_buffer, body); //body
//   strcat(request_buffer, "\r\n"); //new line
//   Serial.println(request_buffer);
//   do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//   Serial.println(response_buffer); //viewable in Serial Terminal
//   const int capacity = 300;
//   StaticJsonDocument<capacity> doc;
//   deserializeJson(doc,response_buffer);
//   const char * id = doc["user_id"];
//   user_id = (char*)id;
// }
// void create_account(){
//   Serial.println("start post update");
//   sprintf(body, "username=");
//   strcat(body, username);
//   strcat(body, "&password=");
//   strcat(body,loginPassword);
//   Serial.println(body);
//   int body_len = strlen(body); //calculate body length (for header reporting)
//   sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/yechengz/packmat2/create_account.py HTTP/1.1\r\n");
//   strcat(request_buffer, "Host: 608dev-2.net\r\n");
//   strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
//   sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
//   strcat(request_buffer, "\r\n"); //new line from header to body
//   strcat(request_buffer, body); //body
//   strcat(request_buffer, "\r\n"); //new line
//   Serial.println(request_buffer);
//   do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//   Serial.println(response_buffer); //viewable in Serial Terminal
// }

// void postUpdate(int alarm_status, int is_active){
//   Serial.println("start post update");
//   sprintf(body, "passcode=");
//   Serial.println(body);
//   char currPasscode[2];
//   for(int i = 0;i < KEY_LENGTH; i++){
//     sprintf(currPasscode,"%d",correctPasscode[i]);
//     strcat(body,currPasscode);
//   }
//   Serial.println(body);
//   if(alarm_status == 1){
//     if (is_active == 1) {
//       strcat(body, "&alarm_status=1&is_active=1");
//     } else {
//       strcat(body, "&alarm_status=1&is_active=0");
//     }
//   }
//   else if(alarm_status == 0){
//     if (is_active == 1) {
//       strcat(body, "&alarm_status=0&is_active=1");
//     } else {
//       strcat(body, "&alarm_status=0&is_active=0");
//     }
//   }
//   strcat(body, "&user_id=");
//   strcat(body, userID);
//   Serial.println(body);
//   int body_len = strlen(body); //calculate body length (for header reporting)
//   sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/yechengz/packmat2/request_handler.py HTTP/1.1\r\n");
//   strcat(request_buffer, "Host: 608dev-2.net\r\n");
//   strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
//   sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
//   strcat(request_buffer, "\r\n"); //new line from header to body
//   strcat(request_buffer, body); //body
//   strcat(request_buffer, "\r\n"); //new line
//   Serial.println(request_buffer);
//   do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//   Serial.println(response_buffer); //viewable in Serial Terminal
//   const int capacity = 300;
//   StaticJsonDocument<capacity> doc;
//   deserializeJson(doc,response_buffer);
//   int status = doc["alarm_status"];
//   alarm_on = status;
// }

// void playTone(){
//   ledcWriteTone(AUDIO_PWM,350);
//   long primary_timer = millis();
//   while(millis()-primary_timer <100&&alarm_on==1);
//   if(alarm_on == 1){
//     ledcWriteTone(AUDIO_PWM, 100);
//     primary_timer = millis();
//     while(millis()-primary_timer <100&&alarm_on == 1);
//   }
// }

// bool comparePasscodes(){
//   bool correct = true;
//     for(int i = 0;i < 4; i++){
//       Serial.println(passcode[i]);
//       if(passcode[i] != correctPasscode[i]){
//         correct = false;
//       }
//     }
//   return correct;
// }

// int checkWebsiteAlarmStatus(){
//   sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/yechengz/packmat2/request_handler.py?user_id=%s HTTP/1.1\r\n", userID);
//   strcat(request_buffer, "Host: 608dev-2.net\r\n\r\n");
//   //Serial.println(request_buffer);
//   do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
//   //Serial.println(response_buffer); //viewable in Serial Terminal
//   const int capacity = 300;
//   StaticJsonDocument<capacity> doc;
//   deserializeJson(doc,response_buffer);
//   int alarm_status = doc["alarm_status"];
//   return alarm_status;
// }

// void setup() {
//   // put your setup code here, to run once:
//   //calibration set up 
//   Serial.begin(115200); delay(10);
//   Serial.println();
//   Serial.println("Starting...");

//   LoadCell.begin();
//   //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
//   unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
//   boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
//   LoadCell.start(stabilizingtime, _tare);
//   if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
//     Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
//     while (1);
//   }
//   else {
//     LoadCell.setCalFactor(-20.5); // user set calibration value (float), initial value 1.0 may be used for this sketch
//     Serial.println("Startup is complete");
//   }
//   while (!LoadCell.update());

//   //button set up
//   tft.init();
//   tft.setRotation(2);
//   resetScreen(tft);
//   welcome(tft);
//   Serial.println("Welcome to PackMat");
//   Serial.begin(115200);

//   state = PM1;
//   pinMode(input1, INPUT_PULLUP);
//   pinMode(input2, INPUT_PULLUP);
//   pinMode(input3, INPUT_PULLUP);
//   pinMode(input4, INPUT_PULLUP);

//   update = 1;

//   pinMode(AUDIO_TRANSDUCER, OUTPUT);

//   //set up AUDIO_PWM which we will control in this lab for music:
//   ledcSetup(AUDIO_PWM, 0, 12);//12 bits of PWM precision
//   ledcWrite(AUDIO_PWM, 0); //0 is a 0% duty cycle for the NFET
//   ledcAttachPin(AUDIO_TRANSDUCER, AUDIO_PWM);
//   int n = WiFi.scanNetworks();
//   Serial.println("scan done");
//   if (n == 0) {
//     Serial.println("no networks found");
//   } else {
//     Serial.print(n);
//     Serial.println(" networks found");
//     for (int i = 0; i < n; ++i) {
//       Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
//       uint8_t* cc = WiFi.BSSID(i);
//       for (int k = 0; k < 6; k++) {
//         Serial.print(*cc, HEX);
//         if (k != 5) Serial.print(":");
//         cc++;
//       }
//       Serial.println("");
//     }
//   }
//   delay(100); //wait a bit (100 ms)

//   //if using regular connection use line below:
//   WiFi.begin(network, password);
//   //if using channel/mac specification for crowded bands use the following:
//   //WiFi.begin(network, password, channel, bssid);


//   uint8_t count = 0; //count used for Wifi check times
//   Serial.print("Attempting to connect to ");
//   Serial.println(network);
//   while (WiFi.status() != WL_CONNECTED && count < 12) {
//     delay(500);
//     Serial.print(".");
//     count++;
//   }
//   delay(2000);
//   if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
//     Serial.println("CONNECTED!");
//     Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
//     delay(500);
//   } else { //if we failed to connect just Try again.
//     Serial.println("Failed to Connect :/  Going to restart");
//     Serial.println(WiFi.status());
//     ESP.restart(); // restart the ESP (proper way)
//   }

//   for (int i = 0; i < KEY_LENGTH; i++) {
//     correctPasscode[i] = 0;
//   }

//   postUpdate(0, 0);
//   alarm_on = 0;
//   numCoded = 0;
//   lastRemoteCheck = millis();
//   timer = millis();
//   lastTone = millis();
//   numPresses = 0;
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   //get weight:
//   static boolean newDataReady = 0;
//   const int serialPrintInterval = 0; //increase value to slow down serial print activity

//   // check for new data/start next conversion:
//   if (LoadCell.update()) newDataReady = true;

//   // get smoothed value from the dataset:
//   if (newDataReady) {
//     if (millis() > t + serialPrintInterval) {
//       curr_weight = LoadCell.getData();
//       //Serial.print("Load_cell output val: ");
//       //Serial.println(curr_weight);
//       newDataReady = 0;
//       t = millis();
//     }
//   }

//   //end get weight
//   packmat(digitalRead(input1), digitalRead(input2), digitalRead(input3), digitalRead(input4));

//   //update old weight
//   old_weight = curr_weight;

// }

// //1: entering numbers/unlock mode, 2: pressure increase, 3: pressure decrease/yes/correct, 4: no/incorrect
// void packmat(int input1, int input2, int input3, int input4){
//   char key = keypad.getKey();
//   switch(state){
//     case PM1:
//       if(update){ //so only prints once
//         Serial.println("In PM1 State"); 
//         setPasscode(tft);
//         update = 0;
//       }
//       if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
//         Serial.println("First number entered");
//         Serial.println("Switching to Program Mode 2");
//         Serial.println(key);
//         correctPasscode[numCoded] = key -'0';
//         Serial.println(correctPasscode[numCoded]);
//         numCoded += 1;
//         update = 1;
//         state = PM2;
//         firstNum(tft, key);
//       }
//       break;

//     case PM2:
//       if(update){ //so only prints once
//         Serial.println("In PM2 State"); 
//         update = 0;
//       }
//       if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
//         Serial.println("Second number entered");
//         Serial.println("Switching to Program Mode 3");
//         Serial.println(key);
//         correctPasscode[numCoded] = key -'0';
//         Serial.println(correctPasscode[numCoded]);
//         numCoded += 1;
//         update = 1;
//         state = PM3;
//         secondNum(tft, key);
//       }
//       break;

//     case PM3:
//       if(update){ //so only prints once
//         Serial.println("In PM3 State"); 
//         update = 0;
//       }
//       if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
//         Serial.println("Third number entered");
//         Serial.println("Switching to Program Mode 4");
//         Serial.println(key);
//         correctPasscode[numCoded] = key -'0';
//         Serial.println(correctPasscode[numCoded]);
//         numCoded += 1;
//         update = 1;
//         state = PM4;
//         thirdNum(tft, key);
//       }
//       break;

//     case PM4:
//       if(update){ //so only prints once
//         Serial.println("In PM4 State"); 
//         update = 0;
//       }
//       if(key && key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '*' && key != '#'){//entering numbers
//         Serial.println("Fourth number entered");
//         Serial.println("Switching to Locked");
//         Serial.println(key);
//         correctPasscode[numCoded] = key -'0';
//         Serial.println(correctPasscode[numCoded]);
//         postUpdate(0, 1);
//         numCoded = 0;
//         update = 1;
//         state = USERNAME;
//         fourthNum(tft, key);
//         display = millis();
//         while(millis()-display < 3000); //so can see fully entered code
//       }
//       break;
//     case USERNAME:
//       if(update){ //so only prints once
//         Serial.println("In USERNAME State"); 
//         update = 0;
//       }
//       userKeyCheck();
//       if(!input3 && millis()-timer >= 1000){
//         user++;
//         numPresses =0;
//         timer = millis();
//       }
//       if(!input4){
//         update = 1;
//         state = PASSWORD;
//       }
//       break;
//     case PASSWORD:
//       if(update){ //so only prints once
//         Serial.println("In PASSWORD State"); 
//         update = 0;
//       }
//       passwordKeyCheck();
//       if(!input3 && millis()-timer >= 1000){
//         pwd++;
//         numPresses =0;
//         timer = millis();
//       }
//       if(!input4){
//         update = 1;
//         state = REST;
//       }
//       break;
//     case LOGIN:
//       if(update){
//         create_account();
//         login();
//         update = 0;
//         state = REST;
//       }
//       break;
//     case REST:
//       ledcWriteTone(AUDIO_PWM,0);
//       numCoded = 0;
//       if(update){ //so only prints once
//         Serial.println("In Rest State"); 
//         unarmed(tft);
//         update = 0;
//         postUpdate(0, 0);
//       }
//       if(curr_weight - old_weight>THRESHOLD){//pressure increase
//         Serial.println("Pressure increase detected.");
//         Serial.println("Switching to package confirmation 1 state");
//         update = 1;
//         state = PC1;
//       }
//       break;
    
//     case PC1:
//       if(update){ //so only prints once
//         Serial.println("In PC1 State"); 
//         packageConfirmation(tft);
//         update = 0;
//       }
//       if(!input3){//yes
//         Serial.println("Package Confirmed");
//         Serial.println("Switching to Program Mode 1");
//         update = 1;
//         state = LOCKED;
//       }else if(!input4){//no
//         Serial.println("No Package");
//         Serial.println("Switching to Rest");
//         update = 1;
//         state = REST;
//       }
//       break;

//     case LOCKED:
//       if(update){ //so only prints once
//         Serial.println("In LOCKED State"); 
//         locked(tft);
//         postUpdate(0, 1);
//         numCoded = 0;
//         update = 0;
//       }
//       if((curr_weight - old_weight)>THRESHOLD){//pressure increase
//         Serial.println("Pressure Increase Detected");
//         Serial.println("Swithcing to Package Confirmation 2");
//         update = 1;
//         state = PC2;
//       }else if(old_weight - curr_weight>THRESHOLD){ //pressure decrease
//         Serial.println("Pressure Decrease Detected");
//         Serial.println("Switching to Alarm");
//         update = 1;
//         state = ALARM;
//       }else if(!input4){//unlock mode
//         Serial.println("Entering Unlock mode");
//         Serial.println("Switching to unlock mode 1");
//         update = 1;
//         state = UM1;
//       }
//       break;
    
//     case PC2:
//       if(update){ //so only prints once
//         Serial.println("In PC2 State"); 
//         packageConfirmation(tft);
//         update = 0;
//       }
//       if(!input3){//yes
//         Serial.println("Package Confirmed");
//         Serial.println("Switching to LOCKED");
//         update = 1;
//         state = LOCKED;
//       }else if(!input4){//no
//         Serial.println("No Package");
//         Serial.println("Switching to Alarm");
//         alarmStart = millis();
//         alarm_on = 1;
//         postUpdate(1, 1);
//         playTone();
//         update = 1;
//         state = ALARM;
//         alarm(tft);
//         display = millis();
//         while(millis()-display < 3000); //so can see fully entered code
//       }
//       break;

//     case ALARM:
//       playTone();
//       numCoded = 0;
//       if(update){ //so only prints once
//         Serial.println("In ALARM State"); 
//         unlockAlarm(tft);
//         postUpdate(1, 1);
//         alarm_on=1;
//         update = 0;
//       }
//       else{
//         state = AS1;
//         update = 1;
//       }
//       break;
    
//     case AS1:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Stop 1 State"); 
//         unlockAlarm(tft);
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
//         Serial.println("First number entered");
//         Serial.println("Switching to Alarm Check 1");
//         passcode[numCoded] = key-'0';
//         numCoded += 1;
//         update = 1;
//         state = AC1;
//         firstNum(tft, key);
//       }
      
//       break;

//     case AC1:

//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Check 1 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(passcode[0] == correctPasscode[0]){ //correct
//         Serial.println("Correct first number");
//         Serial.println("Switching to Alarm Stop 2");
//         update = 1;
//         state = AS2;
//       }else if(passcode[0] != correctPasscode[0]){//incorrect
//         Serial.println("Incorrect first number");
//         Serial.println("Switching to Alarm");
//         update = 1;
//         state = ALARM;
//       }
//       break;

//     case AS2:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Stop 2 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
//         Serial.println("Second number entered");
//         Serial.println("Switching to Alarm Check 2");
//         passcode[numCoded] = key-'0';
//         numCoded += 1;
//         update = 1;
//         state = AC2;
//         secondNum(tft, key);
//       }
//       break;

//     case AC2:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Check 2 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(passcode[1] == correctPasscode[1]){ //correct
//         Serial.println("Correct second number");
//         Serial.println("Switching to AS3");
//         update = 1;
//         state = AS3;
//       }else if(passcode[1] != correctPasscode[1]){//incorrect
//         Serial.println("Incorrect second number");
//         Serial.println("Switching to Alarm");
//         update = 1;
//         state = ALARM;
//       }
//       break;

//     case AS3:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Stop 3 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
//         Serial.println("Third number entered");
//         Serial.println("Switching to Alarm Check 3");
//         passcode[numCoded] = key-'0';
//         numCoded += 1;
//         update = 1;
//         state = AC3;
//         thirdNum(tft, key);
//       }
//       break;

//     case AC3:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Check 3 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(passcode[2] == correctPasscode[2]){ //correct
//         Serial.println("Correct third number");
//         Serial.println("Switching to AS4");
//         update = 1;
//         state = AS4;
//       }else if(passcode[2] == correctPasscode[2]){//incorrect
//         Serial.println("Incorrect third number");
//         Serial.println("Switching to Alarm");
//         update = 1;
//         state = ALARM;
//       }
//       break;

//     case AS4:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Stop 4 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(alarm_on == 0){
//         state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//entering number pad click
//         Serial.println("Fourth number entered");
//         Serial.println("Switching to Alarm Check 4");
//         passcode[numCoded] = key-'0';
//         numCoded = 0;
//         update = 1;
//         state = AC4;
//         fourthNum(tft, key);
//       }
//       break;

//     case AC4:
//       if(millis()-lastTone >= 200){
//         lastTone = millis();
//         playTone();
//       }
//       if(update){ //so only prints once
//         Serial.println("In Alarm Check 4 State"); 
//         update = 0;
//       }
//       if (millis()- lastRemoteCheck >= REMOTE_CHECK_PERIOD){
//         if(checkWebsiteAlarmStatus() == 0){
//           //alarm turns off so switch to state REST
//           state = REST;
//           //reset the numCoded 
//           numCoded = 0;
//           update = 1;
//           break;
//         }
//         else{
//           lastRemoteCheck = millis();
//         }
//       }
//       if(passcode[3] == correctPasscode[3]){ //correct
//         Serial.println("Correct fourth number");
//         Serial.println("Switching to REST");
//         update = 1;
//         alarm_on = 0;
//         state = REST;
//       }else if(passcode[3] != correctPasscode[3]){//incorrect
//         Serial.println("Incorrect fourth number");
//         Serial.println("Switching to Alarm");
//         update = 1;
//         state = ALARM;
//       }
//       break;
    
//     case UM1:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Mode 1 State"); 
//         unlock(tft);
//         update = 0;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
//         Serial.println("First number entered");
//         Serial.println("Switching to Unlock Check 1");
//         passcode[numCoded]=key-'0';
//         numCoded += 1;
//         update = 1;
//         state = UC1;
//         firstNum(tft,key);
//       }
//       break;
    
//     case UC1:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Check 1 State"); 
//         update = 0;
//       }
//       if(passcode[0] == correctPasscode[0]){//correct
//         Serial.println("Correct first number");
//         Serial.println("Switching to Unlock Mode 2");
//         update = 1;
//         state = UM2;
//       }else if(passcode[0] != correctPasscode[0]){//incorrect
//         Serial.println("Incorrect first number");
//         Serial.println("Switching to LOCKED");
//         update = 1;
//         state = LOCKED;
//       }
//       break;

//     case UM2:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Mode 2 State"); 
//         update = 0;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
//         Serial.println("Second number entered");
//         Serial.println("Switching to Unlock Check 2");
//         passcode[numCoded] = key-'0';
//         numCoded += 1;
//         update = 1;
//         state = UC2;
//         secondNum(tft, key);
//       }
//       break;
    
//     case UC2:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Check 2 State"); 
//         update = 0;
//       }
//       if(passcode[1] == correctPasscode[1]){//correct
//         Serial.println("Correct second number");
//         Serial.println("Switching to Unlock Mode 3");
//         update = 1;
//         state = UM3;
//       }else if(passcode[1] != correctPasscode[1]){//incorrect
//         Serial.println("Incorrect second number");
//         Serial.println("Switching to LOCKED");
//         update = 1;
//         state = LOCKED;
//       }
//       break;

//     case UM3:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Mode 3 State"); 
//         update = 0;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
//         Serial.println("Third number entered");
//         Serial.println("Switching to Unlock Check 3");
//         passcode[numCoded] = key-'0';
//         numCoded += 1;
//         update = 1;
//         state = UC3;
//         thirdNum(tft, key);
//       }
//       break;
    
//     case UC3:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Check 3 State"); 
//         update = 0;
//       }
//       if(passcode[2] == correctPasscode[2]){//correct
//         Serial.println("Correct third number");
//         Serial.println("Switching to Unlock Mode 4");
//         update = 1;
//         state = UM4;
//       }else if(passcode[2] != correctPasscode[2]){//incorrect
//         Serial.println("Incorrect third number");
//         Serial.println("Switching to LOCKED");
//         update = 1;
//         state = LOCKED;
//       }
//       break;

//     case UM4:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Mode 4 State"); 
//         update = 0;
//       }
//       if(key&& key!= 'A' && key!= 'B' && key!= 'C' && key!= 'D' && key!= '*' && key!= '#'){//enter number
//         Serial.println("Fourth number entered");
//         Serial.println("Switching to Unlock Check 4");
//         passcode[numCoded] = key-'0';
//         numCoded = 0;
//         update = 1;
//         state = UC4;
//         fourthNum(tft, key);
//       }
//       break;
    
//     case UC4:
//       if(update){ //so only prints once
//         Serial.println("In Unlock Check 4 State"); 
//         update = 0;
//       }
//       if(passcode[3] == correctPasscode[3]){//correct
//         Serial.println("Correct fourth number");
//         Serial.println("Switching to REST");
//         update = 1;
//         state = REST;
//       }else if(passcode[3] != correctPasscode[3]){//incorrect
//         Serial.println("Incorrect fourth number");
//         Serial.println("Switching to LOCKED");
//         update = 1;
//         state = LOCKED;
//       }
//       break;

//   }
// }
