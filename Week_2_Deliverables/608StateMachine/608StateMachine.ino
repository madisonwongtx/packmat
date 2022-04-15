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
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 8; //mcu > HX711 dout pin
const int HX711_sck = 9; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

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
  //Serial.println(curr_weight);
  switch(state){
    case REST:
      if(update){ //so only prints once
        Serial.println("In Rest State"); 
        print_message("REST");
        update = 0;
      }
      if(curr_weight - old_weight>THRESHOLD){//pressure increase
        Serial.println("Pressure increase detected.");
        Serial.println("Switching to package confirmation 1 state");
        //print_message("PC1");
        update = 1;
        delay(150);
        state = PC1;
      }
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
        delay(150);
        state = PM1;
      }else if(!input4){//no
        Serial.println("No Package");
        Serial.println("Switching to Rest");
        //print_message("Rest");
        update = 1;
        delay(150);
        state = REST;
      }
      break;

    case PM1:
      if(update){ //so only prints once
        Serial.println("In PM1 State"); 
        print_message("PM1");
        update = 0;
      }
      if(!input1){//entering numbers
        Serial.println("First number entered");
        Serial.println("Switching to Program Mode 2");
        //print_message("PM2");
        update = 1;
        delay(150);
        state = PM2;
      }
      break;

    case PM2:
      if(update){ //so only prints once
        Serial.println("In PM2 State"); 
        print_message("PM2");
        update = 0;
      }
      if(!input1){//entering numbers
        Serial.println("Second number entered");
        Serial.println("Switching to Program Mode 3");
        //print_message("PM3");
        update = 1;
        delay(150);
        state = PM3;
      }
      break;

    case PM3:
      if(update){ //so only prints once
        Serial.println("In PM3 State"); 
        print_message("PM3");
        update = 0;
      }
      if(!input1){//entering numbers
        Serial.println("Third number entered");
        Serial.println("Switching to Program Mode 4");
        //print_message("PM4");
        update = 1;
        delay(150);
        state = PM4;
      }
      break;

    case PM4:
      if(update){ //so only prints once
        Serial.println("In PM4 State"); 
        print_message("PM4");
        update = 0;
      }
      if(!input1){//entering numbers
        Serial.println("Fourth number entered");
        Serial.println("Switching to Locked");
        //print_message("LOCKED");
        update = 1;
        delay(150);
        state = LOCKED;
      }
      break;

    case LOCKED:
      if(update){ //so only prints once
        Serial.println("In LOCKED State"); 
        print_message("LOCKED");
        update = 0;
      }
      if((curr_weight - old_weight)>THRESHOLD){//pressure increase
        Serial.println("Pressure Increase Detected");
        Serial.println("Swithcing to Package Confirmation 2");
        //print_message("PC2");
        update = 1;
        delay(150);
        state = PC2;
      }else if(old_weight - curr_weight>THRESHOLD){ //pressure decrease
        Serial.println("Pressure Decrease Detected");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        delay(150);
        state = ALARM;
      }else if(!input4){//unlock mode
        Serial.println("Entering Unlock mode");
        Serial.println("Switching to unlock mode 1");
        //print_message("UM1");
        update = 1;
        delay(150);
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
        delay(150);
        state = LOCKED;
      }else if(!input4){//no
        Serial.println("No Package");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        delay(150);
        state = ALARM;
      }
      break;

    case ALARM:
      if(update){ //so only prints once
        Serial.println("In ALARM State"); 
        print_message("ALARM");
        update = 0;
      }
      if (!input4){//stop alarm/unlock
        Serial.println("Attempting to turn off alarm");
        Serial.println("Switching to Alarm Stop 1");
        //print_message("AS1");
        update = 1;
        delay(150);
        state = AS1;
      }
      break;
    
    case AS1:
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 1 State"); 
        print_message("AS1");
        update = 0;
      }
      if(!input1){//entering number pad click
        Serial.println("First number entered");
        Serial.println("Switching to Alarm Check 1");
        //print_message("AC1");
        update = 1;
        delay(150);
        state = AC1;
      }
      break;

    case AC1:
      if(update){ //so only prints once
        Serial.println("In Alarm Check 1 State"); 
        print_message("AC1");
        update = 0;
      }
      if(!input3){ //correct
        Serial.println("Correct first number");
        Serial.println("Switching to Alarm Stop 2");
        //print_message("+AS2"); //+means correct
        update = 1;
        delay(150);
        state = AS2;
      }else if(!input4){//incorrect
        Serial.println("Incorrect first number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        delay(150);
        state = ALARM;
      }
      break;

    case AS2:
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 2 State"); 
        print_message("AS2");
        update = 0;
      }
      if(!input1){//entering number pad click
        Serial.println("Second number entered");
        Serial.println("Switching to Alarm Check 2");
        //print_message("AC2");
        update = 1;
        delay(150);
        state = AC2;
      }
      break;

    case AC2:
      if(update){ //so only prints once
        Serial.println("In Alarm Check 2 State"); 
        print_message("AC2");
        update = 0;
      }
      if(!input3){ //correct
        Serial.println("Correct second number");
        Serial.println("Switching to AS3");
        //print_message("+AS3"); //+means correct
        update = 1;
        delay(150);
        state = AS3;
      }else if(!input4){//incorrect
        Serial.println("Incorrect second number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        delay(150);
        state = ALARM;
      }
      break;

    case AS3:
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 3 State"); 
        print_message("AS3");
        update = 0;
      }
      if(!input1){//entering number pad click
        Serial.println("Third number entered");
        Serial.println("Switching to Alarm Check 3");
        //print_message("AC3");
        update = 1;
        delay(150);
        state = AC3;
      }
      break;

    case AC3:
      if(update){ //so only prints once
        Serial.println("In Alarm Check 3 State"); 
        print_message("AC3");
        update = 0;
      }
      if(!input3){ //correct
        Serial.println("Correct third number");
        Serial.println("Switching to AS4");
        //print_message("+AS4"); //+means correct
        update = 1;
        delay(150);
        state = AS4;
      }else if(!input4){//incorrect
        Serial.println("Incorrect third number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        delay(150);
        state = ALARM;
      }
      break;

    case AS4:
      if(update){ //so only prints once
        Serial.println("In Alarm Stop 4 State"); 
        print_message("AS4");
        update = 0;
      }
      if(!input1){//entering number pad click
        Serial.println("Fourth number entered");
        Serial.println("Switching to Alarm Check 4");
        //print_message("AC4");
        update = 1;
        delay(150);
        state = AC4;
      }
      break;

    case AC4:
      if(update){ //so only prints once
        Serial.println("In Alarm Check 4 State"); 
        print_message("AC4");
        update = 0;
      }
      if(!input3){ //correct
        Serial.println("Correct fourth number");
        Serial.println("Switching to REST");
        //print_message("+REST"); //+means correct
        update = 1;
        delay(150);
        state = REST;
      }else if(!input4){//incorrect
        Serial.println("Incorrect fourth number");
        Serial.println("Switching to Alarm");
        //print_message("ALARM");
        update = 1;
        delay(150);
        state = ALARM;
      }
      break;
    
    case UM1:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 1 State"); 
        print_message("UM1");
        update = 0;
      }
      if(!input1){//enter number
        Serial.println("First number entered");
        Serial.println("Switching to Unlock Check 1");
        //print_message("UC1");
        update = 1;
        delay(150);
        state = UC1;
      }
      break;
    
    case UC1:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 1 State"); 
        print_message("UC1");
        update = 0;
      }
      if(!input3){//correct
        Serial.println("Correct first number");
        Serial.println("Switching to Unlock Mode 2");
        //print_message("+UM2");
        update = 1;
        delay(150);
        state = UM2;
      }else if(!input4){//incorrect
        Serial.println("Incorrect first number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        delay(150);
        state = LOCKED;
      }
      break;

    case UM2:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 2 State"); 
        print_message("UM2");
        update = 0;
      }
      if(!input1){//enter number
        Serial.println("Second number entered");
        Serial.println("Switching to Unlock Check 2");
        //print_message("UC2");
        update = 1;
        delay(150);
        state = UC2;
      }
      break;
    
    case UC2:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 2 State"); 
        print_message("UC2");
        update = 0;
      }
      if(!input3){//correct
        Serial.println("Correct second number");
        Serial.println("Switching to Unlock Mode 3");
        //print_message("+UM3");
        update = 1;
        delay(150);
        state = UM3;
      }else if(!input4){//incorrect
        Serial.println("Incorrect second number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        delay(150);
        state = LOCKED;
      }
      break;

    case UM3:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 3 State"); 
        print_message("UM3");
        update = 0;
      }
      if(!input1){//enter number
        Serial.println("Third number entered");
        Serial.println("Switching to Unlock Check 3");
        //print_message("UC3");
        update = 1;
        delay(150);
        state = UC3;
      }
      break;
    
    case UC3:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 3 State"); 
        print_message("UC3");
        update = 0;
      }
      if(!input3){//correct
        Serial.println("Correct third number");
        Serial.println("Switching to Unlock Mode 4");
        //print_message("+UM4");
        update = 1;
        delay(150);
        state = UM4;
      }else if(!input4){//incorrect
        Serial.println("Incorrect third number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        delay(150);
        state = LOCKED;
      }
      break;

    case UM4:
      if(update){ //so only prints once
        Serial.println("In Unlock Mode 4 State"); 
        print_message("UM4");
        update = 0;
      }
      if(!input1){//enter number
        Serial.println("Fourth number entered");
        Serial.println("Switching to Unlock Check 4");
        //print_message("UC4");
        update = 1;
        delay(150);
        state = UC4;
      }
      break;
    
    case UC4:
      if(update){ //so only prints once
        Serial.println("In Unlock Check 4 State"); 
        print_message("UC4");
        update = 0;
      }
      if(!input3){//correct
        Serial.println("Correct fourth number");
        Serial.println("Switching to REST");
        //print_message("+REST");
        update = 1;
        delay(150);
        state = REST;
      }else if(!input4){//incorrect
        Serial.println("Incorrect fourth number");
        Serial.println("Switching to LOCKED");
        //print_message("LOCKED");
        update = 1;
        delay(150);
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
