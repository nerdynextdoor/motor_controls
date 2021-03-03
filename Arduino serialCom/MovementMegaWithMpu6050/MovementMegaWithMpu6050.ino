#include <Wire.h>

const int stepDL_dirPin = 13;
const int stepDL_stepPin = 12;
const int stepDR_dirPin = 6;
const int stepDR_stepPin = 3;
const int stepUL_dirPin = 7;
const int stepUL_stepPin = 4;
const int stepUR_dirPin = 5;
const int stepUR_stepPin = 2;
int steps = 200;
int stepDelay;
bool custom = false;
bool isrotating = false;
float angleToRotate;


//Mpu6050 stuff
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

//*******************************************//


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


  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission

  // Call this function if you need to get the IMU error values for your module
  // calculate_IMU_error();
  delay(20);
}
void loop() {
  if (Serial.available() > 0) {
   // String a[5];
    String Rx[6];
    for (int i = 0; i < 6; i++) {
      Rx[i] = Serial.readStringUntil(',');
     // Rx[i] = a[i];
      
    }
    if (Rx[0] == "+ok"){
      Serial.println("okok");
      }
    if (Rx[0] != "_"){
      return;
    }
   // Serial.print(Rx[0]+","+Rx[1]+","+Rx[2]+","+Rx[3]+","+Rx[4]);
  // Serial.println("sssssssssssss");
    steps= Rx[4].toInt();
    angleToRotate = Rx[5].toInt();
    
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
        Mpu6050Reading();
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
      angleToRotate = yaw + angleToRotate;
      break;
    case 2:
      RotateRight();
      angleToRotate = yaw - angleToRotate;
      break;
    default:
      return;
        break;
  }
  int j;
      steps = 10000;
  for (int x = 0; x < steps; x++) {
    Mpu6050Reading();
    AllStepPinHigh();
    delayTime(stepDelay);
    AllStepPinLow();
    delayTime(stepDelay);
    j = angleToRotate - yaw;
    if (j == 0){
      Serial.println("okok");
      break;
    }
    Serial.println(j);
      
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
  Serial.println("moving forward");
  digitalWrite(stepDL_dirPin, LOW);
  digitalWrite(stepDR_dirPin, HIGH);
  digitalWrite(stepUL_dirPin, LOW);
  digitalWrite(stepUR_dirPin, HIGH);
}

void MoveBack() {
  Serial.println("moving backwards");
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

void Mpu6050Reading(){
   // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) + 1.32; // AccErrorX ~(-1.10) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 4.16; // AccErrorY ~(-4.27)

  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
 // GyroX = GyroX + 0.99; // GyroErrorX ~(-0.24)
 // GyroY = GyroY + 0.97; // GyroErrorY ~(-0.26)
  GyroZ = GyroZ + 0.59; // GyroErrorZ ~ (-0.15)

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
 // gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
 // gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;

  // Complementary filter - combine acceleromter and gyro angle values
 // roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
 // pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  // Print the values on the serial monitor
 // Serial.print(roll);
 // Serial.print("/");
 // Serial.print(pitch);
 // Serial.print("/");
  //Serial.println(yaw);
  
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    //GyroX = Wire.read() << 8 | Wire.read();
   // GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
   // GyroErrorX = GyroErrorX + (GyroX / 131.0);
   // GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
 // GyroErrorX = GyroErrorX / 200;
 // GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
 // Serial.print("AccErrorX: ");
 // Serial.println(AccErrorX);
 // Serial.print("AccErrorY: ");
 // Serial.println(AccErrorY);
 // Serial.print("GyroErrorX: ");
 // Serial.println(GyroErrorX);
//  Serial.print("GyroErrorY: ");
 // Serial.println(GyroErrorY);
  //Serial.print("GyroErrorZ: ");
  //Serial.println(GyroErrorZ);
}
