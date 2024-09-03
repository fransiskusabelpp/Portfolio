#include <Wire.h>
#include <math.h>
#define pi 3.14

//Calculation
float time = 500; //time respons system in ms
volatile int pulseCount; // Jumlah pulsa sensor yang terdeteksi
float d = 0.6; //tire diameter in meter
int gear = 80;//tooth gear
float minSpeed = 0;//min speed of train
float maxSpeed = 90; // max speed of train
float TimeMaxSpeed = 20; //max time of switch deadman
float TimeMinSpeed = 90; //min time of switch deadman
float yTime;
float xSpeed;
float slope;
int y; //time progressive
float rpm;
float pulseTime;
unsigned int speedMotor;

//value
int value;
int reset;
int cabin;
int cabinState1;
int cabinState2;
int forwardValue;
int reverseValue;
int val = 0;
int neutral = 1;//1 means neutral, 0 not neutral
int stop = 0;
int alert = 0;
double secOn = 0; //press
double secOff = 0; //release

//pin
const byte sensorPin = 2;
int btnReset = 13;
int btnCabin1 = 4;
int btnCabin2 = 5;
int button = 6;
int ledPress = 7;
int buzzer = 8;
int ledDeadman = 9;
int relayDeadman = 10;
int reverseState = 11;
int forwardState = 12;


void setup() {

  Wire.begin();
  pinMode(btnCabin1, INPUT);
  pinMode(btnCabin2, INPUT);
  pinMode(button,INPUT);
  pinMode(btnReset,INPUT);
  pinMode(forwardState,INPUT);
  pinMode(reverseState,INPUT);
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, FALLING);  
  pinMode(ledPress, OUTPUT);
  pinMode(relayDeadman,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(ledDeadman,OUTPUT);

  Serial.begin(9600);
}

void loop() {
  //cabin selection
  cabinState1 = digitalRead(btnCabin1);
  cabinState2 = digitalRead(btnCabin2);

  if(cabinState1 == LOW && cabinState2 == LOW ){
    cabin = 0;
    Wire.beginTransmission(9);//kirim ke motor
    val=0;
    Wire.write(val);
    Wire.endTransmission();
  }
  else if(cabinState1 == HIGH && cabinState2 == LOW ){
    cabin = 1;
    Wire.beginTransmission(9);//kirim ke motor
    val=1;
    Wire.write(val);
    Wire.endTransmission();
  }
  else if(cabinState1 == LOW && cabinState2 == HIGH ){
    cabin = 2;
    Wire.beginTransmission(9);//kirim ke motor
    val=2;
    Wire.write(val);
    Wire.endTransmission();
  }
  else if(cabinState1 == HIGH && cabinState2 == HIGH ){
    cabin = 0; 
    Wire.beginTransmission(9);//kirim ke motor
    val=0;
    Wire.write(val);
    Wire.endTransmission();
  }
  
  time = 500; //500 ms  
  delay(time);// in ms
  
  if(cabin == 1){
    forwardValue = digitalRead(forwardState);
    reverseValue = digitalRead(reverseState);
    
    if(forwardValue == HIGH){
      Serial.println("Forward");
      neutral = 0;
    }
    else if(reverseValue == HIGH){
      Serial.println("Reverse");
      neutral = 0;
    }
    else{
      Serial.println("Neutral");
      neutral = 1;
    }    

    detachInterrupt(digitalPinToInterrupt(sensorPin)); // Menonaktifkan interrupt untuk menghindari race condition   
    pulseTime = (time/1000)*gear;
    rpm = pulseCount * 60.0 / pulseTime;
    pulseCount = 0; // Reset jumlah pulsa sensor pada setiap loop
    attachInterrupt(digitalPinToInterrupt(sensorPin), countPulse, FALLING); // Mengaktifkan interrupt kembali    

    //calculate speed
    speedMotor = (pi*d*rpm*60)/1000;
    Serial.print("Speed : ");
    Serial.print(speedMotor);
    Serial.println(" km/h");

    /*
    PROGRESSIVE PRESS VALUE TIME WITH MANUAL CALCULATION! 
    x1 = min speed ; x2 = max speed ; y1 = min time of switch deadman ; y2 = max time of switch deadman
    x1=0 y1=90 dan x2=120 y2=20
    slope = (y2-y1) / (x2-x1)
    slope = (20 - 90) / (120 - 0)
    slope = -70 / 120
    slope = -0.5833

    y - y1 = slope(speedMotor - x1)
    y - 90 = -0.5833(speedMotor - 0)
    y - 90 = -0.5833(speedMotor)
    y = -0.5833(speedMotor) + 90
    */

    yTime = TimeMaxSpeed - TimeMinSpeed;
    xSpeed = maxSpeed - minSpeed;
    slope = yTime/xSpeed;
    y = (slope*(speedMotor - minSpeed)) + TimeMinSpeed;
    
    //Time progressive
    if(speedMotor >= minSpeed+2 && speedMotor <= maxSpeed && neutral == 0){
      Serial.print("Press progressive : "); 
      Serial.print(y); 
      Serial.println(" second");    
    }
    
    else if (speedMotor > maxSpeed && neutral == 0){
      y=20;
      Serial.print("Press progressive : "); 
      Serial.print(y); 
      Serial.println(" second");    
    } 


    if(stop==0){
      value=digitalRead(button);
      if (speedMotor >= minSpeed+2 && speedMotor <=maxSpeed && neutral == 0){
        Serial.print("State : ");
        if(value==HIGH){                                  //Press button
            secOff=0;
            digitalWrite(ledPress,HIGH);
            Serial.println("PRESS");

            if(alert!=0){
              digitalWrite(buzzer,LOW);
              digitalWrite(ledDeadman,LOW);
            }
            //cek waktu            
                secOn +=0.5;
                Serial.print ("Count : ");
                Serial.print(secOn);
                Serial.println(" second");
                if(secOn >= y){
                  Serial.println("Deadman active");
                  alert = 0;
                  deadActive();
                  beepBuzzer();
                  if(secOn >= y+3){
                    Serial.println("Powering Cut Off");                    
                    if(secOn >= y+8){
                      Serial.println("\nEMERGENCY BREAK!!!");
                      emergency();
                    }
                  }
                }      
        }
        else if(value==LOW){                               //Release Button
          secOn=0;
          digitalWrite(ledPress,LOW);
          Serial.println("RELEASE");

          if(alert!=1){
            digitalWrite(buzzer,LOW);
            digitalWrite(ledDeadman,LOW);
          }  

          secOff +=0.5;
          Serial.print ("Count : ");
          Serial.print(secOff);
          Serial.println(" second");          

          if(speedMotor >= minSpeed+2 && speedMotor <=maxSpeed/2){
            //cek waktu   
                if(secOff >= 6){
                  Serial.println("Deadman active");
                  alert = 1;
                  deadActive();
                  beepBuzzer();
                  if(secOff >= 9){
                    Serial.println("Powering Cut Off");                   
                    if(secOff >= 11){
                      Serial.println("EMERGENCY BREAK!!!");
                      emergency();
                    }
                  }
                }  
          }      
          else if(speedMotor > maxSpeed/2 && speedMotor <= maxSpeed){
            //cek waktu   
                if(secOff >= 5){
                  Serial.println("Deadman active");
                  alert = 1;
                  deadActive();
                  beepBuzzer();
                  if(secOff >= 8){
                    Serial.println("Powering Cut Off");                   
                    if(secOff >= 10){
                      Serial.println("EMERGENCY BREAK!!!");
                      emergency();
                    }
                  }
                }
          }   
        }//else low
      }//if speed

      else if (speedMotor > maxSpeed && neutral == 0){
        Serial.print("State : ");
        if(value==HIGH){                                  //Press button
            secOff=0;
            digitalWrite(ledPress,HIGH);
            Serial.println("PRESS");

            if(alert!=0){
              digitalWrite(buzzer,LOW);
              digitalWrite(ledDeadman,LOW);
            }
            //cek waktu            
                secOn +=0.5;
                Serial.print ("Count : ");
                Serial.print(secOn);
                Serial.println(" second");
                if(secOn >= y){
                  Serial.println("Deadman active");
                  alert = 0;
                  deadActive();
                  beepBuzzer();
                  if(secOn >= y+3){
                    Serial.println("Powering Cut Off");                    
                    if(secOn >= y+8){
                      Serial.println("\nEMERGENCY BREAK!!!");
                      emergency();
                    }
                  }
                }      
        }
        else if(value==LOW){                               //Release Button
          secOn=0;
          digitalWrite(ledPress,LOW);
          Serial.println("RELEASE");

          if(alert!=1){
            digitalWrite(buzzer,LOW);
            digitalWrite(ledDeadman,LOW);
          }  

          secOff +=0.5;
          Serial.print ("Count : ");
          Serial.print(secOff);
          Serial.println(" second");          
     
          //cek waktu   
              if(secOff >= 5){
                Serial.println("Deadman active");
                alert = 1;
                deadActive();
                beepBuzzer();
                if(secOff >= 8){
                  Serial.println("Powering Cut Off");                   
                  if(secOff >= 10){
                    Serial.println("EMERGENCY BREAK!!!");
                    emergency();
                  }
                }
              }  
        }//else low
      }//else

    }//if stop

    else if(stop = 1){
      reset = digitalRead(btnReset); 

      digitalWrite(relayDeadman, HIGH);
      // digitalWrite(btnForward, LOW);
      digitalWrite(buzzer,LOW);
      digitalWrite(ledDeadman,LOW);
      
      Wire.beginTransmission(9);//kirim ke motor
      val=0;
      Wire.write(val);
      Wire.endTransmission();
      Serial.println(val);

      if(speedMotor > 0){
        Serial.println("EMERGENCY BREAK!!!");
        if(reset == HIGH){
          stop = 0;
          secOn = 0;
          secOff = 0;
          digitalWrite(relayDeadman, LOW);
        }
      }
      if(speedMotor <= 0 ){
        Serial.println("EMERGENCY BREAK!!!");
        if(reset == HIGH){
          stop = 0;
          secOn = 0;
          secOff = 0;
          digitalWrite(relayDeadman, LOW);        
        }
      }
    }
    Serial.println(" ");
  }

  else{
    Serial.println("CANT ACCESS!!");
  }
}

void countPulse() {
  pulseCount++;
}

void emergency(){
  stop=1;
}

void beepBuzzer(){
  digitalWrite(buzzer,HIGH);  
}

void deadActive(){
  digitalWrite(ledDeadman,HIGH);
}