#include<SoftwareSerial.h> //Included SoftwareSerial Library
//Started SoftwareSerial at RX and TX pin of ESP8266/NodeMCU
SoftwareSerial s(3,1);

const int sensorIn = A0;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 185;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

unsigned long start_t;
float callibrate = 0;

void setup() {
  s.begin(9600);
  pinMode(sensorIn, INPUT);
  
  for(int i = 0; i < 3; i++) {
     Voltage = getVPP();
     VRMS = (Voltage/2.0) *0.707;
     AmpsRMS = (VRMS * 1000)/mVperAmp;

     if(AmpsRMS > callibrate) {
        callibrate = AmpsRMS;
     }
  }

  s.print("Calibrated Value: ");
  s.println(callibrate);
}

void loop() {
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000)/mVperAmp) - (callibrate);  
  
  if(AmpsRMS < 0) {
     s.println("0");
  } else if(AmpsRMS >= 0.17 && AmpsRMS <= 0.28 ){
     s.println("0.10");
  } else if(AmpsRMS >= 0.32 && AmpsRMS <= 0.38) {
    s.println("0.15");
  } else {
    s.println(AmpsRMS);
  }
  
  delay(500);
}

float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 1024;      // store min value here ESP32 ADC resolution

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(sensorIn);
    
    if (readValue > maxValue)
    {
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 5.0) / 1024.0; //ESP32 ADC resolution 4096

  return result;
}
