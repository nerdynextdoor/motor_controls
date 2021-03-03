
#include <AccelStepper.h>

// Define some steppers and the pins the will use
AccelStepper stepDL(1,2,3), stepDR(1,4,5), stepUL(1,6,7), stepUR(1,8,9), stepUP(1,10,11);

void setup(){  
  Serial.begin(9600);  
  stepDL.setMaxSpeed(1000);
  stepDL.setAcceleration(800);
  stepDR.setMaxSpeed(1000);
  stepDR.setAcceleration(800);
  stepUL.setMaxSpeed(1000);
  stepUL.setAcceleration(800);
  stepUR.setMaxSpeed(1000);
  stepUR.setAcceleration(800);
  stepUP.setMaxSpeed(1000);
  stepUP.setAcceleration(800);
}

void loop()
{
  steprun();
}

void steprun(){

  if(Serial.available() > 0){
     // String package = Serial.readString();;
      //Serial.print("character recieved: ");
      //Serial.println(package);
      String a[5];
      int stepper_pos[5];
      for (int i=0; i<5; i++){
          a[i] = Serial.readStringUntil(',');
          stepper_pos[i] = a[i].toInt();
          Serial.print(stepper_pos[i]);
      }
      stepDL.moveTo(-stepper_pos[0]);
      stepDR.moveTo(stepper_pos[1]);
      stepUL.moveTo(-stepper_pos[2]);
      stepUR.moveTo(stepper_pos[3]);
      stepUP.moveTo(stepper_pos[4]);
  }
  
      stepDL.run();
      stepDR.run();
      stepUL.run();
      stepUR.run();
      stepUP.run();
}
