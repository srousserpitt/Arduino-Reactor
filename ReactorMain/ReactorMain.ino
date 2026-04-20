#include <Servo.h>
#include <CapacitiveSensor.h>

//Component pin assignments
//In/Out naming may be somewhat backwards, the touch stuff is magic to me
//TODO: Try re-arranging for more stable wire behavior?
int g1InPin = 1;
int g2InPin = 2;
int g1OutPin = 3;
int g2OutPin = 4;
int button1Pin = 6;
int button2Pin = 7;
int doorPin = 8;
int wireCircuit = 10;

//Setup for door
Servo doorMotor;
long doorTime = 0;
long dtMax = 8000;

//Touch sensor setup
CapacitiveSensor Guard1 = CapacitiveSensor(g1InPin, g1OutPin);
int g1Read;
int g1Past = LOW;
bool g1Alive = true;
CapacitiveSensor Guard2 = CapacitiveSensor(g2InPin, g2OutPin);
int g2Read;
int g2Past = LOW;
bool g2Alive = true;
int gBuffer = 200;

//Wire setup
bool reactorOn = true;

void setup() {
  //Initiate monitor
  Serial.begin(9600);
  //Attach door pin and make sure servo is off
  doorMotor.attach(doorPin);
  stopServo(doorMotor);
  //Set which pins will be receiving data
  pinMode(g1OutPin, INPUT);
  pinMode(g2OutPin, INPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(wireCircuit, INPUT_PULLUP);
  //Indicate when ready
  Serial.println("Setup Complete");
}

void loop() {
  //If guard is alive, check if it's getting touched
  if(g1Alive) {
    checkGuard(1);
  }
  if(g2Alive) {
    checkGuard(2);
  }
  //If both guards are knocked out, do other code
  if(!g1Alive )//&& !g2Alive)
  {
    //If both buttons are being pushed and door isn't fully open, open door
    if(digitalRead(button1Pin) == HIGH && digitalRead(button2Pin) == HIGH && doorTime < dtMax){
      //First variable is door move speed
      forward(10, doorMotor);
      //Used to keep track of how long door has been moved, use time limit and speed to adjust door opening
      doorTime++;
      //Print statement once door has reached max range
      if(doorTime == dtMax){
        Serial.println("Reactor door is open!");
      }
    } else {
      stopServo(doorMotor);
    }
    //Once door has reached 'maximum' open
    if(doorTime == dtMax && reactorOn) {
      stopServo(doorMotor);
      //Check if wire is disconnected
      if(digitalRead(wireCircuit) == HIGH){
        Serial.println("Reactor has been disabled...");
        reactorOn = false;
      }
    }
  }
}

//Sensor Methods
void checkGuard(int guard) {
  switch(guard) {
    case 1:
      g1Read = digitalRead(g1OutPin);
      if(g1Read == HIGH && g1Past == LOW && millis() > gBuffer){
        g1Alive = false;
        Serial.println("Guard 1 is disabled");
      }
      g1Past = g1Read;
      break;
    case 2:
      g2Read = digitalRead(g2OutPin);
      if(g2Read == HIGH && g2Past == LOW && millis() > gBuffer){
        g2Alive = false;
        Serial.println("Guard 2 is disabled");
      }
      g2Past = g2Read;
      break;
  }
}

//Motor Methods, copied from examples and adjusted to fit our motor model
// speed: 0–100 (% of max)
void forward(int speed, Servo activeServo) {
  int speedCalc = 90 + (90 * speed / 100);
  activeServo.write(speedCalc);
}

//Not actually used or tested
void backward(int speed, Servo activeServo) {
  int speedCalc = 90 - (90 * speed / 100);
  activeServo.write(speedCalc);
}

void stopServo(Servo activeServo) {
  activeServo.write(90); // this motor operates on 0-180 scale so 90 is neutral/stopped
}