//State Machines for Senior Design Chess Robot
enum STATE {S1, S2, S3} gState;
//S1 is state where player is making move. Robot is waiting to receive move
//S2 Chess engine decides move for robot. Sends signal to robot
//S3 is when robot makes move. It then resets back to original position. 

void RobotFSM(void) {

  //transitions
  switch(gstate) {
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
  //pinMode servo 1
  //pinMode servo 2
  //pinMode servo 3
  //pinMode servo 4
  //pinMode servo 5
  //pinMode button
  //

}

void loop() {
  RobotFSM();
  delay(100);

}
