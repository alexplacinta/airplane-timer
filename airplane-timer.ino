#include <EnableInterrupt.h>
#include <Servo.h>
#include <TimedAction.h>
#include <TM1637Display.h>

#define servoPin 4
#define displayCLK 5
#define displayDIO 6
#define rotaryEncA 8
#define rotaryEncB 7
#define buttonPin 9
#define triggerPin 2

Servo myservo;
TM1637Display display = TM1637Display(displayCLK, displayDIO);

int minTime = 5;
int maxTime = 180;
int stepTime = 5;
volatile int totalTime = 5;
volatile int runningTime = totalTime;
volatile bool isRunning = false;
int triggerState;
int triggerLastState;

void tick() {
  runningTime--;
  if (runningTime == 0) {
    isRunning = false;
    display.showNumberDec(totalTime);
    Serial.print("Timer: ");
    Serial.println(totalTime);
  } else {
    display.showNumberDec(runningTime);
    Serial.print("Timer: ");
    Serial.println(runningTime);
  }

}

TimedAction timer = TimedAction(1000, tick);

void rotaryChange() {
  if (isRunning) {
    return;
  }
  
  if (digitalRead(rotaryEncB) == HIGH) {
    if (totalTime < maxTime) {
      totalTime += stepTime;
    }

  } else {
    if (totalTime > minTime) {
      totalTime -= stepTime;
    }
  }

  display.showNumberDec(totalTime);
  Serial.print("Timer: ");
  Serial.println(totalTime);
}

void buttonChange() {
  isRunning = false;
  runningTime = totalTime;
  display.showNumberDec(totalTime);
  Serial.println("Reset");
}

void triggerChange() {
  //  isRunning = false;
  //  runningTime = totalTime;
  //  Serial.print("TRIGGER: ");
  //  Serial.println(digitalRead(triggerPin));
}

void setup() {
  pinMode(rotaryEncA, INPUT);
  pinMode(rotaryEncB, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(triggerPin, INPUT_PULLUP);
  //    myservo.attach(servoPin);

  Serial.begin(9600);
  enableInterrupt(rotaryEncA, rotaryChange, RISING);
  enableInterrupt(buttonPin, buttonChange, FALLING);
  //  enableInterrupt(triggerPin, triggerChange, CHANGE);
  triggerLastState = digitalRead(triggerPin);
  display.setBrightness(10);
  display.showNumberDec(totalTime);
}

void loop() {
  triggerState = digitalRead(triggerPin);

  if (triggerState != triggerLastState) {
    if (triggerState == LOW) {
      Serial.println("Trigger on");
      isRunning = false;
      runningTime = totalTime;
      display.showNumberDec(totalTime);
    } else {
      Serial.println("Trigger released");
      isRunning = true;
      runningTime = totalTime;
      timer.reset();
    }
    delay(50);
  }
  triggerLastState = triggerState;

  if (isRunning) {
    timer.check();
  }
}
