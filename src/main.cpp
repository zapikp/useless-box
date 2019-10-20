//prechod na GitHub
#include <Arduino.h>
#include <Servo.h>

//piny
#define ON  2
#define LED 3
#define PACKA 9
#define VIKO  10
#define IR A0
#define RAND  A1
#define SWITCH_OFF 5
#define SWITCH_ON 4
//nastaveni
#define TIMEOUT 60
#define SEKUNDA 1000
//pozice serv
#define PACKA_UVNITR 20
#define PACKA_PACKA 142
#define PACKA_OFF 175
#define PACKA_VEN 80

#define VIKO_CLOSE 166
#define VIKO_FULL 95
#define VIKO_STRED 110
//urovne snimace
#define IR_TOUCH 750
//doba prebehu serv
#define PREJEZD 800
//nastaveni jasu
#define LED_JAS 200



//hlavicky funkci
Servo serv_viko;
Servo serv_packa;
void switch_off();
void switch_touch_off();
void switch_touch();
void switch_out();
void switch_boom();
void viko_boom();
void switch_off_slow();
void switch_off_viko_slow();

//promenne
uint16_t  timeout=0;
uint32_t  cas;
uint8_t   stav=0;
uint8_t   start=1;
uint8_t   aktiv=0;
uint8_t   aktiv_touch=0;
uint8_t   viko_hyster=50;


void setup() {
  // put your setup code here, to run once:
  pinMode(ON,OUTPUT);
  digitalWrite(ON,HIGH); //zapnuti napajeni
  pinMode(PACKA, OUTPUT);
  pinMode(VIKO,OUTPUT);
  pinMode(SWITCH_OFF, INPUT_PULLUP);
  pinMode(SWITCH_ON, INPUT_PULLUP);
  cas=millis();
  serv_packa.attach(PACKA);
  serv_viko.attach(VIKO);
  serv_packa.write(PACKA_UVNITR);
  serv_viko.write(VIKO_CLOSE);
  Serial.begin(115200);
  randomSeed(analogRead(A1));
  //pro ověření mezí serv
  /*
  while (1) {
    uint16_t hodnota=analogRead(A3);
    uint8_t poloha=map(hodnota,0,1023,20,175);
    serv_packa.write(poloha);
    serv_viko.write(poloha);
    Serial.println(poloha);
    delay(10);
  }
  */
  /*
  //overeni cinnosti infra cidla
  while (1) {
    Serial.println(analogRead(IR));
    delay(100);
  }
  */
  Serial.print(stav);
  Serial.print(" Start ");
  Serial.println(start);
}

void loop() {
  // put your main code here, to run repeatedly:
  //obsluha timeoutu
  if (millis() >(cas + SEKUNDA)){
    cas +=SEKUNDA;
    timeout++;
    if (timeout > TIMEOUT){
    serv_packa.write(PACKA_UVNITR);
    serv_viko.write(VIKO_CLOSE);
    delay(PREJEZD);
    digitalWrite(ON,LOW);
    }
  }
//obsluha události dotyk
if (aktiv_touch < 4){
  while ((analogRead(IR) < (IR_TOUCH-viko_hyster))&&(start==0)){
    if (aktiv==0){
      viko_hyster=0;
    }
    aktiv=1;

    serv_viko.write(VIKO_STRED);
    analogWrite(LED, LED_JAS);
    if (digitalRead(SWITCH_OFF)==0){
      break;
    }
    Serial.println(analogRead(IR));
    delay(50);
  }
}
//zavreni vika
  //Serial.println(analogRead(IR));
  if ((aktiv==1)&&(analogRead(IR)> (IR_TOUCH-viko_hyster))){
    viko_hyster=50;
    if (digitalRead(SWITCH_OFF)==1){
      for (uint8_t i=VIKO_STRED; i < VIKO_CLOSE; i++){
        serv_viko.write(i);
        delay(10);
      }
    }
    aktiv=0;
    analogWrite(LED, 0);
    timeout=0;
  }
//obsluha události switch
  if (digitalRead(SWITCH_OFF)==0){
    //switch_off_slow();
    //*
    Serial.print(stav);
    Serial.print(" Start ");
    Serial.println(start);
    aktiv_touch=0;
    switch(stav){
      case 0:
        switch_off();
        aktiv_touch=random(0, 10);
        Serial.print("Aktiv_touch: ");
        Serial.println(aktiv_touch);
      break;

      case 1:
        switch_off_slow();
        aktiv_touch=random(0, 10);
        Serial.print("Aktiv_touch: ");
        Serial.println(aktiv_touch);
      break;

      case 2:
        switch_touch_off();
        aktiv_touch=random(0, 10);
        Serial.print("Aktiv_touch: ");
        Serial.println(aktiv_touch);
      break;

      case 3:

        switch_off_viko_slow();
        aktiv_touch=random(0, 10);
        Serial.print("Aktiv_touch: ");
        Serial.println(aktiv_touch);
      break;

      case 4:
        viko_boom();
        aktiv_touch=random(0, 10);
        Serial.print("Aktiv_touch: ");
        Serial.println(aktiv_touch);
        stav=0;
        start=0;
      break;



    }

    //*/
    if (start==1){
        stav++;
    } else {
        stav=random(5);

    }
  }

}

//funkce ========================================
void switch_off(){
  serv_viko.write(VIKO_FULL);
  serv_packa.write(PACKA_OFF);
  analogWrite(LED, LED_JAS);
  while (digitalRead(SWITCH_OFF)==0){
    delay(10);
  }
  //delay(PREJEZD);
  serv_packa.write(PACKA_UVNITR);
  delay(PREJEZD/3);
  serv_viko.write(VIKO_CLOSE);
  analogWrite(LED, 0);
  timeout=0;
}

void switch_touch_off(){
  serv_viko.write(VIKO_FULL);
  serv_packa.write(PACKA_PACKA);
  analogWrite(LED, LED_JAS);
  //delay(PREJEZD/3);
  //serv_viko.write(VIKO_STRED);
  delay(PREJEZD);
  serv_packa.write(PACKA_OFF);
  serv_viko.write(VIKO_FULL);
  while (digitalRead(SWITCH_OFF)==0){
    delay(10);
  }
  serv_packa.write(PACKA_UVNITR);
  delay(PREJEZD/2);
  serv_viko.write(VIKO_CLOSE);
  analogWrite(LED, 0);
}

void switch_touch(){
  serv_packa.write(PACKA_PACKA);
  delay(PREJEZD);
  serv_packa.write(PACKA_UVNITR);
  delay(PREJEZD);
  timeout=0;
}

void switch_out(){
  serv_packa.write(PACKA_VEN);
  delay(PREJEZD);
  timeout=0;
}


void switch_boom(){
  serv_viko.write(VIKO_FULL);
  analogWrite(LED, LED_JAS);
  for (uint8_t i=0; i < 4; i++){
    serv_packa.write(PACKA_PACKA);
    delay(PREJEZD/4);
    serv_packa.write(PACKA_VEN);
    delay(PREJEZD/4);
  }
  serv_packa.write(PACKA_OFF);
  while (digitalRead(SWITCH_OFF)==0){
    delay(10);
  }
  serv_packa.write(PACKA_UVNITR);
  delay(PREJEZD/3);
  serv_viko.write(VIKO_CLOSE);
  analogWrite(LED, 0);
  timeout=0;
}

void viko_boom(){
  for (uint8_t i=0; i < 4; i++){
      serv_viko.write(VIKO_STRED);
      delay(PREJEZD/6);
      serv_viko.write(VIKO_CLOSE);
      delay(PREJEZD/6);
  }
  switch_off();
  timeout=0;
}
void switch_off_slow(){
  serv_viko.write(VIKO_FULL);
  analogWrite(LED, LED_JAS);
  for (uint8_t i=PACKA_UVNITR; i < PACKA_PACKA; i++){
      serv_packa.write(i);
      delay(10);
  }
  delay(PREJEZD/3);
  serv_packa.write(PACKA_OFF);
  while (digitalRead(SWITCH_OFF)==0){
    delay(10);
  }
  //delay(PREJEZD);
  serv_packa.write(PACKA_UVNITR);
  delay(PREJEZD/3);
  serv_viko.write(VIKO_CLOSE);
  analogWrite(LED, 0);
  timeout=0;

}

void switch_off_viko_slow(){
  uint8_t jas_step=255/(VIKO_CLOSE-VIKO_FULL);
  uint8_t jas_led=0;
  for (uint8_t i=VIKO_CLOSE; i > VIKO_FULL; i--){
      serv_viko.write(i);
      delay(40);
      analogWrite(LED,jas_led);
      jas_led=jas_led+jas_step;
  }
  /*
  for (uint8_t i=PACKA_UVNITR; i < PACKA_PACKA; i++){
      serv_packa.write(i);
      delay(1);
  }
  delay(PREJEZD/3);
  */
  serv_packa.write(PACKA_OFF);
  while (digitalRead(SWITCH_OFF)==0){
    delay(10);
  }
  serv_packa.write(PACKA_UVNITR);
  delay(PREJEZD/3);
  serv_viko.write(VIKO_CLOSE);
  analogWrite(LED, 0);
  timeout=0;
}
