#include <Servo.h>

// Rotary Encoder Inputs
#define CLK0 2
#define DT0 3
#define SW0 6

// Rotary Encoder Inputs
#define CLK1 10
#define DT1 11
#define SW1 4

#define RED_LED 12
#define MODE_BUTTON 13

// the button for player 0
#define BUTTON0 8

// the button for player 1.
#define BUTTON1 7

#define SERVOPIN0 9
#define SERVOPIN1 5

int counter0 = 0;
int currentStateCLK0;
int lastStateCLK0;
String currentDir0 ="";
unsigned long lastButtonPress0 = 0;

int seconds0 = 0;
int servoPosition0 = 0;
Servo clockServo0;

int counter1 = 0;
int currentStateCLK1;
int lastStateCLK1;
String currentDir1 ="";
unsigned long lastButtonPress1 = 0;

int seconds1 = 0;
int servoPosition1 = 0;
Servo clockServo1;

// use state variables to debounce the player buttons.  Delays take too much time.
bool button0State;
bool button1State;

// track remaining time and elapsed time.
long remaining_millis0;
unsigned long previous_millis0;
unsigned long current_millis0; 

long remaining_millis1;
unsigned long previous_millis1;
unsigned long current_millis1;

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
	  currentStateCLK0 = digitalRead(CLK0);

	  // If last and current state of CLK are different, then pulse occurred
	  // React to only 1 state change to avoid double count
	  if (currentStateCLK0 != lastStateCLK0  && currentStateCLK0 == 1){

		  // If the DT state is different than the CLK state then
		  // the encoder is rotating CCW so decrement
		  if (digitalRead(DT0) != currentStateCLK0) {
		  	counter0 --;
			  currentDir0 ="CCW";
		  } else {
			  // Encoder is rotating CW so increment
			  counter0 ++;
			  currentDir0 ="CW";
		  }

		  Serial.print("Direction: ");
		  Serial.print(currentDir0);
		  Serial.print(" | Counter: ");
		  Serial.println(counter0);
	  }

	  // Remember last CLK state
	  lastStateCLK0 = currentStateCLK0;

  	// Read the button state
    int btnState0;
	  btnState0 = digitalRead(SW0);

	  //If we detect LOW signal, button is pressed
	  if (btnState0 == LOW) {
		//if 250ms have passed since last LOW pulse, it means that the
		//button has been pressed, released and pressed again
		if (millis() - lastButtonPress0 > 250) {
			Serial.println("Button pressed!");

      // reset counter
      counter0 = 0;
		}

		// Remember last button press event
		lastButtonPress0 = millis();
	}

	// Put in a slight delay to help debounce the reading
	delay(1);

  if (counter0 > 20) {
    counter0 = 20;
  } else if (counter0 < 0) {
    counter0 = 0;
  }

  // map count to seconds.
  seconds0 = map(counter0, 0, 20, 0, 300);
//  Serial.print("(adjust) seconds = ");
//  Serial.println(seconds0);
  delay(1);

  // keep track of how much time is remaining (in msec).
  remaining_millis0 = 1000 * (unsigned long)seconds0;

  // debug
  remaining_millis1 = remaining_millis0;
  //Serial.print("(adjust) remaining_millis0 = ");
  //Serial.println(remaining_millis0);

  // map seconds to servo position.
  servoPosition0 = map(seconds0, 0, 300, 0, 180);
  servoPosition0 = 180 - servoPosition0;
  clockServo0.write(servoPosition0);
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
    }
    
    // toggle player and set previous time.
    currentPlayer = 1;
    previous_millis1 = millis();

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
    
    // toggle player and set previous time.
    currentPlayer = 0;
    previous_millis0 = millis();

  } else if (digitalRead(BUTTON1) == HIGH && button1State == true) {
    // button 1 was just released.
    button1State = false;
  }
}

void updateRemainingTime() {
  unsigned long delta;
  if (currentMode == MODE_PLAY) {
    if(currentPlayer == 0) {
      //update remaining time for player 0.
      current_millis0 = millis();
      delta = current_millis0 - previous_millis0;
      remaining_millis0 = remaining_millis0 - delta;
      if (remaining_millis0 < 0) {
        remaining_millis0 = 0;
      }
      previous_millis0 = current_millis0;
    } else if (currentPlayer == 1) {
      //update remaining time for player 1.
      current_millis1 = millis();
      delta = current_millis1 - previous_millis1;
      remaining_millis1 = remaining_millis1 - delta;
      if (remaining_millis1 < 0) {
        remaining_millis1 = 0;
      }
      previous_millis1 = current_millis1;
    }
  }
}

void checkAdjustClocks() {
  // only update clock through encoders if in MODE_SET.
  // if you update the clock, also update the time remaining variable.
  if (currentMode == MODE_SET) {
    adjustClocks();
  }
}

void updateDials() {
  if (currentMode == MODE_PLAY) {
    // update current player dial.
    if (currentPlayer == 0) {
      // map count to seconds.
      seconds0 = remaining_millis0 /1000;

      Serial.print("seconds (0) = ");
      Serial.println(seconds0);

      // map seconds to servo position.
      servoPosition0 = map(seconds0, 0, 300, 0, 180);
      servoPosition0 = 180 - servoPosition0;
      clockServo0.write(servoPosition0);
    } else if (currentPlayer == 1) {
      // map count to seconds.
      seconds1 = remaining_millis1 /1000;

      Serial.print("seconds (1) = ");
      Serial.println(seconds1);

      // map seconds to servo position. 
    }
  // delay a little.
  delay(100);
  }
}

void setup() {
	
	// Set encoder pins as inputs
	pinMode(CLK0,INPUT);
	pinMode(DT0,INPUT);
	pinMode(SW0, INPUT_PULLUP);

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
	lastStateCLK0 = digitalRead(CLK0);

  clockServo0.attach(SERVOPIN0);

  currentMode = MODE_SET;
  currentPlayer = 0;
}

void loop() {
  checkModeSetButton();
  refreshModeLED();
  
  // update time remaining.
  // do this before checking the player buttons.
  updateRemainingTime();
  checkPlayerButtons();

  // update the dials.
  updateDials();
  checkAdjustClocks();
}
