#include <Servo.h>

// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4

#define RED_LED 12
#define MODE_BUTTON 13

// the button for player 0
#define BUTTON0 8

// the button for player 1.
#define BUTTON1 7

#define SERVOPIN 9

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;

int seconds = 0;
int servoPosition = 0;
Servo clockServo;

// use state variables to debounce the player buttons.  Delays take too much time.
bool button0State;
bool button1State;

// track remaining time and elapsed time.
unsigned long remainingmillis0;
unsigned long startmillis0;
unsigned long currengmillis0; 

// this enumeration is used to represent the mode of the system.
enum SystemMode {
  MODE_SET,
  MODE_READY,
  MODE_PLAY
};

// 0 if player 0, 1 if player 1. 
unsigned char currentPlayer = 0;

// this variable keeps track of the current system mode.
SystemMode currentMode;
SystemMode previousMode;

void checkModeSetButton() {
  // check mode button.
  int modeVal;
  modeVal = !digitalRead(MODE_BUTTON);
  if (modeVal == 1) {
    // button is pressed, toggle the mode between set and ready.
    // first set previous mode.
    previousMode = currentMode;
    if (currentMode == MODE_SET) {
      currentMode = MODE_READY;
    } else {
      currentMode = MODE_SET;
    }
    // delay to allow user time to release button.
    delay(250);
  }
}

void refreshModeLED() {
    // refresh LED.
  if (currentMode == MODE_SET) {
    // turn on mode LED.
    digitalWrite(RED_LED, HIGH);
  } else {
    // turn off mode LED.
    digitalWrite(RED_LED, LOW);
  }
}

void adjustClocks() {
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
    int btnState;
	  btnState = digitalRead(SW);

	  //If we detect LOW signal, button is pressed
	  if (btnState == LOW) {
		//if 250ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress > 250) {
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

  // keep track of how much time is remaining (in msec).
  remainingmillis0 = 1000 * seconds;

  // map seconds to servo position.
  servoPosition = map(seconds, 0, 300, 0, 180);
  servoPosition = 180 - servoPosition;
  clockServo.write(servoPosition);
}

void checkPlayerButtons() {
  // check player buttons.
  if (digitalRead(BUTTON0) == LOW && button0State == false) {
    // button 0 was just pressed.
    button0State = true;

    // are we in ready mode?
    if (currentMode == MODE_READY) {
      // switch to play mode.
      previousMode = currentMode;
      currentMode = MODE_PLAY;
      // set initial time values.
    }

    // toggle player.
    currentPlayer = 1;

  } else if (digitalRead(BUTTON0) == HIGH && button0State == true) {
    // button 0 was just released.
    button0State = false;
  }

  if (digitalRead(BUTTON1) == LOW && button1State == false) {
    // button 1 was just pressed.
    button1State = true;

    // are we in ready mode?
    if (currentMode == MODE_READY) {
      // switch to play mode.
      previousMode = currentMode;
      currentMode = MODE_PLAY;
    }

    // toggle player.
    currentPlayer = 0;

    // update time

    // record start time of current player.
    startmillis0 = millis();


  } else if (digitalRead(BUTTON1) == HIGH && button1State == true) {
    // button 1 was just released.
    button1State = false;
  }

}

void checkAdjustClocks() {
  // only update clock through encoders if in MODE_SET.
  // if you update the clock, also update the time remaining variable.
  if (currentMode == MODE_SET) {
    adjustClocks();
  }
}

void setup() {
	
	// Set encoder pins as inputs
	pinMode(CLK,INPUT);
	pinMode(DT,INPUT);
	pinMode(SW, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);

  // assume buttons are initially unpressed.
  button0State = false;
  button1State = false;

	// Setup Serial Monitor
	Serial.begin(115200);

	// Read the initial state of CLK
	lastStateCLK = digitalRead(CLK);

  clockServo.attach(SERVOPIN);

  currentMode = MODE_SET;
}

void loop() {
  checkModeSetButton();
  refreshModeLED();
  checkPlayerButtons();

  // update time remaining.

  checkAdjustClocks();

}