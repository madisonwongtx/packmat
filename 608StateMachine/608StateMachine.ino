unsigned long button_timer;
const int BUTTON_TIMEOUT = 50;
const int input1 = 45; //entering numbers, unlock mode
const int input2 = 39; //pressure increase
const int input3 = 38; //pressure decrease, yes
const int input4 = 37; //no
uint8_t state;


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
  Serial.println("Welcome to PackMat");
  Serial.begin(115200);

  state = REST;
  pinMode(input1, INPUT_PULLUP);
  pinMode(input2, INPUT_PULLUP);
  pinMode(input3, INPUT_PULLUP);
  pinMode(input4, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  packmat(input1, input2, input3, input4);

}

//1: entering numbers/unlock mode, 2: pressure increase, 3: pressure decrease/yes, 4: no
void packmat(int input1, int input2, int input3, int input4){
  Serial.println("working setup");
}
