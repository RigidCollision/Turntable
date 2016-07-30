////////////////////////////////////////////////////////////////////////////////
//
// DCC Turntable Control Test Routines

#include <DCC_Decoder.h>
#include <AccelStepper.h>
#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////
// Defines and structures
//
#define kDCC_INTERRUPT 0

typedef struct
{
  int address; 
}

DCCAccessoryAddress;
DCCAccessoryAddress gAddresses[4];

////////////////////////////////////////////////////////////////////////////////
// Now we'll wrap the stepper in an AccelStepper object

AccelStepper stepper1(1,4,5);

volatile boolean stopNow = false; // flag for Interrupt Service routine
boolean homed = false; // flag to indicate when motor is homed
int stepCount = 33152; // Move this many steps; 33152 = 1 turn

////////////////////////////////////////////////////////////////////////////////
// Decoder Init 

void ConfigureDecoder()
{
  gAddresses[0].address = 200;
  gAddresses[1].address = 201;
  gAddresses[2].address = 202;
  gAddresses[3].address = 203;
}

////////////////////////////////////////////////////////////////////////////////
// Basic accessory packet handler 

void BasicAccDecoderPacket_Handler(int address,boolean activate,byte data)
{
  // Convert NMRA packet address format to human address
  address -= 1;
  address *= 4;
  address += 1;
  address += (data & 0x06) >> 1;
  boolean enable = (data & 0x01) ? 1 : 0;
  for (int i=0; i < (int) (sizeof(gAddresses) / sizeof(gAddresses[0])); i++)
  {
    if (address == gAddresses[i].address)
    {
      Serial.print("Basic addr: ");
      Serial.print(address,DEC);
      Serial.print("   activate: ");
      Serial.println(enable,DEC);
      if (enable)
      {
        switch (i)
        {
          case 0:
            stepper1.moveTo(0);
            break;
          case 1:
            stepper1.moveTo(7498);
            break;
           case 2:
            stepper1.moveTo(5498);
            break;
          case 3:
            stepper1.moveTo(60);
            break;
         }
      }
      else
      {
        switch (i)
        {
          case 0:
            stepper1.moveTo(0);
            break;
           case 1:
            stepper1.moveTo(7498);
            break;
          case 2:
            stepper1.moveTo(5498);
            break;
          case 3:
            stepper1.moveTo(160);
            break;
         }
      }
    }
  }
}


void setup()
{
////////////////////////////////////////////////////////////////////////////////
//configure Inturrupt pin 1 (digital pin3) as an input and enable the internal pull-up resistor

  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), intService, LOW);

  Serial.begin(9600);
  
////////////////////////////////////////////////////////////////////////////////
// Set AccelStepper defaults

  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(500);
  stepper1.setSpeed(1000);
  stepper1.setBacklash(34);
  stepper1.move(stepCount);
}

void loop()
{
  if (stopNow)
    {
      
////////////////////////////////////////////////////////////////////////////////
// We found 'home', best detach the interrupt, set our position to a value that 
// when we move to '0' will park the turntable at our first exit.

    detachInterrupt(digitalPinToInterrupt(3));

    stepper1.setCurrentPosition(-6450);
    stopNow = false; // Prevents repeated execution of the above code  
    homed = true;
    stepper1.moveTo(0);
    }
    
  if(homed)
    {
      
////////////////////////////////////////////////////////////////////////////////
// Put any code in here that only runs once the turntable is homed
      static int addr = 0;
      
      DCC.loop();
        
      if( ++addr >= (int)(sizeof(gAddresses)/sizeof(gAddresses[0])) )
      {
        addr = 0;
      }
      DCC.SetBasicAccessoryDecoderPacketHandler(BasicAccDecoderPacket_Handler,true);
      ConfigureDecoder();
      DCC.SetupDecoder(0x00, 0x00, kDCC_INTERRUPT);   
    }
  
  stepper1.run();  
}

////////////////////////////////////////////////////////////////////////////////
// Interrupt service routine for INT 1

void intService()  
{
  stopNow = true; // Set flag to show Interrupt recognised and then stop the motor  
}

