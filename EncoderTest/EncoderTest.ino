#include <Servo.h>

// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4

#define SERVOPIN 9

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

int seconds = 0;
int servoPosition = 0;
Servo clockServo;

void setup() {
	
	// Set encoder pins as inputs
	pinMode(CLK,INPUT);
	pinMode(DT,INPUT);
	pinMode(SW, INPUT_PULLUP);

	// Setup Serial Monitor
	Serial.begin(115200);

	// Read the initial state of CLK
	lastStateCLK = digitalRead(CLK);

  clockServo.attach(SERVOPIN);
}

void loop() {
	
	// Read the current state of CLK
	currentStateCLK = digitalRead(CLK);

	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DT) != currentStateCLK) {
			counter --;
			currentDir ="CCW";
		} else {
			// Encoder is rotating CW so increment
			counter ++;
			currentDir ="CW";
		}

		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(counter);
	}

	// Remember last CLK state
	lastStateCLK = currentStateCLK;

	// Read the button state
	int btnState = digitalRead(SW);

	//If we detect LOW signal, button is pressed
	if (btnState == LOW) {
		//if 50ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 50) {
			Serial.println("Button pressed!");

      // reset counter
      counter = 0;
		}

		// Remember last button press event
		lastButtonPress = millis();
	}

	// Put in a slight delay to help debounce the reading
	delay(1);


  if (counter > 20) {
    counter = 20;
  } else if (counter < 0) {
    counter = 0;
  }

  // map count to seconds.
  seconds = map(counter, 0, 20, 0, 300);
  Serial.print("seconds = ");
  Serial.println(seconds);

  // map seconds to servo position.
  servoPosition = map(seconds, 0, 300, 0, 180);
  servoPosition = 180 - servoPosition;
  clockServo.write(servoPosition);

}