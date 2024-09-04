// Pin 6: buzzer
// Pin 9: trigPin Ultrasonic
// Pin 10: echoPin Ultrasonic
// Pin 8: Servo
// Pin A5: SCL LCD
// Pin A4: SDA LCD

#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>
long duration, cm, inches;
Servo myServo;
int trigPin = 9;
int echoPin = 10;
int buzzer = 6:
int initalPos = 0;
LiquidCrystal_I2C lcd(0x27,20,4);

void stop();

void setup() {
  int i;
  int n;
  lcd.init();
  lcd.backlight();

  myServo.attach(8);
  Serial.begin (9600);
  pinMode (trigPin, OUTPUT);
  pinMode (echopin, INPUT);
  pintMode (buzzer, OUTPUT);

}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);

  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) / 29.1;
  inches = (duration/2) / 74;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(250);

  lcd.setCursor(1,0);
  lcd.print("  Pengisi air  ");
  lcd.setCursor(1,1);
  lcd.print("  Otomatis  ");
  myServo.write(initialPos + 0);

  if (cm <= 11){
    myServo.write(initialPos + 180);//pencet ON
    digitalWrite(buzzer, HIGH);

    lcd.setCursor(1,0);
    lcd.print("  Sedang mengisi  ");
    lcd.setCursor(1,1);
    lcd.print("  air  ");
    delay(1000);

    myServo.write(initialPos + 0);//balik semula
    delay(4500);//isi air
    myServo.write(initialPos + 180);//mencet OFF
    digitalWrite(buzzer, LOW);
    delay(600);
    myServo.write(initalPos + 0);//balik semula
    delay(2000);

    if (cm <= 11){
      stop();
    }
    else if (cm > 11){
      loop();
    }
  }
}

void stop (){
  myServo.write(initalPos + 0);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);

  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) / 29.1;
  inches = (duration/2) / 74;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(250);

  if (cm>11){
    loop();
  }
  else{
    lcd.SetCursor(1,0);
    lcd.print("  Lepas Gelas  ");
    lcd.SetCursor(0,1);
    lcd.print("   Ok");
    stop():
  }
}