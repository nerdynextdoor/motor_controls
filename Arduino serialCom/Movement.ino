const int stepDL_dirPin = 3;
const int stepDL_stepPin = 2;
const int stepDR_dirPin = 5;
const int stepDR_stepPin = 4;
const int stepUL_dirPin = 7;
const int stepUL_stepPin = 6;
const int stepUR_dirPin = 9;
const int stepUR_stepPin = 8;
int steps = 200;
int stepDelay;
bool custom = false;
void setup() {
  Serial.begin(9600);
  pinMode(stepDL_dirPin, OUTPUT);
  pinMode(stepDL_stepPin, OUTPUT);
  pinMode(stepDR_dirPin, OUTPUT);
  pinMode(stepDR_stepPin, OUTPUT);
  pinMode(stepUL_dirPin, OUTPUT);
  pinMode(stepUL_stepPin, OUTPUT);
  pinMode(stepUR_dirPin, OUTPUT);
  pinMode(stepUR_stepPin, OUTPUT);

}
void loop() {
  if (Serial.available() > 0) {
   // String a[5];
    String Rx[5];
    for (int i = 0; i < 5; i++) {
      Rx[i] = Serial.readStringUntil(',');
     // Rx[i] = a[i];
      
    }
    if (Rx[0] != "_"){
      return;
    }
   // Serial.print(Rx[0]+","+Rx[1]+","+Rx[2]+","+Rx[3]+","+Rx[4]);
  // Serial.println("sssssssssssss");
    steps= Rx[4].toInt();
   // Serial.println(steps);
    switch (Rx[1].toInt()) {
      case 1:
        One_StepPerSec();
        break;

      case 5:
        Five_StepsPerSec();
        break;

      case 15:
        Fifteen_StepsPerSec();
        break;

      case 30:
        Thirty_StepsPerSec();
        break;

      case 32:
        ThirtyTwo_StepsPerSec();
        break;

      case 64:
        SixtyFour_StepsPerSec();
        break;
      default:
        custom = true;
        stepDelay = Rx[1].toInt();
        break;
    };
    
    switch (Rx[2].charAt(0)) {
      case 'm':
        Moveforward_Back(Rx[3].toInt());
        break;

      case 'u':
        // 5StepsPerSec();
        break;

      case 's':
        ShiftLeft_Right(Rx[3].toInt());
        break;

      case 'r':
        RotateLeft_Right(Rx[3].toInt());
        break;

      default:
       // stepDelay = 250;
        break;
    };
    
  }

}

void One_StepPerSec()
{
  stepDelay = 1000;
  custom = false;
}

void Five_StepsPerSec()
{
  stepDelay = 200;
  custom = false;
}


void Fifteen_StepsPerSec()
{
  stepDelay = 66;
  custom = false;
}

void Thirty_StepsPerSec()
{
  stepDelay = 33;
  custom = false;
}

void ThirtyTwo_StepsPerSec()
{
  stepDelay = 31;
  custom = false;
}

void SixtyFour_StepsPerSec()
{
  stepDelay = 15;
  custom = false;
}


void delayTime(int DT)
{
  if(custom == false)
  {
    delay(DT);
  }
  else{
    delayMicroseconds(DT);
  }
  //Serial.println(custom);
}

void Moveforward_Back(int dir) {
  switch (dir) {
    case 1:
      MoveForward();
      break;
    case 2:
      MoveBack();
      break;
    default:
      return;
        break;
  }

  //stepDelay = 250;
  for (int x = 0; x < steps; x++) {
    AllStepPinHigh();
    //delay(1000);
    delayTime(stepDelay);
    AllStepPinLow();
    // delay(1000);
    delayTime(stepDelay);
  //  Serial.println(x);
  }
  // delay(1000);
}

void ShiftLeft_Right(int dir) {
  switch (dir) {
    case 1:
      ShiftLeft();
      break;
    case 2:
      ShiftRght();
      break;
    default:
      return;
        break;
  }
  //    stepDelay = 150;
  for (int x = 0; x < steps; x++) {
    AllStepPinHigh();
    delayTime(stepDelay);
    AllStepPinLow();
    delayTime(stepDelay);
  }
  //    delay(1000);
}

void RotateLeft_Right(int dir) {
  switch (dir) {
    case 1:
      RotateLeft();
      break;
    case 2:
      RotateRight();
      break;
    default:
      return;
        break;
  }
  //    stepDelay = 150;
  for (int x = 0; x < steps; x++) {
    AllStepPinHigh();
    delayTime(stepDelay);
    AllStepPinLow();
    delayTime(stepDelay);
  }
  //    delay(1000);
}

//****************************************//
//************ Send pulse ****************//
void AllStepPinHigh() {
  digitalWrite(stepDL_stepPin, HIGH);
  digitalWrite(stepDR_stepPin, HIGH);
  digitalWrite(stepUL_stepPin, HIGH);
  digitalWrite(stepUR_stepPin, HIGH);
}

void AllStepPinLow() {
  digitalWrite(stepDL_stepPin, LOW);
  digitalWrite(stepDR_stepPin, LOW);
  digitalWrite(stepUL_stepPin, LOW);
  digitalWrite(stepUR_stepPin, LOW);
}

//*******************************************//

//*******************************************//
//**********Movement Direction***************//
void MoveForward() {
  digitalWrite(stepDL_dirPin, LOW);
  digitalWrite(stepDR_dirPin, HIGH);
  digitalWrite(stepUL_dirPin, LOW);
  digitalWrite(stepUR_dirPin, HIGH);
}

void MoveBack() {
  digitalWrite(stepDL_dirPin, HIGH);
  digitalWrite(stepDR_dirPin, LOW);
  digitalWrite(stepUL_dirPin, HIGH);
  digitalWrite(stepUR_dirPin, LOW);
}

void ShiftRght() {
  digitalWrite(stepDL_dirPin, HIGH);
  digitalWrite(stepDR_dirPin, HIGH);
  digitalWrite(stepUL_dirPin, LOW);
  digitalWrite(stepUR_dirPin, LOW);
}

void ShiftLeft() {
  digitalWrite(stepDL_dirPin, LOW);
  digitalWrite(stepDR_dirPin, LOW);
  digitalWrite(stepUL_dirPin, HIGH);
  digitalWrite(stepUR_dirPin, HIGH);
}

void RotateRight() {
  digitalWrite(stepDL_dirPin, LOW);
  digitalWrite(stepDR_dirPin, LOW);
  digitalWrite(stepUL_dirPin, LOW);
  digitalWrite(stepUR_dirPin, LOW);
}

void RotateLeft() {
  digitalWrite(stepDL_dirPin, HIGH);
  digitalWrite(stepDR_dirPin, HIGH);
  digitalWrite(stepUL_dirPin, HIGH);
  digitalWrite(stepUR_dirPin, HIGH);
}
