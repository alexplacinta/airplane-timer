#include <EnableInterrupt.h>
#include <Servo.h>
#include <TimedAction.h>
#include <TM1637Display.h>

#define servoPin A4 //4
#define displayCLK 5
#define displayDIO 6
#define rotaryEncA A1 //8
#define rotaryEncB A0 //7
#define buttonPin A2 //9
#define triggerPin A3 //2
#define rgbRedPin 10
#define rgbGreenPin 11
#define rgbBluePin 12

Servo myservo;
TM1637Display display = TM1637Display(displayCLK, displayDIO);

int minTime = 10;
int maxTime = 180;
int stepTime = 10;
volatile int totalTime = minTime;
volatile int runningTime = totalTime;
volatile bool isRunning;
int triggerState;
int triggerLastState;

void tick() {
  runningTime--;
  if (runningTime == 0) {
    
    display.showNumberDec(totalTime);
    myservo.attach(servoPin);
    myservo.write(90);
    delay(500);
    myservo.write(0);
    isRunning = false;
    delay(500);
    myservo.detach();
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
//  myservo.attach(servoPin);
//  myservo.write(0);
//  delay(1000);
//  myservo.detach();
  Serial.println("Reset");
}

void triggerChange() {
  //  isRunning = false;
  //  runningTime = totalTime;
  //  Serial.print("TRIGGER: ");
  //  Serial.println(digitalRead(triggerPin));
}

void RGB_color(int redValue, int greenValue, int blueValue)
{
  analogWrite(rgbRedPin, redValue);
  analogWrite(rgbGreenPin, greenValue);
  analogWrite(rgbBluePin, blueValue);
}

void setup() {
  pinMode(rotaryEncA, INPUT);
  pinMode(rotaryEncB, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(triggerPin, INPUT_PULLUP);
//  pinMode(rgbRedPin, OUTPUT);
//  pinMode(rgbGreenPin, OUTPUT);
//  pinMode(rgbBluePin, OUTPUT);
  isRunning = false;
//  RGB_color(0, 0, 70);
//  myservo.attach(servoPin);
//  myservo.write(0);
//  delay(1000);
//  myservo.detach();

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
