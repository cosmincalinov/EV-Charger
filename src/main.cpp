#include <Arduino.h>

// led and button macros
#define RED_PIN 6
#define GREEN_PIN 5

#define LED1 7
#define LED2 8
#define LED3 9
#define LED4 10

#define BTN1 3
#define BTN2 2

int BTN2State = HIGH; // current reading from the input pin
int lastBTN2State = HIGH;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
bool stoppedCharging = 0; // flag that verifies if the chraging was stopped
unsigned long BTN2Start; // the time when BTN2 was pressed

// function that handles the case when BTN2 was pressed
void BTN2pressed() {
  // read the state of the switch into a local variable:
  BTN2State = digitalRead(BTN2);
  // If the switch changed, due to noise or pressing:
  if (BTN2State != lastBTN2State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    // if BTN2 was pressed 
    if(!BTN2State) {
      // save the time when it was pressed
      BTN2Start = millis();
    }
  }
  // update the stoppedCharging flag if the time from the last press was 1 second
  if(!BTN2State && millis() - BTN2Start > 1000) stoppedCharging = 1;

  // update the lastBTN2state variable
  lastBTN2State = BTN2State;
}

// function that does 3 synchronous blinks after a successful charge
void syncBlink() {
  for(int i = 0; i < 3; i++) {
    for(int j = LED1; j <= LED4; j++) digitalWrite(j, LOW);
    delay(600);

    for(int j = LED1; j <= LED4; j++) digitalWrite(j, HIGH);
    delay(600);
  }
    
  // disables all LEDs
  for(int i = LED1; i <= LED4; i ++) digitalWrite(i, LOW);

  // xhnages the state of the RGB LED to green
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  Serial.println("Incarcare completa!");
}

// function that treats the charging process
void charge() {
  // reinitialize the flag after every loop
  stoppedCharging = 0;

  // if BTN1 was pressed
  if(!digitalRead(BTN1)) {
    // change the state of the RGB LED to red
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    Serial.println("Incarcare...");

    // blink every LED, while also checking if the charging was stopped
    for(int i = LED1; (i <= LED4) && (stoppedCharging == 0); i++) {
      // blink a single LED 3 times
      for(int j = 0; (j <= 2) && (stoppedCharging == 0); j++){
        digitalWrite(i, HIGH);
        // check if BTN2  was pressed
        for(int k = 0; k < 6; k++){
          delay(100);
          BTN2pressed();
        }
        // keep the last LED ON
        digitalWrite(i, LOW);

        for(int k = 0; k < 6; k++){
          delay(100);
          BTN2pressed();
        }
      }
      // after charging was complete, turn off the LEDs
      digitalWrite(i, HIGH);
    }
    // if the charging wasn't stopped, do 3 synchronous blinks
    if(stoppedCharging == 0) syncBlink();
    // if the charging was stopped
    else {
      // disable all pins
      for(int i = LED1; i <= LED4; i++) digitalWrite(i, LOW);
      // change the state of the RGB LED to green
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);

      Serial.println("Incarcare oprita.");
    }
  }
}

void setup() {
  // set baud rate to 9600
  Serial.begin(9600);
  Serial.println("Doriti sa incarcati?");
  // set all pins connected to LEDs to the output mode
  for(int i = 4; i <= 10; i++) pinMode(i, OUTPUT);
  // set RGB LED state to green
  digitalWrite(GREEN_PIN, HIGH);
}

void loop() {
  charge();
}

