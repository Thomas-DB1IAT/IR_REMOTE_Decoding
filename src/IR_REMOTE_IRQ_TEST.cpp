/*

  IR Remote Control Decoder
  DB1IAT

  Remotes using different coding and Bitlength
  This is just a Set of Decoding Routines for the 
  Remotes i found here around

  Some Remotes sending for a Repeat the complete Code again
  and some just sending a addidional Start Condition ( 4 Steps long)

  How it works:
  There is one Interrupt Routine that will be triggert on each
  Change of the IRQ_PIN. MyRun will be Set to true on the first Change of the IRQ_PIN
  The Time Information Microseconds minus the oMicros will be stored in the IrqData Array
  so we may use that information to decode the Bits.
  Those Bits following the Start Condition in small Time Steps so if there is for at least
  15 mS no Change at the IRQ_PIN the Frame of Data Ends and we reset the MyRun Variable to false

  Now we have the Data of one Frame in the Array IrqData and the StepCounter contains the Value
  of Steps in the Frame. Whit this Information we are able to decode the Frame. Before we starting 
  the Decoding what of course takes some Time we get a Copy from the IrqData Array to the Data Array
  and from StepCount to MyStepCount. So the IRQ Service Routine may Use this Memory again, even while
  we still docoding the Frame.

  
  Remotes tested with:
  
  PRO VISION DVD
  NEC RU-M111
  RG405-DS4
  TELEKOM MAGENTA TV USB-Stick Remote
  SAMSUNG BN59-01247A
  Sony Audio
  

*/

#include <Arduino.h>

unsigned long oMicros = 0;
unsigned long IrqData[100];
unsigned long Data[100];

volatile bool MyRun = false;
volatile int StepCount = 0;
volatile int Changed = 0;

int MyStepCount = 0;

const int IRQ_PIN = 14;

void IR_ISR(void) {
  //Changed = 1;    
  IrqData[StepCount] = micros()-oMicros;
  StepCount ++;
  if (StepCount > 198) StepCount = 0;
  if (!MyRun) {
    MyRun = true;
  }   
  oMicros = micros();  
}

void setup() {
  Serial.begin(250000);
  while (!Serial);
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), IR_ISR, CHANGE);  
  //pinMode(IRQ_PIN, INPUT_PULLUP);
}

byte   BitNum = 0;

// Expected Pulse Length 500 uS +-20% max
// Min Pulse length for a 0 Bit
int tMin = 400;
// Max Pulse length for a 0 Bit
int tMax = 700;


void Do26Steps(void) {
  byte Byte1  = 0;
  byte Byte2  = 0;
  byte Byte3  = 0;
  byte Byte4  = 0;
  if ((Data[1] > 2200) && (Data[1] < 2700)) {
    Serial.print("S ");
    // Start Condition
    int i = 2;
    // Get First Byte
    while (i < 14) {
      if ((Data[i] > tMin) && (Data[i] < tMax)) {
        if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
          // clear the Bit
          Byte1 &= ~(1 << BitNum);
        } else {Byte1 |= (1 << BitNum);}
      }
      i+=2;
      BitNum ++;
    }
    i = 16;
    BitNum = 0;
    while (i < 26) {
      if ((Data[i] > tMin) && (Data[i] < tMax)) {
        if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
          // clear the Bit
          Byte2 &= ~(1 << BitNum);
        } else {Byte2 |= (1 << BitNum);}
      }
      i+=2;
      BitNum ++;
    }
    if (Byte1 < 16) Serial.print("0");
    Serial.print(Byte1, HEX); Serial.print(" ");
    if (Byte2 < 16) Serial.print("0");
    Serial.println(Byte2, HEX);
  }  
}  

void Do38Steps(void) {
  byte Byte1  = 0;
  byte Byte2  = 0;
  byte Byte3  = 0;
  byte Byte4  = 0;
  if ((Data[1] > 7900) && (Data[1] < 8100)) {
    Serial.print("S ");
    if ((Data[2] > 3900) && (Data[2] < 4100)) {
      //Serial.print("B1 ");
      // Start Condition
      if ((Data[20] > 3900) && (Data[20] < 4100)) {
        //Serial.print("B2 ");
        // Second Byte Condition OK
        // Get First Byte
        int i = 3;
        while (i < 19) {
          if ((Data[i] > tMin) && (Data[i] < tMax)) {
            if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
              // clear the Bit
              Byte1 &= ~(1 << BitNum);
            } else {Byte1 |= (1 << BitNum);}
          }
          i+=2;
          BitNum ++;
        }
        i = 21;
        BitNum = 0;
        while (i <= 38) {
          if ((Data[i] > tMin) && (Data[i] < tMax)) {
            if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
              // clear the Bit
              Byte2 &= ~(1 << BitNum);
            } else {Byte2 |= (1 << BitNum);}
          }
          i+=2;
          BitNum ++;
        }
        if (Byte1 < 16) Serial.print("0");
        Serial.print(Byte1, HEX); Serial.print(" ");
        if (Byte2 < 16) Serial.print("0");
        Serial.println(Byte2, HEX);
      }
    }
  }
} 

void Do42Steps(void) {
  byte Byte1  = 0;
  byte Byte2  = 0;
  byte Byte3  = 0;
  byte Byte4  = 0;
  if ((Data[1] > 2200) && (Data[1] < 2700)) {
    Serial.print("S ");
    // Start Condition
    int i = 2;
    // Get First Byte
    while (i < 16) {
      if ((Data[i] > tMin) && (Data[i] < tMax)) {
        if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
          // clear the Bit
          Byte1 &= ~(1 << BitNum);
        } else {Byte1 |= (1 << BitNum);}
      }
      i+=2;
      BitNum ++;
    }
    i = 18;
    BitNum = 0;
    while (i < 26) {
      if ((Data[i] > tMin) && (Data[i] < tMax)) {
        if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
          // clear the Bit
          Byte2 &= ~(1 << BitNum);
        } else {Byte2 |= (1 << BitNum);}
      }
      i+=2;
      BitNum ++;
    }
    i = 34;
    BitNum = 0;
    while (i < 42) {
      if ((Data[i] > tMin) && (Data[i] < tMax)) {
        if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
          // clear the Bit
          Byte3 &= ~(1 << BitNum);
        } else {Byte3 |= (1 << BitNum);}
      }
      i+=2;
      BitNum ++;
    }
    if (Byte1 < 16) Serial.print("0");
    Serial.print(Byte1, HEX); Serial.print(" ");
    if (Byte2 < 16) Serial.print("0");
    Serial.print(Byte2, HEX); Serial.print(" ");
    if (Byte3 < 16) Serial.print("0");
    Serial.println(Byte3, HEX);
  }  
}  

void Do68Steps(void) {
  byte Byte1  = 0;
  byte Byte2  = 0;
  byte Byte3  = 0;
  byte Byte4  = 0;
  if ((Data[1] > 4500) && (Data[1] < 9700)) {
    Serial.print("S ");
    if ((Data[2] > 4400) && (Data[2] < 4600)) {
      //Serial.print("B1 ");
      // Start Condition
      int i = 3;
      // Get First Byte
      while (i < 19) {
        if ((Data[i] > tMin) && (Data[i] < tMax)) {
          if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
            // clear the Bit
            Byte1 &= ~(1 << BitNum);
          } else {Byte1 |= (1 << BitNum);}
        }
        i+=2;
        BitNum ++;
      }
      i = 19;
      BitNum = 0;
      while (i < 35) {
        if ((Data[i] > tMin) && (Data[i] < tMax)) {
          if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
            // clear the Bit
            Byte2 &= ~(1 << BitNum);
          } else {Byte2 |= (1 << BitNum);}
        }
        i+=2;
        BitNum ++;
      }
      i = 35;
      BitNum = 0;
      while (i < 51) {
        if ((Data[i] > tMin) && (Data[i] < tMax)) {
          if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
            // clear the Bit
            Byte3 &= ~(1 << BitNum);
          } else {Byte3 |= (1 << BitNum);}
        }
        i+=2;
        BitNum ++;
      }
      i = 51;
      BitNum = 0;
      while (i < 67) {
        if ((Data[i] > tMin) && (Data[i] < tMax)) {
          if ((Data[i+1] > tMin) && (Data[i+1] < tMax)) {
            // clear the Bit
            Byte4 &= ~(1 << BitNum);
          } else {Byte4 |= (1 << BitNum);}
        }
        i+=2;
        BitNum ++;
      }
      if (Byte1 < 16) Serial.print("0");
      Serial.print(Byte1, HEX); Serial.print(" ");
      if (Byte2 < 16) Serial.print("0");
      Serial.print(Byte2, HEX); Serial.print(" ");
      if (Byte3 < 16) Serial.print("0");
      Serial.print(Byte3, HEX); Serial.print(" ");
      if (Byte4 < 16) Serial.print("0");
      Serial.println(Byte4, HEX);
    }
  }  
} 

void loop() {
  // Check for end of Transmission
  // If we are in MyRun Mode and 15mS nothing Changed
  if ((micros() >= oMicros +15000) && (MyRun)) {
    memcpy(Data, IrqData, sizeof(IrqData));
    // Nomore Signal Changes
    // Save BitCount
    MyStepCount = StepCount;
    // Clear BitCount
    StepCount = 0;
    MyRun = false;
    Serial.print("StepCnt ");
    Serial.print(MyStepCount);
    Serial.print(" - ");
    BitNum = 0;
    
    switch (MyStepCount) {
      case 4:
        // REPEAT
        Serial.println("Repeat");
        break;
      case 26:
        // Sony
        Do26Steps();
      break;
      case 38:
        // NEC RU-M111
        Do38Steps();
      break;
      case 42:
        // Sony
        Do42Steps();
      break;
      case 68:
        // SAMSUNG BN59-01247A
        Do68Steps();
      break;
      default:
        // Print Timing Values
        Serial.println("");
        int BitNum = 0;
        int i = 0;
        while (i < MyStepCount) {
          Serial.print(i); Serial.print(" "); Serial.print(BitNum); Serial.print(" "); Serial.print(Data[i]); Serial.print(" "); Serial.println(Data[i+1]);
          i+=2;
          BitNum++;
          if (BitNum == 8) BitNum = 0;
        }
      break;    
    }  
  }
}
