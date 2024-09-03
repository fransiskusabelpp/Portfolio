//MASTER
int MC2_FSLP1 = 2 ,MC2_FSLP2 = 3, MC2_SSLP1_GREEN = 4, MC2_SSLP1_RED = 5, MC2_SSLP2_GREEN = 6, MC2_SSLP2_RED = 7;
int MC1_FSLP1 = 8, MC1_FSLP2 = 9, MC1_SSLP1_GREEN = 10, MC1_SSLP1_RED = 11, MC1_SSLP2_GREEN = 12, MC1_SSLP2_RED = 13;
int swCabin1 = 17;
int swCabin2 = 16;
int forwardState = 15;
int reverseState = 14;
int cabinState1;
int cabinState2;
int forwardValue;
int reverseValue;

void setup() {
  pinMode(swCabin1,INPUT);
  pinMode(swCabin2,INPUT);
  pinMode(MC1_FSLP1,OUTPUT);
  pinMode(MC1_FSLP2,OUTPUT);
  pinMode(MC1_SSLP1_GREEN,OUTPUT);
  pinMode(MC1_SSLP1_RED,OUTPUT);
  pinMode(MC1_SSLP2_GREEN,OUTPUT);
  pinMode(MC1_SSLP2_RED,OUTPUT); 
  pinMode(MC2_FSLP1,OUTPUT);
  pinMode(MC2_FSLP2,OUTPUT);
  pinMode(MC2_SSLP1_GREEN,OUTPUT);
  pinMode(MC2_SSLP1_RED,OUTPUT);
  pinMode(MC2_SSLP2_GREEN,OUTPUT);
  pinMode(MC2_SSLP2_RED,OUTPUT);
  Serial.begin(9600);
}


void loop() {
  cabinState1 = digitalRead(swCabin1);
  cabinState2 = digitalRead(swCabin2);
  forwardValue = digitalRead(forwardState);
  reverseValue = digitalRead(reverseState);
  
  if(cabinState1 == HIGH && cabinState2 == LOW && forwardValue == HIGH && reverseValue == LOW){//Cabin 1 Forward
    state1();
    state2();
    Serial.println("Cabin 1 Active, Train Forward");
  }
  else if(cabinState1 ==  HIGH && cabinState2 == LOW && forwardValue == LOW && reverseValue == HIGH){//Cabin 1 Reverse
    state3();
    state4();
    Serial.println("Cabin 1 Active, Train Reverse");
  }  
  else if(cabinState1 ==  LOW && cabinState2 == HIGH && forwardValue == HIGH && reverseValue == LOW){//Cabin 2 Forward
    state3();
    state4();
    Serial.println("Cabin 2 Active, Train Forward");
  }
  else if(cabinState1 == LOW && cabinState2 == HIGH && forwardValue == LOW && reverseValue == HIGH){//Cabin 2 Reverse
    state1();
    state2();
    Serial.println("Cabin 2 Active, Train Reverse");
  }  
  else Serial.println("ERROR!");
}

void state1(){//cabin 1 forward || cabin 2 reverse
  digitalWrite(MC1_FSLP1,LOW);
  digitalWrite(MC1_FSLP2,HIGH);
  digitalWrite(MC1_SSLP1_GREEN,LOW);
  digitalWrite(MC1_SSLP1_RED,LOW);
  digitalWrite(MC1_SSLP2_GREEN,LOW);
  digitalWrite(MC1_SSLP2_RED,LOW);
  //Serial.println("MASUK STATE 1");
}

void state2(){//cabin 1 forward || cabin 2 reverse
  digitalWrite(MC2_FSLP1,HIGH);
  digitalWrite(MC2_FSLP2,LOW);
  digitalWrite(MC2_SSLP1_GREEN,LOW);
  digitalWrite(MC2_SSLP1_RED,HIGH);
  digitalWrite(MC2_SSLP2_GREEN,HIGH);
  digitalWrite(MC2_SSLP2_RED,LOW);
  //Serial.println("MASUK STATE 2");
}

void state3(){//cabin 1 reverse || cabin 2 forward
  digitalWrite(MC1_FSLP1,HIGH);
  digitalWrite(MC1_FSLP2,LOW);
  digitalWrite(MC1_SSLP1_GREEN,LOW);
  digitalWrite(MC1_SSLP1_RED,HIGH);
  digitalWrite(MC1_SSLP2_GREEN,HIGH);
  digitalWrite(MC1_SSLP2_RED,LOW);
  //Serial.println("MASUK STATE 3");
}

void state4(){//cabin 1 reverse || cabin 2 forward
  digitalWrite(MC2_FSLP1,LOW);
  digitalWrite(MC2_FSLP2,HIGH);
  digitalWrite(MC2_SSLP1_GREEN,LOW);
  digitalWrite(MC2_SSLP1_RED,LOW);
  digitalWrite(MC2_SSLP2_GREEN,LOW);
  digitalWrite(MC2_SSLP2_RED,LOW);
  //Serial.println("MASUK STATE 4");
}