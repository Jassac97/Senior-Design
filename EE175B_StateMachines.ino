//State Machines for Senior Design Chess Robot

#include<Servo.h>

Servo baseServo; //base servo
Servo elbowServo; //elbow servo
Servo shoulderServo; //shoulder servo
Servo wristServo; //wrist servo
Servo clawServo; //claw servo

enum STATE {S1, S2, S3} gState;
//S1 is state where player is making move. Robot is waiting to receive move
//S2 Chess engine decides move for robot. Sends signal to robot
//S3 is when robot makes move. It then resets back to original position.
int B = 0; //button value 

void RobotFSM(void) {

  //transitions
  switch(gState) {
    case S1: 

    break;

    case S2:


    break;

    case S3


    break;


  }

  //actions
  switch(gState) {

    case S1:
    //player makes move
    //robot waits for move in initial position
    //player hits button after move is complete to send signal to chess engine

    break;

    case S2:
    //chess engine recieves player's move data
    //engine decides what move the robot should make and sends the information to robot

    break;

    case S3:
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
  pinMode(2, INPUT); //button

}

void loop() {
  B = digitalRead(2);
  RobotFSM();
  delay(100);
}
