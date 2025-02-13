//State Machines for Senior Design Chess Robot

#include<Servo.h>

Servo baseServo; //base servo
Servo elbowServo; //elbow servo
Servo shoulderServo; //shoulder servo
Servo wristServo; //wrist servo
Servo clawServo; //claw servo

const int buttonPin = 2; //button value 
const int GreenLEDPin = 3; // green LED pin
const int RedLEDPin = 4; //Red LED pin

enum STATE {S1, S2, S3} gState = S1;
//S1 is state where player is making move. Robot is waiting to receive move
//S2 Chess engine decides move for robot. Sends signal to robot
//S3 is when robot makes move. It then resets back to original position.

int buttonState = 0; //Button state. 0 = not pressed

void RobotFSM(void) {
  buttonState = digitalRead(buttonPin);

  //transitions
  switch(gState) {
    case S1: 
    if (buttonState == HIGH) {
      gState = S2;
    }
    else{
      gState = S1;
    }

    break;

    case S2:


    break;

    case S3


    break;


  }

  //actions
  switch(gState) {

    case S1:
    digitalWrite(GreenLEDPin, HIGH); //Green LED On
    digitalWrite(RedLEDPin, LOW); //Red LED Off
    //player makes move
    //robot waits for move in initial position
    //player hits button after move is complete to send signal to chess engine

    break;

    case S2:
    digitalWrite(GreenLEDPin, LOW); //Green LED off
    digitalWrite(RedLEDPin, LOW); //Red LED Off
    //chess engine recieves player's move data
    //engine decides what move the robot should make and sends the information to robot

    break;

    case S3:
    digitalWrite(GreenLEDPin, LOW); //Green LED Off
    digitalWrite(RedLEDPin, HIGH); //Red LED On
    //robot recieves data from chess engine on move it needs to make
    //robot proceeds to make move
    //once robot makes move, it resets back in original position
    break;

    default:
    break;
  }
}


void setup() {
  Serial.begin(9600);
  baseServo.attach(8);
  elbowServo.attach(9);
  shoulderServo.attach(10);
  wristServo.attach(11);
  clawServo.attach(12);
  pinMode(buttonPin, INPUT); //button
  pinMode(GreenLEDPin, OUTPUT); //Green LED
  pinMode(RedLEDPin, OUTPUT); //Red LED

}

void loop() {
  RobotFSM();
  delay(100);
}
