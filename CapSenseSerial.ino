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
#define DELAY 200    // delay in millisecs between cap tests
#define CapSendPin 4    // D4 used for both Uno and Attiny85
#ifdef UNO
#define CapSensePin 2   // used 2 for Uno, 3 for Attiny85
#define ALERT 6  // digital out pin for threshold alert  (UNO D6)
#else
#define CapSensePin 3   // used 3 for Attiny85
#define ALERT 1  // digital out pin for threshold alert (D1, chip pin 6) 
#endif
#define SensorRate 9600
CapacitiveSensor   cs = CapacitiveSensor(CapSendPin, CapSensePin);        // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire and or foil if desired
SoftwareSerial Sercom(RX, TX);

bool editMode=false;
bool dbgMode=false;
unsigned int Threshold=500; 
int Inbyte;
unsigned int tval;
char buffer[24];

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
    sprintf(buffer,"%d \n",N);
    return buffer;
  }
char* xprint(int N)
  {
    sprintf(buffer,"%x \n",N);
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
        // diag("char read:"); diag(xprint(Inbyte));
        if(Inbyte>96)
          {
           editMode=false; 
           int testchar=Inbyte^0x60;
           // diag("casetest=");  diag(xprint(testchar));
           switch (testchar)
              {
               case 3:{
                 diag("cmd=c\n");
                 Threshold=Calibrate();
                 diag(nprint(Threshold));
                 break; }
               case 4:
                 diag("cmd=d\n");
                 if(!dbgMode) 
                    {dbgMode=true; diag("debug mode ON");}
                 else {diag("debug mode OFF"); dbgMode=false;}  
                 break; 
               case 17:{
                 diag("cmd=q\n");
                 diag(nprint(Threshold));
                 break; }  
               break;
               }   // end switch 
           }     // end I>96
        else  //  Inbyte .LE. 96
           {
           if(Inbyte>47)  
           // hex values 30-3f = digits so assume entering new threshold
               {
                int testchar=Inbyte^0x30;
                diag("digit=");
                diag(xprint(testchar));
                if(editMode) 
                   {tval=tval*10+testchar;}
                else
                   {editMode=true;tval=testchar; }
                diag("tval:");
                diag(nprint(tval));   
               }
            else // inbyte <47 invisible control chars
              {
                 if(Inbyte==13 && editMode)
                   {
                   diag("change Threshold:");
                   Threshold=tval;
                   diag(nprint(Threshold));
                   } 
                 editMode=false;    
               }
           }    // end Inbyte .LE. 96                            
        }              // end while serial
    long SenseNum =  cs.capacitiveSensor(30);
    if(SenseNum>Threshold)
       {
        diag(nprint(SenseNum));                  // print sensor output 
        digitalWrite(ALERT,LOW);
        delay(20);
        digitalWrite(ALERT,HIGH);
        
       }
    delay(DELAY);                             // arbitrary delay to limit data to serial port 
}

