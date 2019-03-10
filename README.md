# CapSenseSerial

/*  Arduino Capacitive Sensor with serial interface for threshold setting   14/8/18   Bruce Woolmore
 * 11/3/19 WIP!  NEW COMMAND SYNTAX UNDER CONSTRUCTION
 * UNO option uses D5,D7 for soft serial, D4,D2 for cap sense send/sense pins, D6 for digital LOW alert, outputs debug info to std USB serial port.
 * Attiny85 uses D0,D2 (chip ins 5,7) for soft serial, D4,D3 (chip pins 3,2) for cap sense send/sense pins, D1 (chip pin 6) for digital LOW alert
 */
16/8/18 Command mode revised and extended
if 0x03 received on  RX line; set Threshold vlaue for triggering; decimal value of next char received *  10 = threshold for alert (sets alert pin, sends value to serial port)
if 0x02 received, send back decimal value of current Threshold  setting   
of 0x01 received, do 50 samples of  capacitance reading and send back mean of those values (for setting sensible Threshold)    
