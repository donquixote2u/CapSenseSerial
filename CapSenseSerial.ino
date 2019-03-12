#include <CapacitiveSensor.h>
#include <SoftwareSerial.h>
/*  Arduino Capacitive Sensor with serial interface for threshold setting   14/8/18   Bruce Woolmore
 * send "|" for command mode, then decimal value of next char * 10 = threshold for alert (sets alert pin, sends value to serial port)
 * UNO uses D5,D7 for soft serial, D4,D2 for cap sense send/sense pins, D6 for digital LOW alert
 * Attiny85 uses D0,D2 (chip ins 5,7) for soft serial, D4,D3 (chip pins 3,2) for cap sense send/sense pins, D1 (chip pin 6) for digital LOW alert
 */
#define UNO   // comment out for Attiny version, serial debug dropped 
#define RX 5 // *** D0, Pin 5  UNO D5
#define TX 7 // *** D2, Pin 7  UNO D7
#define ALERT 6  // digital out pin for threshold alert (D1, chip pin 6) (UNO D6)
#define DELAY 100    // delay in millisecs between cap tests
#define CapSendPin 4    // D4 used for both Uno and Attiny85
#ifdef UNO
#define CapSensePin 2   // used 2 for Uno, 3 for Attiny85
#else
#define CapSensePin 3   // used 3 for Attiny85
#endif
#define SensorRate 9600
CapacitiveSensor   cs = CapacitiveSensor(CapSendPin, CapSensePin);        // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire and or foil if desired
SoftwareSerial Sercom(RX, TX);

bool cmdMode=false;
bool dbgMode=true;
int Threshold=500; 
int Inbyte;
int tval;

int Calibrate()
{
 long  max=0; long z=0;
 for (int i=0; i <= 50; i++)
    { // get highest of 50 consecutive reads
      z=cs.capacitiveSensor(30);
      if(z>max) {max=z;}
      delay(20);
    }
 max=max*1.1; // calc threshold at 10% over highest   
 diag(nprint(max));
 return max;     
}

void diag(char* P)
  {if (dbgMode) {Sercom.print(P);}  }

char* nprint(int N)
  {
    char buffer[100];
    sprintf(buffer,"%d \n",N);
    return buffer;
  } 

void setup()                    
{
   cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Sercom.begin(SensorRate);
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
        if(Inbyte>57)
          {
             if((cmdMode)&&(Inbyte==67)) // Command=C=Calibrate
                {
                  Threshold=Calibrate();
                  diag(nprint(Threshold));
                }
             else
                 {if((cmdMode)&&(Inbyte==68))  // Command="D"=toggle Debug mode
                            {if(!dbgMode) 
                              {dbgMode=true; diag("debug mode ON");}
                             else {diag("debug mode OFF"); dbgMode=false;}  
                             }  
                  else          
                    {if(Inbyte==81)  // Command="Q"=Query Threshold value
                            {diag(nprint(Threshold)); }
                    else if(Inbyte==126)  // Command="~"=Command Mode ON
                          {
                            cmdMode=true;
                            tval=0;
                            diag("CMD mode ON\n");
                          }
                    }   // end I=81
                 }      // end I=68
            }     // end I>57
        else  //  Inbyte .LE. 57
           {
            if((Inbyte>47)&&(cmdMode))
               {
                tval=Inbyte&0x40;
                diag("tval:");
                diag(nprint(tval));
                Threshold=(Threshold*10)+tval;
               }
            else // inbyte <47 
              {
                 if(Inbyte==10)
                   {cmdMode=false;
                   Threshold=tval;
                   diag(nprint(Threshold));
                    } 
                 else {cmdMode=false;}    
               }
           }    // end Inbyte .LE. 57                            

        }              // end while serial
    long SenseNum =  cs.capacitiveSensor(30);
    if(SenseNum>Threshold)
       {
        diag(nprint(SenseNum));                  // print sensor output 
        digitalWrite(ALERT,LOW);
        delay(200);
        digitalWrite(ALERT,HIGH);
        #ifdef UNO        
        Serial.print("Threshold is "); Serial.print(Threshold,DEC);
        Serial.print(" Sensenum is "); Serial.print(SenseNum,DEC); Serial.write('\n');
        #endif
       }
    delay(DELAY);                             // arbitrary delay to limit data to serial port 
}



