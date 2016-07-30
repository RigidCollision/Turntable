
// Speed test designed to run a stepper at various speeds to find the exact speed at which it runs with no harmonics or vibration
// Use the monitor to see and record the current speed.

// defines pins numbers

const int stepPin = 4; 
const int dirPin = 5; 

int stepCount  = 6500;
int delayCount = 0;

void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  
  Serial.begin(9600);
}
void loop() {
  for(int x =300; x < 1200; x++) {
    delayCount = x;
    
    digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction

    Serial.println(delayCount);
     
    for(int i = 0; i < stepCount; i++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(delayCount); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(delayCount); 
    }
    delay(1000); // One second delay
  }
}
