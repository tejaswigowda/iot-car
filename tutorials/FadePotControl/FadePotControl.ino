/*
  Fading

  This example shows how to fade an LED using the analogWrite() function.

  The circuit:
  - LED attached from digital pin 9 to ground.

  created 1 Nov 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fading
*/
int potPin = A0;
int ledPin = 13;    // LED connected to digital pin 9
int potVal = 0;

void setup() {
  // nothing happens in setup
}

void loop() {
  potVal = analogRead(potPin);
  int pwmVal = map(potVal, 0, 1024, 0, 255);
    analogWrite(ledPin, pwmVal); // 100%
    delay(10); 
  }

  
