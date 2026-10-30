#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial BT(6, 5);

String str = "Hello from Arduino";
char cmd;

// הגדרות פינים
const int trigPin = 4;
const int echoPin = 2;

const int redLED = 11;
const int greenLED = 12;
const int buzzer = 13;

const int servoPin = 10;
const int lightSensorPin = A0; // חיישן אור

Servo gateServo;

// מרחק שמוגדר כ"זיהוי רכב" (בס"מ)
const int detectionDistance = 10;

// סף חושך – ערך אנלוגי מתחתיו נחשב "לילה"
const int lightThreshold = 400;

// מצב קודם של השער (true = פתוח, false = סגור)
bool gateIsOpen = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Gate close!");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  gateServo.attach(servoPin);
  gateServo.write(0);

  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
  digitalWrite(buzzer, LOW);

  Serial.begin(9600);
  BT.begin(9600);
  BT.print(str);
}

void loop() {
  long duration;
  int distance;
  int lightLevel = analogRead(lightSensorPin);
  bool isDark = lightLevel < lightThreshold;

  // קריאת חיישן מרחק
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; 

  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Light Level: ");
  Serial.println(lightLevel);

  // קריאת פקודה מהבלוטוס
  if (BT.available()) {
    cmd = BT.read();
  }

  // תנאים לפתיחת שער:
  bool openByDistance = (distance > 0 && distance <= detectionDistance);
  bool openByBluetooth = (cmd == '1');

  if ((isDark && openByBluetooth) || (!isDark && (openByDistance || openByBluetooth))) {
    if (!gateIsOpen) {
      gateServo.write(90); // פתיחה
      lcd.setCursor(0, 0);
      lcd.print("Gate open!       ");
      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, LOW);
      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);
      gateIsOpen = true;
    }
  } else {
    if (gateIsOpen) {
      gateServo.write(0); // סגירה
      lcd.setCursor(0, 0);
      lcd.print("Gate close!      ");
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);
      gateIsOpen = false;
    }
  }

  delay(1000);
}