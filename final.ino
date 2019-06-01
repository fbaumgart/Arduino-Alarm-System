//Alarm system project using:
//Arduino UNO, PIR motion sensor, buzzer,  LCD 16x2 display, 4x4 keypad, GSM SIM800L module

//libraries
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

//constants
const byte BUZZER = 10; // pin 10 BUZZER
const byte PIRSENSOR = 13;   //pin 13 PIR SENSOR
const byte COUNTDOWN_TIME = 3; //time to arm the alarm

//variables
bool armedFlag = false;
bool alarmFlag = false;
String answer = ""; //user PIN answer when disarming
String hiddenchars = ""; //contains star symbols used to mask the PIN
String password = "1234"; //hardcoded PIN
byte i = 0; //iterator
int time = 0;
bool keyFlag = false;

//GSM
SoftwareSerial gsm800(11,12);

//LCD display init
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

//Keypad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {5, 4, 3, 2};  //COLUMN PINS
byte rowPins[ROWS] = {9, 8, 7, 6}; //ROW PINS
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);        //Serial init
  gsm800.begin(9600);
  pinMode(PIRSENSOR, INPUT);   //set PIRSENSOR pin as an input
  pinMode(BUZZER, OUTPUT);
  Serial.println("---- ALARM ----"); 
  lcd.begin(16, 2); //LCD type setup
}

void alarmOn() {
  for( ; i <= COUNTDOWN_TIME ; i++)
  {
    lcd.clear();
    digitalRead(PIRSENSOR); //start reading from PIR
    lcd.setCursor(0, 0); //set cursor at 0 row 0 column
    lcd.print("Alarm will be");
    lcd.setCursor(0, 1);
    lcd.print("armed in: ");
    lcd.print(COUNTDOWN_TIME - i); //Show elapsed time to arm the alarm
    delay(500);
    Serial.println(i);
    digitalWrite(BUZZER, HIGH); //buzzer sounds when countdown
    delay(500);
    digitalWrite(BUZZER, LOW);

    if(i == COUNTDOWN_TIME)
	{
      digitalWrite(BUZZER, HIGH);  //longer buzzer sound for the countdown end
      delay(900);
      digitalWrite(BUZZER, LOW);
    }
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm is armed"); 
  byte motion = digitalRead(PIRSENSOR);      //read from PIR
  Serial.println(motion);

  if(motion == HIGH)                      //serial output
  {                                     //HIGH = motion detected, LOW = no motion
    alarmFlag = true;
    Serial.println("Motion detected!");
    Serial.println(motion);
    Serial.println("Pass the code: ");
    lcd.clear();
    lcd.begin(16, 2); 
    lcd.setCursor(3, 0);
    lcd.print("***ALARM***");
    lcd.setCursor(0, 1); 
    lcd.print("Pass the code:"); 
    
    while(alarmFlag) //if motion detected
	{
      time++;
      Serial.println(time);
      digitalWrite(BUZZER, HIGH);
      delay(100);
      digitalWrite(BUZZER, LOW);
      delay(100); 
      char key = keypad.getKey();
     
      if(key) 
	  {
        if(key == '1' || key == '2' || key == '3' ||
           key == '4' || key == '5' || key == '6' ||
           key == '7' || key == '8' || key == '9' || key == '0')
		   {
           answer += key;
           hiddenchars += '*';
           lcd.setCursor(11, 1);
           lcd.print(hiddenchars);
           }
		else if (key == '*') //key to accept the provided PIN
		{
           lcd.setCursor(0, 0);
          if(answer == password)
		  {
             answer = "";
			       hiddenchars = "";
             armedFlag = false;
             alarmFlag = false;
             motion = LOW;
             i = 0;
             time = 0;
		  }
		  else
		  {
             answer = "";
			       hiddenchars = "";
			       lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("Wrong PIN!");
			       delay(2000);
             lcd.clear();
             lcd.setCursor(3, 0);
             lcd.print("***ALARM***");
             lcd.setCursor(0, 1); 
             lcd.print("Pass the code:"); 
          }
        }
		else if (key == '#') //reset PIN input
		{
			answer = "";
			hiddenchars = "";
            lcd.clear();
            lcd.setCursor(3, 0);
            lcd.print("***ALARM***");
            lcd.setCursor(0, 1); 
            lcd.print("Pass the code:");  
		}
	  }
    if(time > 150){
            lcd.clear();
            answer = "";
            hiddenchars = "";
            armedFlag = false;
            alarmFlag = false;
            ruch = LOW;
            i = 0;
            time = 0;
            gsm800.write("AT+CMGF=1\r\n");
            gsm800.write("AT+CMGS=\"+xx xxx xxx xxx\"\r\n"); //type your phone number here
            gsm800.write("Security breach!!!");
            delay(1000);
            gsm800.write((char)26);
            delay(1000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("No code provided!");
            lcd.setCursor(0, 1); 
            lcd.print("SMS sent"); 
            digitalWrite(BUZZER, HIGH); 
            delay(10000);
            digitalWrite(BUZZER, LOW);
        }
    }  
  }
  delay(100);
  digitalWrite(BUZZER, LOW);
  delay(100);
}

void loop()
{
  //variables
  char key = keypad.getKey();
  byte motion = digitalRead(PIRSENSOR);      //read from PIR

  //lcd
  lcd.setCursor(0,0);
  lcd.print("Alarm disarmed");
  lcd.setCursor(0,1);
  lcd.print("To arm press - * ");

  //key to arm the alarm
  if(key == '*')
  {
    armedFlag = true;
  }

if(keyFlag)
  {
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Arming the alarm");
        lcd.setCursor(0,1);
        lcd.print("Pass the code: ");
     
      do
    {

          char key = keypad.getKey();
        if(key == '1' || key == '2' || key == '3' ||
           key == '4' || key == '5' || key == '6' ||
           key == '7' || key == '8' || key == '9' || key == '0')
       {
        Serial.println(key);
           answer += key;
           hiddenchars += '*';
           lcd.setCursor(11, 1);
           lcd.print(hiddenchars);
           }
    else if (key == '*') //key to accept provided passcode
    {

           lcd.clear();
         
          if(answer == password)
      {
             armedFlag = true;
             answer = "";
             hiddenchars = "";
             keyFlag=false;
      }
      else
      {
             answer = "";
             hiddenchars = "";
             lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("Wrong PIN!");
             delay(2000);
             lcd.clear();
             lcd.setCursor(0, 1); 
             lcd.print("Pass the code:"); 
          }
        }
    else if (key == '#') //reset PIN input
    {
            answer = "";
            hiddenchars = "";
            lcd.clear();
            lcd.setCursor(3, 0);
            lcd.print("Uzbrajanie");
            lcd.setCursor(0, 1); 
            lcd.print("Podaj kod:");  
    }
    }while(keyFlag);
    
  }

  while(armedFlag)
  {
    alarmOn(); 
  }
}