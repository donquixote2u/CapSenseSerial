#include <CapacitiveSensor.h>
#include <SoftwareSerial.h>
/*  Arduino Capacitive Sensor with serial interface for threshold setting   14/8/18   Bruce Woolmore
 * send "|" for command mode, then decimal value of next char * 10 = threshold for alert (sets alert pin, sends value to serial port)
 * UNO uses D5,D7 for soft serial, D4,D2 for cap sense send/sense pins, D6 for digital LOW alert
 * Attiny85 uses D0,D2 (chip ins 5,7) for soft serial, D4,D3 (chip pins 3,2) for cap sense send/sense pins, D1 (chip pin 6) for digital LOW alert
 */
CapacitiveSensor   cs = CapacitiveSensor(4,2);        // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire and or foil if desired
#define RX 5 // *** D0, Pin 5  UNO D5
#define TX 7 // *** D2, Pin 7  UNO D7
#define ALERT 6  // digital out pin for threshold alert (D1, chip pin 6) (UNO D6)
#define DELAY 100    // delay in millisecs between cap tests
SoftwareSerial Sercom(RX, TX);

bool cmdMode=false;
int Threshold=500; 
int Inbyte;

void setup()                    
{
   cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Sercom.begin(19200);
   Serial.begin(115200);
   pinMode(ALERT,OUTPUT);
   digitalWrite(ALERT, HIGH);
}

void loop()                    
{
   while(Sercom.available()>0)
       {
        Inbyte=Sercom.read();
        // DEBUG Serial.print("READ IN:\n");Serial.write(Inbyte);Serial.write(':');Serial.print(Inbyte,DEC);Serial.write('\n');
        if(cmdMode) 
          {
            Threshold=Inbyte*10;
            cmdMode=false;
            Serial.print("Threshold set to "); Serial.print(Threshold,DEC);Serial.write('\n');
          }
        else  // not cmd mode
          {
            if(Inbyte=='|')
                {
                  cmdMode=true;
                  // DEBUG Serial.print("CMD mode ON\n");
                }
          }      
       }
    long SenseNum =  cs.capacitiveSensor(30);
    if(SenseNum>Threshold)
       {
        Sercom.print(SenseNum);                  // print sensor output 
        Sercom.write('\n');
        digitalWrite(ALERT,LOW);
        delay(200);
        digitalWrite(ALERT,HIGH);
        Serial.print("Threshold is "); Serial.print(Threshold,DEC);
        Serial.print(" Sensenum is "); Serial.print(SenseNum,DEC); Serial.write('\n');
       }
    delay(DELAY);                             // arbitrary delay to limit data to serial port 
}

