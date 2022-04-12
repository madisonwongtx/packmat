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
  packmat(digitalRead(input1), digitalRead(input2), digitalRead(input3), digitalRead(input4));

}

//1: entering numbers/unlock mode, 2: pressure increase, 3: pressure decrease/yes/correct, 4: no/incorrect
void packmat(int input1, int input2, int input3, int input4){
  switch(state){
    case REST:
      if(update){ //so only prints once
        Serial.println("In Rest State"); 
        print_message("REST");
        update = 0;
      }
      if(!input2){//pressure increase
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
      if(!input2){//pressure increase
        Serial.println("Pressure Increase Detected");
        Serial.println("Swithcing to Package Confirmation 2");
        //print_message("PC2");
        update = 1;
        delay(150);
        state = PC2;
      }else if(!input3){ //pressure decrease
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