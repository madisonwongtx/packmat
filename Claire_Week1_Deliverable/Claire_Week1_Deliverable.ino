/**
Make sure that the Keypad library is installed. Look for Keypad library by Mark Stanly, Alexander Brevig, Version 3.1.0
**/
#include <Keypad.h>

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
int passcode[4];
int correctPasscode[4] = {1,2,3,4};

int numCoded = 0;
int done = 0;
int alarmStart = -1;

uint8_t AUDIO_PWM = 1;
uint8_t AUDIO_TRANSDUCER = 14;

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
void playAlarm(int timeElapsed){
  int primary_timer = alarmStart;
  while(millis()-alarmStart < timeElapsed ){
    //play tone
    ledcWriteTone(AUDIO_PWM,350);
    primary_timer = millis();
    while(millis()-primary_timer <100&&millis()-alarmStart < timeElapsed);
    if(millis()-alarmStart >= timeElapsed){
      break;
    }
    ledcWriteTone(AUDIO_PWM, 100);
    primary_timer = millis();
    while(millis()-primary_timer <100&&millis()-alarmStart < timeElapsed);
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
      playAlarm(5000);
    }
    done = 0;
  }
}
