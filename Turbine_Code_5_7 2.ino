//2024 WiscWind Team
//Turbine Side Arduino Code

//---Serial Communication Start---//
const char endMarker = '>';
String incomingData = "";
bool dataStarted = false;
int bufferSize = 10;
int decimalPlaces = 10;
bool loadBrake = false;
bool turbineBrakeState = false;   //SEND
float windSpeed = 0.0;            //SEND
int BrakeStateRead = 0;           //SEND
int LastBrakeStateRead = 0;       //RECEIVE
String type="";
//---Serial Communication End---//

//---Braking Setup Variables Start---//
int BTN_PIN = 4;       //pin that connects to the e-stop button
boolean btnState = 0;  //current state of the button
boolean lastBtnState = 0;
int Relay1 = 6;   // Digital pin D6
//---Braking Setup Variables End---//


//---RPM Setup Start---//
const byte PulsesPerRevolution = 6; 
const unsigned long ZeroTimeout = 100000;  
const byte numReadings = 2;  // Number of samples for smoothing. The higher, the more smoothing, but it's going to
                             // react slower to changes. 1 = no smoothing. Default: 2.
/////////////
// Variables:
/////////////

volatile unsigned long LastTimeWeMeasured;                        // Stores the last time we measured a pulse so we can calculate the period.
volatile unsigned long PeriodBetweenPulses = ZeroTimeout + 1000;  // Stores the period between pulses in microseconds.
                                                                  // It has a big number so it doesn't start with 0 which would be interpreted as a high frequency.
volatile unsigned long PeriodAverage = ZeroTimeout + 1000;        // Stores the period between pulses in microseconds in total, if we are taking multiple pulses.
                                                                  // It has a big number so it doesn't start with 0 which would be interpreted as a high frequency.
unsigned long FrequencyRaw;                                       // Calculated frequency, based on the period. This has a lot of extra decimals without the decimal point.
unsigned long FrequencyReal;                                      // Frequency without decimals.
unsigned int RPM;                                                 // Raw RPM without any processing.
unsigned int PulseCounter = 1;                                    // Counts the amount of pulse readings we took so we can average multiple pulses before calculating the period.

unsigned long PeriodSum;  // Stores the summation of all the periods to do the average.

unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;  // Stores the last time we measure a pulse in that cycle.
                                                          // We need a variable with a value that is not going to be affected by the interrupt
                                                          // because we are going to do math and functions that are going to mess up if the values
                                                          // changes in the middle of the cycle.
unsigned long CurrentMicros = micros();                   // Stores the micros in that cycle.
                                                          // We need a variable with a value that is not going to be affected by the interrupt
                                                          // because we are going to do math and functions that are going to mess up if the values
                                                          // changes in the middle of the cycle.

// We get the RPM by measuring the time between 2 or more pulses so the following will set how many pulses to
// take before calculating the RPM. 1 would be the minimum giving a result every pulse, which would feel very responsive
// even at very low speeds but also is going to be less accurate at higher speeds.
// With a value around 10 you will get a very accurate result at high speeds, but readings at lower speeds are going to be
// farther from eachother making it less "real time" at those speeds.
// There's a function that will set the value depending on the speed so this is done automatically.
unsigned int AmountOfReadings = 1;

unsigned int ZeroDebouncingExtra;  // Stores the extra value added to the ZeroTimeout to debounce it.
                                   // The ZeroTimeout needs debouncing so when the value is close to the threshold it
                                   // doesn't jump from 0 to the value. This extra value changes the threshold a little
                                   // when we show a 0.

// Variables for smoothing tachometer:
unsigned long readings[numReadings];  // The input.
unsigned long readIndex;              // The index of the current reading.
unsigned long total;                  // The running total.
unsigned long average;                // The RPM value after applying the smoothing.
//
//---RPM Setup End---//

//---Power Sensor Setup---//
#include <Adafruit_INA260.h>
Adafruit_INA260 ina260 = Adafruit_INA260();
//---Power Sensor End---//

//---Variables that i am not sure---//
float current = 0;
float voltage = 0;
float power = 0;

//---Linear Actuator Start---//
int nMOS = 8;
int pwm = 3;
double PWM_val = 80.0;
//---Linear Actuator End---//

// Control
int update = 0;
int loadCheck = 0;
bool exitLoadBrake = 0;
bool done = 0;

unsigned long startMillis;
const unsigned long threshold = 6000;

void setup() {
  //delay(5000);
  Serial1.begin(4800);  // Initialize serial communication
  Serial.begin(4800);   // Initialize communication between Arduino

  //---RPM Setup Start---//
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);  // Enable interruption pin 2 when going from LOW to HIGH.
  delay(1000);
  //---RPM Setup End---//

  //while(!Serial) { delay(10);}

  //Power Sensor Check//
  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while(1);
  }
  Serial.println("Found");

  //---Emergency Braking Setup Start---//
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(Relay1, OUTPUT);  //declare relays as output
  pinMode(nMOS, OUTPUT);
  digitalWrite(Relay1, LOW);
  digitalWrite(nMOS, LOW);
  //analogWrite(pwm, 110);
  //delay(10);

  // kp = 0.8;  
  // ki = 0.20;  
  // kd = 0.001;  
  // last_time = 0;
Serial1.print(1);  //?????
}  //SETUP END

void loop() {

  while (true) {

  digitalWrite (nMOS, HIGH);
      analogWrite(pwm, 150);
      //delay(3000);
    if (RPM >=  200 && RPM < 699){
      digitalWrite (nMOS, HIGH);
      analogWrite(pwm, 150);
      //delay(3000);
    }
    if (RPM >700 && RPM <= 1000){
       digitalWrite(nMOS, HIGH);
      analogWrite(pwm, 140);
      // delay(3000);
    }
    if (RPM >= 1001 && RPM <= 1500){
       digitalWrite(nMOS, HIGH);
      analogWrite(pwm, 120);
      //delay(3000);
    }

    if (RPM >= 1450 && RPM <= 1550) {
       digitalWrite(nMOS, HIGH);
      analogWrite(pwm, 140);
    }
    if (RPM >= 1550 && RPM <= 1600) {
       digitalWrite(nMOS, HIGH);
      analogWrite(pwm, 155);
    }

    if (RPM >= 1600){
      digitalWrite(nMOS, HIGH);
      analogWrite(pwm, 80);
      digitalWrite(Relay1, HIGH);
      //delay(10000);
    }

    Serial.print("loadCheck: ");
    Serial.println(loadCheck);
    Serial.print("update: ");
    Serial.println(update);

//Load Disconnect Logic
  // if (startMillis - threshold >= 0){
  //   Serial1.print('m');
  // }

  if(btnState == LOW && update == 1 && current <=0.1 && power <=0.2 && RPM >= 150 && loadCheck != 10){
    loadBrake = 1;
    digitalWrite(nMOS, HIGH);
    analogWrite(pwm, 80);
    Serial1.print('z');
    loadCheck += 1;
    delay(5000);
      }
if(loadCheck == 2){
  Serial1.print('x');
}
   if (exitLoadBrake && loadCheck != 2){
     digitalWrite(nMOS, HIGH);
      analogWrite(pwm, 140);
      loadCheck = 2;
      delay(3000);
      Serial1.print('x');
    }
    
   if (done){
    Serial1.print('c');
   }
  // //---RPM Loop Start---//
  LastTimeCycleMeasure = LastTimeWeMeasured;  // Store the LastTimeWeMeasured in a variable.
  CurrentMicros = micros();
  if (CurrentMicros < LastTimeCycleMeasure) {
    LastTimeCycleMeasure = CurrentMicros;
  }
  FrequencyRaw = 10000000000 / PeriodAverage;                                                                                                 // Calculate the frequency using the period between pulses.
  if (PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {  // If the pulses are too far apart that we reached the timeout for zero:
    FrequencyRaw = 0;                                                                                                                         // Set frequency as 0.
    ZeroDebouncingExtra = 2000;                                                                                                               // Change the threshold a little so it doesn't bounce.
  } else {
    ZeroDebouncingExtra = 0;  // Reset the threshold to the normal value so it doesn't bounce.
  }
  FrequencyReal = FrequencyRaw / 10000;  // Get frequency without decimals.

  // Calculate the RPM:
  RPM = FrequencyRaw / PulsesPerRevolution * 60;  // Frequency divided by amount of pulses per revolution multiply by 60 seconds to get minutes.
  RPM = RPM / 10000;                              // Remove the decimals.
  // Smoothing RPM:
  total = total - readings[readIndex];  // Advance to the next position in the array.
  readings[readIndex] = RPM;            // Take
  //s the value that we are going to smooth.
  total = total + readings[readIndex];  // Add the reading to the total.
  readIndex = readIndex + 1;            // Advance to the next position in the array.

  if (readIndex >= numReadings) {  // If we're at the end of the array:
    readIndex = 0;                 // Reset array index.
  }
  // Calculate the average:
  average = total / numReadings;  // The average value it's the smoothed result.

  Serial.print("\tRPM: ");
  Serial.print(RPM);
  Serial.print("\tTachometer: ");
  Serial.println(average);

  // //---RPM Loop End---//
 

  //---Emergency Braking Start---//
  btnState = digitalRead(BTN_PIN);  //E-STOP Button Pressed Braking

  if (((btnState != lastBtnState) & (btnState == HIGH))) {
    Serial.println("Button Pressed");
    lastBtnState = btnState;
    turbineBrakeState = btnState;
    Serial.println("Turbine Brake Status: " + turbineBrakeState);
    Serial.println("Pitched");
    //SHORT GENERATOR
    digitalWrite(Relay1, HIGH);
  }

  if (((btnState != lastBtnState) & (btnState == LOW))) {  //Button released
    Serial.println("Button Released");
    lastBtnState = btnState;
    turbineBrakeState = btnState;
    Serial.println("Pitched");
    digitalWrite(Relay1, LOW);
    Serial.println("BRAKE NOW: " + turbineBrakeState);
    update = 1;    
    startMillis = millis();
  }
  //---Emergency Braking End---//

  voltage = ina260.readBusVoltage() / 1000;
  Serial.print(voltage);
  Serial.print(" V  ");
  Serial. print(current);
  Serial.print("A  ");
  Serial.print(power);
  Serial.print("W  ");
  current =ina260.readCurrent() / 1000;
  power = ina260.readPower() / 1000;


while(Serial1.available()){
  //Serial1

    char incomingChar = Serial1.read();
    if (incomingChar == 'l') {
      exitLoadBrake = 1;
      Serial.println();
      Serial.println("EXIT LOAD BRAKE");
    } else if (incomingChar == 't') {
      Serial.println();
      Serial.println("Not Load Disconnect");
      loadBrake =0;
    } else if (incomingChar == 'h') {
      done = 1;
    } else if(incomingChar=='p'){
      type="Power: ";  
      dataStarted=true;
      incomingData=""; 
    }else if(dataStarted&&incomingChar==endMarker){
      dataStarted=false;
      Serial.println("Received "+type+incomingData);
      power=incomingData.toFloat();
    } else if(dataStarted){
      incomingData+=incomingChar;
    }
}
    char data[15];  
      if(turbineBrakeState){
        Serial1.print('t');     
      } else {
        Serial1.print('f');
      }

    itoa(average, data, 10);  //SEND RPM
    sendPacket(data, 'r');    //SEND RPM


  //SERIAL COMMUNICATION END
  Serial.println();
  Serial.print("Turbine Brake Status: ");
  Serial.println(turbineBrakeState);

    delay(500);
  }
}

void sendPacket(char data[], char denom) {
  Serial1.print(denom);
  Serial1.print(data);
  Serial1.print(endMarker);
}

void Pulse_Event() {// The interrupt runs this to calculate the period between pulses:
   
  PeriodBetweenPulses = micros() - LastTimeWeMeasured;  // Current "micros" minus the old "micros" when the last pulse happens.
  LastTimeWeMeasured = micros();                        // Stores the current micros so the next time we have a pulse we would have something to compare with.

  if (PulseCounter >= AmountOfReadings)  // If counter for amount of readings reach the set limit:
  {
    PeriodAverage = PeriodSum / AmountOfReadings;  // Calculate the final period dividing the sum of all readings by the
                                                   // amount of readings to get the average.
    PulseCounter = 1;                              // Reset the counter to start over. The reset value is 1 because its the minimum setting allowed (1 reading).
    PeriodSum = PeriodBetweenPulses;               // Reset PeriodSum to start a new averaging operation.

    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);  // Remap the period range to the reading range.

    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);  // Constrain the value so it doesn't go below or above the limits.
    AmountOfReadings = RemapedAmountOfReadings;                           // Set amount of readings as the remaped value.
  } else {
    PulseCounter++;                               // Increase the counter for amount of readings by 1.
    PeriodSum = PeriodSum + PeriodBetweenPulses;  // Add the periods so later we can average.
  }
}




