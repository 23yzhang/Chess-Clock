#include <Servo.h>

// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4

#define RED_LED 12
#define MODE_BUTTON 13

// the button for player 0
#define BUTTON_0 8

// the button for player 1.
#define BUTTON_1 7

#define SERVOPIN 9

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

int seconds = 0;
int servoPosition = 0;
Servo clockServo;

// this enumeration is used to represent the mode of the system.
enum SystemMode {
  MODE_SET,
  MODE_READY,
  MODE_PLAY
};

// 0 if player 0, 1 if player 1. 
unsigned char currPlayer = 0;

// this variable keeps track of the current system mode.
SystemMode currentMode;

void setup() {
	
	// Set encoder pins as inputs
	pinMode(CLK,INPUT);
	pinMode(DT,INPUT);
	pinMode(SW, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(MODE_BUTTON, INPUT_PULLUP);

	// Setup Serial Monitor
	Serial.begin(115200);

	// Read the initial state of CLK
	lastStateCLK = digitalRead(CLK);

  clockServo.attach(SERVOPIN);

  currentMode = MODE_SET;
}

void loop() {

  // refresh LED.
  if (currentMode == MODE_SET) {
    // turn on mode LED.
    digitalWrite(RED_LED, HIGH);
  } else {
    // turn off mode LED.
    digitalWrite(RED_LED, LOW);
  }

  // check mode button.
  int modeVal = !digitalRead(MODE_BUTTON);
  if (modeVal == 1) {
    if (currentMode == MODE_SET) {
      currentMode = MODE_READY;
    } else {
      currentMode = MODE_SET;
    }
    // delay to allow user time to release button.
    delay(250);
  }
	
  // only update clock through encoders if in MODE_SET.
  if (currentMode == MODE_SET) {
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
	
}