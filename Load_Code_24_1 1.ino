//2024 WiscWind Team
//Load Side Arduino Mega Code
//

//aa

//---Serial Communication Setup Start---//
const char endMarker = '>';
//RECEIVING DATA START//
String incomingRPM = "";  //String for RPM Values
String incomingData = "";
bool eSTOP = false;  //String for Turbine Brake State
//String voltageTurbine = "";      //String for Voltage Reading from Turbine
boolean dataStarted = false;  
float RPM = 0;
//RECEIVING DATA END//

//SENDING DATA START//
float powerSend = 0.0;        // Output Power Reading to be sent to Turbine
bool loadBrakeState = false;  // 0 for LOW ; 1 for HIGH
//SENDING DATA END//

String type = "";
//---Serial Communication Setup End---//

//---Power Sensor Setup---//
#include <Adafruit_INA260.h>
Adafruit_INA260 ina260 = Adafruit_INA260();
//---Power Sensor End---//

//---Variables that i am not sure---//
float current = 0;
float voltage = 0;
float power = 0;
int prev_eSTOP = 0;
int lastLoadBrakeState = 0;
int eSTOP_Count = 0;
//---Variables that i am not sure---//

int Relayy = 10;  //Set the relay to pin 11

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;


int loadCheck = 0;
bool done1 = 0;
bool ready = 0;
bool checkcheck = 0;

void setup() {
  Serial1.begin(4800);  // Initialize serial communication btw Arduino 1 & 2
  Serial.begin(4800);   // Intitialize comm btw Arduino and PC

//while(!Serial) { delay(10);}
  // //Power Sensor Check//
  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while(1);
  }
  Serial.println("Found");

  pinMode(Relayy, OUTPUT);    
  digitalWrite(Relayy, LOW);  


  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);

  digitalWrite(2, LOW);    //20
  digitalWrite(3, LOW);    //5
  digitalWrite(4, HIGH);   //200
  digitalWrite(5, LOW);    //10
  digitalWrite(6, HIGH);   //120
  digitalWrite(7, LOW);    //39
  digitalWrite(9, LOW); 
  //LoadSwitching//

  //load disconnect logic
  // startMillis = millis();
}
//---SETUP END---//

void loop() {
Serial1.print(9);
    while(true) {


    //  if (loadBrakeState){
    //   digitalWrite(Relayy, HIGH);
    //   delay(10000);
    //   Serial1.print('l');
    //  }
    //  if (!loadBrakeState && !done1){
    //   Serial1.print('h');
    //   delay(2000);
    //   Serial.print("a");
    //   digitalWrite(Relayy, LOW);
    //  }

    while(Serial1.available()){
    char incomingChar = Serial1.read();
      if (incomingChar == 'r') {
        dataStarted = true;
        incomingData = "";
        type= "RPM: ";
        }else if (incomingChar == 't') {
          eSTOP = true;
        }else if (incomingChar == 'f') {
          eSTOP = false;
        }else if (incomingChar == 'z') {
          if (eSTOP_Count == 1){
          loadBrakeState = 1; 
          }
        }else if (incomingChar == 'm') {
          ready = 1;
        }else if (incomingChar == 'x') {
          checkcheck = 1;
          Serial.print("done xxx");
          loadBrakeState = 0;
        }else if (incomingChar == 'c') {
          done1 = 1;
        }else if (dataStarted && incomingChar == endMarker) {
          dataStarted = false;
          Serial.println("Received " + type + incomingData);
          if (type.equals("RPM: ")) {
            RPM = incomingData.toFloat();
            }
            incomingData = "";
        }else if (dataStarted) {
          incomingData += incomingChar;
      }
    }
    
 
  current = ina260.readCurrent() / 1000;
  Serial.print(current);
  Serial.print("A ");
  Serial.print(",");

  voltage = ina260.readBusVoltage() / 1000;
  Serial.print(voltage);
  Serial.print(" V");
  Serial.print(",");

  power = ina260.readPower() / 1000;
  Serial.print(power);
  Serial.println(" W");   

  if(current > 0.75){
    digitalWrite(9, HIGH);
  } 

    emergencyBrake(eSTOP);
   disconnectLoad( eSTOP_Count, current, eSTOP, RPM, ready, checkcheck);
    loadSwitching(RPM, voltage);

      char data[15];
      dtostrf(power, 10, 10, data);
     // sendPacket(data, 'p');


       delay(200);
    
  }
}  // LOOP END


void sendPacket(char data[], char denom) {
    Serial1.print(denom);
    Serial1.print(data);
    Serial1.print(endMarker);
}
void processReceivedData(String data) {
  Serial.print("Received: " + type);
  Serial.println(data.toFloat(), 10);
}
void loadSwitching(float RPM, float voltage) {   //120  5
  if (RPM >= 0 && RPM < 200) { //5m/s   !!!
    digitalWrite(2, LOW);    //20
    digitalWrite(3, LOW);    //5
    digitalWrite(4, HIGH);   //200
    digitalWrite(5, LOW);    //10
    digitalWrite(6, HIGH);   //120
    digitalWrite(7, LOW);    //39

    } else if (RPM >= 201 && RPM < 251) {  // 40  4    200  3
     digitalWrite(2, LOW);    //20
    digitalWrite(3, LOW);    //5
    digitalWrite(4, HIGH);   //200
    digitalWrite(5, LOW);    //10
    digitalWrite(6, HIGH);   //120
    digitalWrite(7, LOW);    //39

      } else if (RPM >= 252 && RPM < 350) { //40 4    50 6
       digitalWrite(2, LOW);    //20
    digitalWrite(3, LOW);    //5
    digitalWrite(4, HIGH);   //200
    digitalWrite(5, LOW);    //10
    digitalWrite(6, HIGH);   //120
    digitalWrite(7, LOW);    //39
        } else if (RPM >= 353 && RPM < 405) {//  40 4     50 6     120 5
          digitalWrite(2, LOW);    //20
          digitalWrite(3, LOW);    //5
          digitalWrite(4, HIGH);   //200
          digitalWrite(5, HIGH);    //10
          digitalWrite(6, HIGH);   //120
          digitalWrite(7, LOW);  //60

          } else if (RPM >= 405 && RPM < 601 || (voltage >= 11 && voltage <= 11.4)) {// 20 2      50 6
            digitalWrite(2, HIGH);    //20
            digitalWrite(3, LOW);    //5
            digitalWrite(4, LOW);   //200
            digitalWrite(5, LOW);    //10
            digitalWrite(6, HIGH);   //120
            digitalWrite(7, LOW);   //60

        } else if (RPM >= 602 && RPM < 805 || (voltage >= 11.5 && voltage <12.5 )) {// 10 7    40 4    50 6   120 5   200 3
          digitalWrite(2, LOW);    //20
          digitalWrite(3, HIGH);    //5
          digitalWrite(4, HIGH);   //200
          digitalWrite(5, HIGH);    //10
          digitalWrite(6, HIGH);   //120
          digitalWrite(7, HIGH);   //10M/S

      } else if (RPM >= 806 && RPM < 2000 || voltage >= 12.6) {   // 10 7    50 6
        digitalWrite(2, LOW);    //20
        digitalWrite(3, LOW);    //5
        digitalWrite(4, LOW);   //200
        digitalWrite(5, LOW);    //10
        digitalWrite(6, LOW);   //120
        digitalWrite(7, HIGH); 
     } else {
    digitalWrite(2, LOW);    //20
    digitalWrite(3, LOW);    //5
    digitalWrite(4, LOW);   //200
    digitalWrite(5, LOW);    //10
    digitalWrite(6, LOW);   //120
    digitalWrite(7, HIGH);  //60
    }
}
void emergencyBrake(bool eStop) {

  if (eSTOP) {  //E-Stop Braking Begin
      digitalWrite(Relayy, HIGH);          //Low is off and High is on
      prev_eSTOP = 1;
      eSTOP_Count = 1;
      Serial.println("Break");
    }

  if ((prev_eSTOP == 1) && (!eSTOP)) {  //E-stop Mode OFF, Normal Operation ON                                                                                 // DELAY IS NEEDED TO PITCH
    Serial.println("Normal State");
    delay(3000);
    digitalWrite(Relayy, LOW);  //Low is off and High is on
    prev_eSTOP = 0;
    } 
}
void disconnectLoad(int eSTOP_Count, float current, bool eSTOP, float RPM, bool ready, bool checkcheck){
  if ((eSTOP_Count == 1 && current <=0 && RPM >=500) && voltage <= 0.5 && ready && !checkcheck) {
    delay(1000);
  }
  if ((!eSTOP && eSTOP_Count == 1  && current <= 0 && voltage <=0 &&  RPM>= 500)){ //|| (!eSTOP && RPM>= 300 && current <=0)
    loadBrakeState = true;
    digitalWrite(Relayy, HIGH);
    Serial1.print('l');
    Serial.println("Load Disconnect");
    //delay(10000);
    lastLoadBrakeState = 1;
  }
}
