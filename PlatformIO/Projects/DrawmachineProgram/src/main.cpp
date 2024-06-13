#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// set MotorDriver Pins
#define M1_STEP   27
#define M1_DIR    14
#define M2_STEP   17
#define M2_DIR    16
#define M3_STEP   26
#define M3_DIR    25

// make counter volatile for use in interrupt
volatile uint32_t counter = 0;

// set the LCD number of columns and rows
#define lcdColumns  20
#define lcdRows     4
#define lcdAdress   0x27

// set rotary encoder Pins
#define ENCODER_CLK 33
#define ENCODER_DT  35
#define ENCODER_SW  32

// Make option volatile for use in interrupt
volatile int option = 0;
volatile int prevOption = 0;

volatile bool runMotors = false;
volatile int currentPattern = 0;

int motorCounters[3][3] = {
  {200, 100, 150},
  {500, 400, 300},
  {150, 200, 400}
};

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // set the LCD address to 0x27 for a 20 chars and 4 line display

void stepMotor(int motorNumber, bool motorFlag) {
  if (motorFlag) {
    if (motorNumber == 1) {
      digitalWrite(M1_STEP, !digitalRead(M1_STEP));
    } else if (motorNumber == 2) {
      digitalWrite(M2_STEP, !digitalRead(M2_STEP));
    } else if (motorNumber == 3) {
      digitalWrite(M3_STEP, !digitalRead(M3_STEP));
    }
  }
}

void IRAM_ATTR onTimer() {
  counter++;
  if (counter % motorCounters[currentPattern][0] == 0) {
    stepMotor(1, runMotors); // Step motor M1
  } else if (counter % motorCounters[currentPattern][1] == 0) {
    stepMotor(2, runMotors); // Step motor M2
  } else if (counter % motorCounters[currentPattern][2] == 0) {
    stepMotor(3, runMotors); // Step motor M3
  }
  if(counter == motorCounters[currentPattern][2]*75) {
    counter = 0;
    runMotors = false;
  }
}

void readEncoder() {
  //static unsigned long lastInterruptTime = 0;
  //unsigned long interruptTime = millis();
  
  // Debouncing mechanism to ignore rapid changes
  //if (interruptTime - lastInterruptTime > 50) {
   int dtValue = digitalRead(ENCODER_DT);
    if (dtValue == HIGH) {
      option++;     // Clockwise
    }
    if (dtValue == LOW) {
      option--;     // Counterclockwise
    }
    if (option == 5) {
      option = 0; // Set option to 0 if it reaches 5
    }
    if (option == -1) {
      option = 4; // Set option to 4 if it reaches -1
    }
  //}
  //lastInterruptTime = interruptTime;
}

int getOption() {
  noInterrupts();
  int result = option;
  interrupts();
  return result;
}

void handleoption() {
  noInterrupts();
  int currentOption = getOption();
  if (currentOption == 3) {
    runMotors = true;
  } else if (currentOption == 4) {
    runMotors = false;
  } else {
    currentPattern = currentOption; 
  }
  interrupts();
}

void setup(){
  // initialize the LCD
  lcd.init();

  // initialize the rotary encoder
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_SW), handleoption, CHANGE);

  // initialize the MotorDriver
  pinMode(M1_STEP, OUTPUT);
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_STEP, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M3_STEP, OUTPUT);
  pinMode(M3_DIR, OUTPUT);

  // initialize the Timer
  hw_timer_t * timer = NULL;
  timer = timerBegin(1, 80, true); // 80 prescaler
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true); // 1 second interval
  timerAlarmEnable(timer);
}

void printMessage(int option) {
  if (option != prevOption) {
    lcd.clear();
  }
  
  if (option == 0) {
    lcd.print("<P1>   P2    P3  ");
  } 
  else if (option == 1) {
    lcd.print(" P1   <P2>   P3  ");
  } 
  else if (option == 2) {
    lcd.print(" P1    P2   <P3> ");
  }
  else {
    lcd.print(" P1    P2    P3 ");
  }

  lcd.setCursor(0, 1);
  if (option == 3) {
    lcd.print(" <start>  stop ");
  }
  else if (option == 4) {
    lcd.print("  start  <stop>");
  }
  else {
    lcd.print("  start   stop ");
  }

  lcd.setCursor(0, 2);
  lcd.print(getOption() == 0 ? "Pattern 1" : getOption() == 1 ? "Pattern 2" : "Pattern 3");
  
  lcd.setCursor(0, 3);
  lcd.print("Counter: " + String(getOption()));
  prevOption = option;
}

void loop(){
  printMessage(getOption());
}