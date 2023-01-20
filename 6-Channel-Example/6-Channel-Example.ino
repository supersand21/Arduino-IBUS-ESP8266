#include <SoftwareSerial.h>   // Hardware serial will most likely also work if all software serial references are swapped.

#define IBUS_BUFFSIZE 14      // Should be at least 2 + (IBUS_MAXCHANNELS * 2)
#define IBUS_MAXCHANNELS 6    // Number of channels to read

static uint8_t ibusIndex = 0;
static uint8_t ibus[IBUS_BUFFSIZE] = {0};
static uint16_t rcValue[IBUS_MAXCHANNELS];

SoftwareSerial ss(13, 15);    // Initialize software serial on arduino pins Rx: 13, Tx: 15
// IBUS signal line should be connected to the Rx pin. Tx pin can be ignored.

int a, b, c, d, e, f; // Some temporary variables
int ch1, ch2, ch3, ch4, ch5, ch6;  // place for channel values to be stored


void setup()
{
  ss.begin(115200);           // Begin software serial at IBUS protocol baud rate of 115200
  Serial.begin(19200);        // Begin hardware serial for printing values to the arduino monitor
}

void loop()
{
  readRx();   // Reads IBUS data from the rx pins
}


void readRx() {
  while (ss.available()) {
    uint8_t val = ss.read();
    
    // Look for 0x2040 as start of packet
    if (ibusIndex == 0 && val != 0x20) {
      ibusIndex = 0;
    }
    if (ibusIndex == 1 && val != 0x40) {
      ibusIndex = 0;
    }
    
    if (ibusIndex == IBUS_BUFFSIZE) { // If the start of a packet is found and ibus buffer is full, convert the array of bytes to thier channel values
      ibusIndex = 0;
      int high=3;
      int low=2;
      for(int i=0;i<IBUS_MAXCHANNELS; i++) {
        rcValue[i] = (ibus[high] << 8) + ibus[low];
        high += 2;
        low += 2;
      }
      a = rcValue[0];
      b = rcValue[1];
      c = rcValue[2];
      d = rcValue[3];
      e = rcValue[4];
      f = rcValue[5];
      
      // Perform value check to filter out corrupted batches
      if(a > 900 && a < 2100 && b > 900 && b < 2100 && c > 900 && c < 2100 && d > 900 && d < 2100 && e > 900 && e < 2100 && f > 900 && f < 2100) {
        ch1 = a;
        ch2 = b;
        ch3 = c;
        ch4 = d;
        ch5 = e;
        ch6 = f;
        Serial.print(ch1);
        Serial.print("       ");
        Serial.print(ch2);
        Serial.print("       ");
        Serial.print(ch3);
        Serial.print("       ");
        Serial.print(ch4);
        Serial.print("       ");
        Serial.print(ch5);
        Serial.print("       ");
        Serial.println(ch6);
      }else{
        Serial.println("ERR: channel corruption");
      }
      return;
    }
    else {  // If the ibus buffer is not full, save the current byte and increment the index.
      ibus[ibusIndex] = val;
      ibusIndex++;
    }
  }
}
