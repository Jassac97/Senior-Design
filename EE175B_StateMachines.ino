#include <Servo.h>

//State Machines for Senior Design Chess Robot

Servo baseServo;    
Servo elbowServo;    
Servo shoulderServo;
Servo wristServo;
Servo clawServo;
byte numList = 2; //switch back to 0
const byte buttonPin = 2; //button value 
const byte GreenLEDPin = 3; // green LED pin
const byte RedLEDPin = 4; //Red LED pin
bool chessEngineTurn = false;
bool armTurn = false;
bool moveDone = true;
enum TurnDetermined {Idle, Hold, RobotTurn} turnState;
enum Robot {RM_Reset, RM_S1, RM_S2, RM_S3, RM_S4} robotState;
enum ChessEngine {Initial, Send, Receive} chessEngineState;
#define MAX_ROWS 10   
#define MAX_COLS 4 // number of angles per list sent from Python
#define MAX_LENGTH 100   // Maximum number of characters to read

char inputBuffer[MAX_LENGTH]; 
//float angles[MAX_ROWS][MAX_COLS];  // 2D array to store parsed angles
float angles[2][4] = {{57.53, 90.9, 91.57, 99.33},
                      {33.96, 113.42, 110.72, 102.70}}; //Test RobotFsm()
int angleToPulse(float angle) {
    return (int)((500.0/45.0 * angle) + 500);
}
void move_servo(Servo &servo, float current, float target) {
    int initialPulse;
    int targetPulse;

    initialPulse = angleToPulse(current);
    targetPulse = angleToPulse(target);
    Serial.println(initialPulse);
    Serial.println(targetPulse);
    float step = (targetPulse -  initialPulse) / 50.0;

    if (step != 0) {
      for (float pulse = initialPulse; (step > 0) ? (pulse <= targetPulse) : (pulse >= targetPulse); pulse += step) {
         if (targetPulse > initialPulse) {
          if (pulse > targetPulse) {
            servo.writeMicroseconds(targetPulse);
            }
          else {
            servo.writeMicroseconds((int)pulse);
            }
         }
         
         if (targetPulse < initialPulse) {
          if (pulse < targetPulse) {
            servo.writeMicroseconds(targetPulse);
            }
          else {
            servo.writeMicroseconds((int)pulse);
            }
         }
         delay(20);  // Adjust delay for speed
      }
    }
}

int parseAngles(char* input) {
    // Process data received from the chess engine. 
    // Example: 30,45,60,0;90,120,150,0 -> 2D float array = {{30,45,60,0},{90,120,150,0}}
    char* arr[MAX_COLS];
    int numList = 0;
    char* list = strtok(input, " ; ");
    arr[numList] = new char[strlen(list) + 1];
    strcpy(arr[numList], list);
    while (list != NULL) {
      numList += 1;
      list = strtok(NULL, " ; ");
       if (list != NULL) {  // Prevents NULL issue
         arr[numList] = new char[strlen(list) + 1];  
         strcpy(arr[numList], list);  
      }
    }
    
    int rowIdx;   
    for (int j = 0; j < numList; j++){;
      rowIdx = j;
      char* col = strtok(arr[j], ",");
      int columnIdx = 0;
      angles[rowIdx][columnIdx] = atoi(col);
      while (col != NULL) {
        columnIdx++;
        col = strtok(NULL, ",");
        if (col != NULL) { // Prevents NULL issue
          angles[rowIdx][columnIdx] = atoi(col);
        }
      } 
    }
    return numList;
}

bool containsSemicolon(char input[]) {
    // Iterate through the input array to check for a semicolon
    for (int i = 0; i < 128; i++) {
        if (input[i] == ';') {
            return true;  // Return true if semicolon is found
        }
        if (input[i] == '\0') {
            break;  // Stop if we reach the end of the string (null terminator)
        }
    }
    return false;  // Return false if semicolon is not found
}

void TurnDeterminedFSM(void){
  switch(turnState) {
    case Idle:
      if (!digitalRead(buttonPin)) {
        turnState = Idle;
      }
      else {
        turnState = Hold; 
      }
    break;

    case Hold:
      if (digitalRead(buttonPin)) {
          turnState = Hold;
        }
        else {
          turnState = RobotTurn;
          chessEngineTurn = true;
          moveDone = false;
        }
    break;

    case RobotTurn:
      if (!moveDone) {
        turnState = RobotTurn;
      }
      else {
        turnState = Idle;
      }
    break;

    default:
      turnState = Idle;
    break;
  }
    switch(turnState) {

      case Idle:
        digitalWrite(GreenLEDPin,HIGH);
        digitalWrite(RedLEDPin,LOW);
      break;
      
      case Hold:
        digitalWrite(GreenLEDPin,HIGH);
        digitalWrite(RedLEDPin,LOW);
      break;
      
      case RobotTurn:
        digitalWrite(GreenLEDPin,LOW);
        digitalWrite(RedLEDPin,HIGH);
      break;
    }
}
int rowIdx;
float base_cur = 90.0;
float shoulder_cur = 180.0;
float elbow_cur = 90.0;
float wrist_cur = 90.0;

void RobotFSM(void) {
  //transitions
  switch(robotState) {
    case RM_Reset: 
    if (armTurn) { 
      robotState = RM_S1;
      base_cur = 90;
      shoulder_cur = 180;
      elbow_cur = 90;
      wrist_cur = 90;
    }
    else{
      robotState = RM_Reset;
      base_cur = 90;
      shoulder_cur = 180;
      elbow_cur = 90;
      wrist_cur = 90; 
    }
    break;

    case RM_S1:
      robotState = RM_S2;
      base_cur = angles[rowIdx][0];
      elbow_cur = angles[rowIdx][2];
      wrist_cur = angles[rowIdx][3];
    break;
      
    case RM_S2:
      robotState = RM_S3;
      shoulder_cur = angles[rowIdx][1];
    break;
  
    case RM_S3:
      robotState = RM_S4;
    break;
    
    case RM_S4:
      Serial.println("RM_S4");
      Serial.println(numList);
      if (numList != 0) {
        robotState = RM_S1;
      }
      else {
        robotState = RM_Reset;
        moveDone = true;
        armTurn = false;
      }
      Serial.print(rowIdx);
      base_cur = angles[rowIdx][0];
      shoulder_cur = 135.0;
      elbow_cur = angles[rowIdx][2];
      wrist_cur = angles[rowIdx][3];
    break;

    default:
      robotState = RM_Reset;
    break;
  }

  //actions
  switch(robotState) {
    
    case RM_Reset:
      Serial.println("RM_Reset");
      move_servo(baseServo,base_cur,90.0);
      move_servo(shoulderServo, shoulder_cur, 180.0);
      move_servo(elbowServo,elbow_cur,90.0);
      move_servo(wristServo,wrist_cur,90.0);
      
    break;
 
    case RM_S1:
      Serial.println("RM_S1");
      rowIdx =  2 - numList;
      move_servo(baseServo,base_cur,angles[rowIdx][0]);
      move_servo(elbowServo,elbow_cur,angles[rowIdx][2]);
      move_servo(wristServo,wrist_cur,angles[rowIdx][3]);
      
//      baseServo.writeMicroseconds(angleToPulse(angles[rowIdx][0]));
//      elbowServo.writeMicroseconds(angleToPulse(angles[rowIdx][2]));
//      wristServo.writeMicroseconds(angleToPulse(170 - angles[rowIdx][3]));
    break;

    case RM_S2:
      Serial.println("RM_S2");
      move_servo(shoulderServo, shoulder_cur, angles[rowIdx][1]);
//      shoulderServo.writeMicroseconds(angleToPulse(angles[rowIdx][1]));
    break;

    case RM_S3:
      Serial.println("RM_S3");
       Serial.println(rowIdx);
    if (rowIdx == 0 || rowIdx == 2) {
        clawServo.writeMicroseconds(angleToPulse(110)); // Grasp
        Serial.println("Grasp");
      }
      else {
        clawServo.writeMicroseconds(angleToPulse(90)); // Release
        Serial.println("Release");
      }
    Serial.println("Done");
    break;
    
    case RM_S4:
      Serial.println("RM_S4");
      move_servo(shoulderServo, shoulder_cur, 135);
      numList-=1;
    Serial.println("Done");
    break;
    
  }
}

bool sent, received;
String message = "";
void ChessEngineFSM(void) {  
  switch(chessEngineState) {

    case Initial:
      if (!chessEngineTurn) {
        chessEngineState = Initial;
      }
      else {
        chessEngineState = Send;
      }
    break;
    
    case Send:
        if (!sent) {
          chessEngineState = Send;
        }
        else {
          chessEngineState = Receive;
        }
    break;
    
    case Receive:
        if (!received) {
          chessEngineState = Receive;
        }
        else {
          chessEngineState = Initial;
          chessEngineTurn = false;
          armTurn = true;
        } 
    break;
    
    default:
      chessEngineState = Initial;
    break;
  }  
  switch (chessEngineState) {
      
    case Initial:
      Serial.println("State: Initial");
    break;

    case Send:
      Serial.println("State:Send");
      Serial.println("READY");
      delay(1000);
      message = Serial.readStringUntil('\n');
      Serial.println(message == "RECEIVED"); 
      if (message == "RECEIVED"){
        sent = true;
      }
      else {
        sent = false;
      }
    break;
    
    case Receive:
        Serial.println("State:RECEIVE");
        char input[128];
        int idx = 0;
        while (Serial.available() > 0) {
            char incomingByte = Serial.read();
            if (incomingByte == '\n') {  // End of input (newline)
                input[idx] = '\0';  // Null-terminate the string
                break;
            }
            input[idx++] = incomingByte;
        }
        if (containsSemicolon(input)) {
          int numLists = parseAngles(input);
          received = true;
        }
        else {
          received = false;
        }
      
    break;
  }
}


void setup() {
  Serial.begin(9600);
  baseServo.attach(3); //base servo pin 8
  shoulderServo.attach(5); //elbow servo pin 9
  elbowServo.attach(6); //shoulder servo pin 10
  wristServo.attach(9); //wrist servo pin 11
  clawServo.attach(10); //claw servo pin 12
  armTurn = true;
  
  baseServo.writeMicroseconds(angleToPulse(90.0));
  shoulderServo.writeMicroseconds(angleToPulse(180));
  elbowServo.writeMicroseconds(angleToPulse(90.0));
  wristServo.writeMicroseconds(angleToPulse(90.0));
  clawServo.writeMicroseconds(angleToPulse(90.0));
//  pinMode(buttonPin, INPUT); //button
//  pinMode(GreenLEDPin, OUTPUT); //Green LED
//  pinMode(RedLEDPin, OUTPUT); //Red LED
}

void loop() {
//  TurnDeterminedFSM();
  //ChessEngineFSM();
  RobotFSM();
  delay(1000);
}
