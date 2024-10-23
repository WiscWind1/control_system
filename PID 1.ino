#include <Servo.h>
Servo myServo;
#define PIN_SERVO (3)
double dt, last_time;
 double integral, previous, output = 0;
 double kp, ki, kd;
 double setpoint = 75.00;  // RPM at 11m/s??
 float stroke;    

 double RPM; //


void setup(){
myServo.attach(PIN_SERVO);
SetStrokePerc(30.6);  // put the min stroke!!!
    delay(10000);
 kp = 0.8;   
   ki = 0.20;   
   kd = 0.001;  
   last_time = 0;
   Serial.begin(9600);
   for(int i = 0; i < 50; i++)
   {
     Serial.print(setpoint);
     Serial.print(",");
     Serial.println(0);
     delay(100);
   }
   delay(100);
}


void loop() {

  if(RPM > setpoint-30 ){             //start the PID control mode, 30?
   double now = millis();
   dt = (now - last_time)/1000.00;
   last_time = now;
 
  double actual = RPM;
   double error = setpoint - actual;
   output = pid(error);
 stroke = stroke - output;     // error+   PD+, P needed to become smaller to increase RPM
 
  // Setpoint VS Actual
   Serial.print(setpoint);
   Serial.print(",");
   Serial.println(actual);
  // Error
   //Serial.println(error);
 
  delay(300);
 }


SetStrokePerc(stroke);
    delay(5000);
}

void SetStrokePerc(float strokePercentage) {
  if ( strokePercentage >= 1.0 && strokePercentage <= 99.0 ) { //set min and max)
    int usec = 1000 + strokePercentage * ( 2000 - 1000 ) / 100.0 ;
    myServo.writeMicroseconds( usec );
}''
}

 float pid(double error)
 {
   double proportional = error;
   integral += error * dt;
   double derivative = (error - previous) / dt;
   previous = error;
   double output = (kp * proportional) + (ki * integral) + (kd * derivative);   
   return output;
 }
 
