#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <Stepper.h>
// date: 07 Apr 2023
// edit date: 9 May 2023


LiquidCrystal_I2C lcd(0x27, 16, 2);

//------------------ Button Pins ------------------------//
#define leftButton 14
#define upButton 15
#define downButton 16
#define rightButton 17 

//------------------ Stepper ------------------------//
#define stepPin 5
#define dirPin 2

//------------------ Servo ------------------------//
Servo snippers;
#define servo 10
#define openAngle 0          
#define closedAngle 180

//----------------- User Input ------------------------//
unsigned int wireLength = 10;
unsigned int wireQuantity = 1;

//----------------- System Setting ------------------------//
int state = 0;  
int previousWireLength = 10;
int lenIncrement = 5;
int previousWireQuantity = 1;
int qtyIncrement = 1;
// this value of mmPerStep provides a precise step 
// for the stepper to produce the desired length
float mmPerStep = 0.0387; 

Stepper myStepper(200, dirPin, stepPin);


void setup() {
  Serial.begin(9600);
 
  lcd.begin(16, 2); //LCD columns and rows
  lcd.backlight();

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);

  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  myStepper.setSpeed(360);
 
  snippers.attach(servo);
  snippers.write(openAngle);
  
  delay(1000);
}

void loop() {
  if (!digitalRead(rightButton)){
    if(state == 5){
      state = 0;
    }
    else{
      state += 1;
    }
    delay(200);
    lcd.clear();
  }
  if (!digitalRead(leftButton) && state > 0 && state < 4){
    state -=1;
    delay(200);
    lcd.clear();
  }

 
  switch (state){
    case 0:
      homeScreen();
      break;
    case 1:
       chooseWireLength();
       break;
    case 2:
      chooseWireQuantity();
      break;
    case 3:
      confirm();
      break;
    case 4:
      currentlyCutting();
      break;
    case 5:
      finishedCutting();
      break;
   }
}

void homeScreen(){
  lcd.setCursor(0, 0);
  lcd.print("WIRE CUTTER");
  lcd.setCursor(11, 1);
  lcd.print("NEXT>");
  delay(100);
}

void chooseWireLength(){
  wireLength = changeLenValue(wireLength);
 
  //clear LCD if required
  if(previousWireLength != wireLength){
    lcd.clear();
    previousWireLength = wireLength;
  }
 
  //Display information on LCD
  lcd.setCursor(0, 0);
  lcd.print("LENGTH:" + (String)wireLength + "mm");
  displayNavigation();
}

void chooseWireQuantity(){
  wireQuantity = changeQtyValue(wireQuantity);
 
  //clear LCD if required
  if(previousWireQuantity != wireQuantity){
    lcd.clear();
    previousWireQuantity = wireQuantity;
  }
 
  //Display information on LCD
  lcd.setCursor(0, 0);
  lcd.print("QUANTITY:" + (String)wireQuantity);
  displayNavigation();
}

void confirm(){
  lcd.setCursor(0, 0);
  lcd.print((String)wireLength + "mm x " + (String)wireQuantity + "pcs");
  lcd.setCursor(0, 1);
  lcd.print("<BACK");
  lcd.setCursor(10, 1);
  lcd.print("START>");
  delay(500);
}

void currentlyCutting(){
  lcd.setCursor(0, 0);
  lcd.print((String)0 + "/" + (String)wireQuantity);
  lcd.setCursor(0, 1);
  lcd.print("???s");

  int stepsToTake = (int)wireLength/mmPerStep;
  for(int i = 0; i < wireQuantity; i++){
 
    unsigned long timeForOneCycle = millis();
    
    // Wire feeder
    Serial.println("clockwise");
    myStepper.step(stepsToTake);
    delay(500);
    
    lcd.setCursor(0, 0);
    lcd.print((String)(i+1) + "/" + (String)wireQuantity);

    // Wire cutting
    snippers.write(closedAngle);
    delay(1000);
    snippers.write(openAngle);
    delay(1000);
 
    
    lcd.setCursor(0, 1);
 
    unsigned long timeRemaining = ((millis() - timeForOneCycle)*(wireQuantity - (i+1)))/1000;
    lcd.print((String)timeRemaining + "s    ");
    
  }
  wireLength = 10;
  wireQuantity = 1;
  state = 5;
}

 
void finishedCutting(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CUTTING COMPLETE");
  lcd.setCursor(11, 1);
  lcd.print("NEXT>");
  delay(100);
}

int changeLenValue(int currentValue){
  /******************************************************************
  * to check if the input wire length is greater than 100 mm (10cm)
  * if so set the wirelength to 100 mm only
  * 100 mm (10cm) is the max length for this prototype
  ******************************************************************/
  if (currentValue > 100){
    currentValue = 100;
  }
  else{
    if (!digitalRead(upButton)) {
      currentValue += lenIncrement;
      lenIncrement += 5;
    }
    if (!digitalRead(downButton)) {
      if(currentValue - lenIncrement >= 10){
          currentValue -= lenIncrement;
          lenIncrement += 5;
      }
      else{
          currentValue = 10;
      }
    }
    if (digitalRead(downButton) && digitalRead(upButton)){
      lenIncrement = 5;
    }
  }
  return currentValue;
}

int changeQtyValue(int currentValue){
  if (!digitalRead(upButton)) {
    currentValue += qtyIncrement;
    qtyIncrement += 1;
  }
  if (!digitalRead(downButton)) {
    if(currentValue - qtyIncrement >= 1){
      currentValue -= qtyIncrement;
      qtyIncrement += 1;
    }
    else{
      currentValue = 1;
    }
  }
  if (digitalRead(downButton) && digitalRead(upButton)){
    qtyIncrement = 1;
  }
  return currentValue;
}
  
void displayNavigation(){
  lcd.setCursor(0, 1);
  lcd.print("<BACK");
  lcd.setCursor(11, 1);
  lcd.print("NEXT>");
  delay(100);
}
