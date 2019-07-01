# CapSenseSerial

/*  Arduino Capacitive Sensor with serial interface for threshold setting   14/8/18   Bruce Woolmore
 * 11/3/19 v2  NEW COMMAND SYNTAX
 * UNO option uses D5,D7 for soft serial, D4,D2 for cap sense send/sense pins, D6 for digital LOW alert, accepts config commands and outputs debug info to serial port.
 * Attiny85 uses D0,D2 (chip ins 5,7) for soft serial, D4,D3 (chip pins 3,2) for cap sense send/sense pins, D1 (chip pin 6) for digital LOW alert
 */
16/3/19 config/debug commands revised:
if digits 0-9 followed by <CR> (x13) received on  RX line; set Threshold value to that number e.g. 567<CR> will set threshold to 567; ie capacitance value over 
 567 will trigger alert line LOW
 sending "q" (query) to the serrial port will return the current threshold value
 sending "d" sets debug mode, diagnostics output to serial port 
sending "c" activates a calibrate mode;  the  sensor is monitored for 50 reads, and a value 10% above the maximum sense value  read is used to set  the threshold value (the default value at startup is 500)

Test configuration for  Attiny85 was  3v3  supply, std (default)  1 mhz clock