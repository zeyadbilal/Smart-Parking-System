#include<Servo.h>
#include<LiquidCrystal_I2C.h>

// SCL A5
// SDA A4




#define entraceSensor1  2   //S1 entrance
#define entraceSensor2  3  //S2 entrance

#define exitSensor1 6
#define exitSensor2 7

#define ldrPin A1
#define ledPin1 11
#define ledPin2 12

//#define autoManualPin
//#define enterControl
//#define exitControl

#define buzzerEnter 9
#define buzzerExit 10 

#define lpgPin A3


LiquidCrystal_I2C lcd(0x27 , 16 , 2);

// Enterance Readings
int entranceRead1;
int entranceRead2;

int flaglpg ;

// Exit Readings
int exitRead1;
int exitRead2;

//Servos
Servo EntranceServo;
Servo ExitServo;

const int threshold = 200;
int ldrValue;  
int lpgvalue;
int autoManualControl; // 1 -> manual  0 -> auto

int exitControlReading;
int enterControlReading;


int avaliablePlaces = 5 ;


void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();

    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
    
    pinMode(entraceSensor1 , INPUT);
    pinMode(entraceSensor2 , INPUT);
    
    pinMode(exitSensor1 , INPUT);
    pinMode(exitSensor2 , INPUT);

    // pinMode(exitControl , OUTPUT);
    // pinMode(enterControl , OUTPUT);
    // pinMode(autoManualPin, INPUT);
    
    pinMode(buzzerExit , OUTPUT);
    digitalWrite(buzzerExit, LOW); 
    pinMode(buzzerEnter , OUTPUT);
    digitalWrite(buzzerEnter, LOW); 
    
    EntranceServo.attach(4);
    EntranceServo.write(0);

    ExitServo.attach(8);
    ExitServo.write(0);
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Avaliable places :" );
  lcd.setCursor(7, 1);
  lcd.print(avaliablePlaces);
  delay(100);

  autoControl();
  ldrOperation();
  lpg_();
  updateReadingValues();

}

// void checkAutoManualMode()
// {
//   if(autoManualControl)
//   {
//     manualControl();
//   }
//   else
//   {
//     autoControl();
//   }
// }

// void manualControl()
// {
//   if(enterControlReading)
//   {
//     EntranceServo.write(90);
//   }
//   else if(!enterControlReading) 
//   {
//     EntranceServo.write(90);
//   }

//   if(exitControlReading)
//   {
//         ExitServo.write(90);
//   }
//   else if(!exitControlReading) 
//   {
//     ExitServo.write(0)
//   }

  
// }

void autoControl()
{
    EntranceGateOperation();
    ExitGateOperation();
}

void updateReadingValues()
{

  entranceRead1 = !digitalRead(entraceSensor1);
  entranceRead2 = !digitalRead(entraceSensor2);
  
  exitRead1 = !digitalRead(exitSensor1);
  exitRead2 = !digitalRead(exitSensor2);

  ldrValue = analogRead(ldrPin);
  lpgvalue = analogRead(lpgPin);

  // autoManualControl = digitalRead(autoManualPin);
  // exitControlReading = digitalRead(exitControl);
  // enterControlReading = digitalRead(enterControl);
}

void ldrOperation()
{

  // Serial.println(ldrValue);       // print sensor value
  if(ldrValue<threshold){ 
    if(avaliablePlaces < 5 && avaliablePlaces >= 3)
    {
    digitalWrite(ledPin1,HIGH);  // turn LED on
    digitalWrite(ledPin2,LOW);
    }
    else if(avaliablePlaces < 3)
    {
    digitalWrite(ledPin2,HIGH);
    digitalWrite(ledPin1,HIGH);  // turn LED on
    }
  }  
  else{  // else, if sensor reading is greater than threshold
    digitalWrite(ledPin1,LOW);    // turn LED off
    digitalWrite(ledPin2,LOW);    // turn LED off
  }
}

void ExitGateOperation()
{
  if(exitRead1 && avaliablePlaces < 5)
    {
      ExitServo.write(90);
       lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Car Exiting");
      unsigned long startTime = millis();
      while(!exitRead2)
        {
          exitRead2 = !digitalRead(exitSensor2);
          if (millis() - startTime >= 10000 && digitalRead(exitSensor1)) {
          ExitServo.write(0);
          break;
        }}
      lcd.clear();
    }
    if(exitRead2 && ExitServo.read() == 90)
    {
      avaliablePlaces++;
      ExitServo.write(0);
      delay(1500);
    }
    while(exitRead2 && ExitServo.read() == 0)
    {
      digitalWrite(buzzerExit , HIGH);
      exitRead2 = !digitalRead(exitSensor2);
    }
    digitalWrite(buzzerExit , LOW);
}

void EntranceGateOperation()
{
  if(entranceRead1  && avaliablePlaces > 0){
      EntranceServo.write(90);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Car Entering");
      unsigned long startTime = millis();
      while(!entranceRead2)
        {
          entranceRead2 = !digitalRead(entraceSensor2);
          if (millis() - startTime >= 10000 && digitalRead(entraceSensor1)) {
          EntranceServo.write(0);
          break;
        }}
      lcd.clear();
    }
    if(entranceRead2 && !entranceRead1 && EntranceServo.read() == 90){
      EntranceServo.write(0);
      avaliablePlaces--;
      delay(1500);
    }
    while(entranceRead2 &&  EntranceServo.read() == 0){
      digitalWrite(buzzerEnter , HIGH);
         entranceRead2 = !digitalRead(entraceSensor2);

    }
    digitalWrite(buzzerEnter,LOW);
}

void lpg_() {
  lpgvalue = analogRead(lpgPin);
  Serial.println(lpgvalue);
  if (lpgvalue >= 600) {
    flaglpg = 1;
    digitalWrite(ledPin1, 1);
    digitalWrite(ledPin2, 1);
    EntranceServo.write(90);
    ExitServo.write(90);
    digitalWrite(buzzerEnter, 1);
    digitalWrite(buzzerExit, 1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FIRE..FIRE");
    delay(1000);
  }
  if (lpgvalue < 500 && flaglpg == 1) {
    flaglpg = 0;
    digitalWrite(buzzerEnter, 0);
    digitalWrite(buzzerExit, 0);
    EntranceServo.write(0);
    ExitServo.write(0);
    delay(10);
    // digitalWrite(led1,0);
    // digitalWrite(led2,0);
  }
}



// void fireDetectorOperation()
// {
//   Serial.println(lpgvalue);
//   while( lpgvalue >= 500 ){

//     digitalWrite(ledPin1,1);
//     digitalWrite(ledPin2,1);
   
//     EntranceServo.write(90);
//     ExitServo.write(90);
   
//     digitalWrite(buzzer,1);
//     digitalWrite(buzzer,1);
   
//     lcd.clear();
//     lcd.setCursor(0,0);
//     lcd.print("FIRE..FIRE");
  
//   }
//   if( lpgvalue < 500 && EntranceServo.read() == 90 && ExitServo.read() == 90 && entersens1 == 0 ){

//     digitalWrite(buzzer,0);
//     digitalWrite(buzzer,0);
    
//     EntranceServo.write(0);
//     ExitServo.write(0);
    
// }
// }
