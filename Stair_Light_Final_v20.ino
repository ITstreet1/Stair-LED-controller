/* Stair Light sketch
 * Author: Petrovic Dejan
 * Date 26 Aug 2022
 * 
 * V2.0
 * Dodato:
 * Oled ekran
 * 5x Pot
 * 
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SHIFTPWM_NOSPI                            //  Uncomment this part to NOT use the SPI port and change the pin numbers. This is 2.5x slower  Do not change!!!
const int ShiftPWM_dataPin = 4;                   //  Do not change!!!
const int ShiftPWM_clockPin = 2;                  //  Do not change!!!
const int ShiftPWM_latchPin = 3;                  //  Do not change!!!
///
int pot1 = A1;  //light
int pot2 = A2;  //open time
int pot3 = A3;  //stair nr
int pot4 = A6;  //dist
int pot5 = A7;  //pwm
int light, stairNR, dist, pwm = 0;
int mapLight, mapStairNR, mapDist, mapPwm = 0;
long opTime, mapOpTime = 0;
///
#include <ShiftPWM.h>
const bool ShiftPWM_invertOutputs = false;         
const bool ShiftPWM_balanceLoad = false;         
#define sensorLightPin A0                         // Light sensor  Do not change!!!
const int trigPin1 = 11;                          // Upper sensor  Do not change!!!
const int echoPin1 = 12;                          // Upper sensor  Do not change!!!
const int trigPin2 = 9;                           // Bottom sensor Do not change!!! 
const int echoPin2 = 10;                          // Bottom sensor Do not change!!! 
unsigned char maxBrightness = 100;                // Here you set the number of brightness levels. Adjust(0-255)!!
unsigned char pwmFrequency = 75;                  // These values affect the load of ShiftPWM 
unsigned int numRegisters = 3;                    // Number of shift registers Do not change!!!
boolean sensorUpperActive;
boolean sensorBottomActive;
int numberOfstairs;                               // ! 3 ! Setup Number of Your stairs. Adjust UP TO 24 !!!!
int pause;                                        // ! 2 ! Setup delay ON/OFF  Adjust!!! (optimal value 30000)
byte faidoutSpeed=5;                              // ! 5 ! How fast to fadeout     Adjust!!!
byte faidinSpeed=5;                               // ! 5 ! How fast to fadein      Adjust!!!
int val;
int photoCellTreshold;                            // ! 1 ! LDR treshold (optimal value 300)
long duration1, duration2;
int distance1, distance2;
int distanceCheck;                                // ! 4 ! Sonar distance
//oled
#define SCREEN_ADDRESS 0X3C
Adafruit_SSD1306 display(128, 64, &Wire, -1);
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 25);
  display.print("Loading...");
  display.display();
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(pwmFrequency, maxBrightness);
  pinMode(sensorLightPin, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  Serial.begin(9600);
  delay(1000);
  display.setRotation(1);
  display.setTextSize(2);
}
void loop() {
checkPot();
displayData();
val = analogRead(sensorLightPin);                  //Checking light level 
  if (val <= photoCellTreshold){                   // Adjust photocell cencivity. (val<10 dark.....val>800 bright..)    !!!!!!!!                                               
    BottomTriggerFire();                           //Checking bottom sensor
    UpperTrigerFire();                             //Checking upper sensor
    switchONOFFfromdown();                         //Stairs on/off from down to up
    switchONOFFfromUp();                           //Stairs on/off from up to down
  }
}
void BottomTriggerFire() {
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.034 / 2;
  if (distance2 <= distanceCheck ) {
    sensorBottomActive = true;
    Serial.println("bottom active");
  }
}
void UpperTrigerFire() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.034 / 2;
  if (distance1 <= distanceCheck ) {
    sensorUpperActive = true;
    Serial.println("upper active");
  }
}
void switchONOFFfromdown() {
  if (sensorBottomActive==true && sensorUpperActive==false){
        for(int i=0; i<numberOfstairs; i++){
          Serial.print(i);
          for(int a=0; a<maxBrightness; a++){
          ShiftPWM.SetOne(i,a);
          delay(faidoutSpeed);
           }
          }
        Serial.println();
        Serial.println("Pause ON");
        delay(pause); 
        Serial.println();
        Serial.println("Pause OFF");
        for(int i=0; i<numberOfstairs; i++){
          Serial.print(i);
          for(int a=maxBrightness;a>=0;a--){
          ShiftPWM.SetOne(i,a);
          if(a==0){
          ShiftPWM.SetOne(i,0);  
          }
          delay(faidinSpeed);
           }
          }
        sensorBottomActive = false ;
        Serial.println("bottom false");
  }
}
void switchONOFFfromUp () {
    if ( sensorUpperActive==true && sensorBottomActive==false) {
        for(int i=numberOfstairs-1; i>=0; i--){
          Serial.print(i);
          for(int a=0; a<maxBrightness; a++){
          ShiftPWM.SetOne(i,a);
          delay(faidoutSpeed);
           }
          }
        Serial.println();
        Serial.println("Pause ON");
        delay(pause); 
        Serial.println();
        Serial.println("Pause OFF");
        for(int i=numberOfstairs-1; i>=0; i--){
          Serial.print(i);
          for(int a=maxBrightness;a>=0;a--){
          ShiftPWM.SetOne(i,a);
          if(a==0){
          ShiftPWM.SetOne(i,0);  
          }
          delay(faidinSpeed);
           }
          }
        sensorUpperActive= false ;  
        Serial.println("Upper false");
  }
}
void checkPot(){
  //light
  light = analogRead(pot1);
  photoCellTreshold = light;  //declaration of light
  Serial.println(light);
  mapLight = map(light, 0, 1020, 1, 10);
  Serial.println(mapLight);
  //Time
  opTime = analogRead(pot2);
  Serial.println(opTime);
  mapOpTime = map(opTime, 0, 1020, 1000, 60000);
  pause = mapOpTime;          //declaration of open time
  Serial.println(mapOpTime);
  //Stair
  stairNR = analogRead(pot3);
  Serial.println(stairNR);
  mapStairNR = map(stairNR, 0, 1020, 1, 24);
  numberOfstairs = mapStairNR; //declaration nr of stairs
  Serial.println(mapStairNR);
  //Dist
  dist = analogRead(pot4);
  Serial.println(dist);
  mapDist = map(dist, 0, 1020, 1, 100);
  distanceCheck = mapDist;     //declaration of distance
  Serial.println(mapDist);
  //PWM
  pwm = analogRead(pot5);
  Serial.println(pwm);
  mapPwm = map(pwm, 0, 1020, 1, 10);
  faidinSpeed = mapPwm;          //declaration of pwm
  faidoutSpeed = mapPwm;         //declaration of pwm
  Serial.println(mapPwm);
}
void displayData(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(mapLight);
  display.setCursor(0,25);
  int mapOpTimeToDisplay = mapOpTime/1000;
  display.print(mapOpTimeToDisplay);
  display.setCursor(0,50);
  display.print(mapStairNR);
  display.display();
  display.setCursor(0,75);
  display.print(mapDist);
  display.setCursor(0,100);
  display.print(mapPwm);
  display.display();
}
