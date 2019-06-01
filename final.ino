//Projekt alarmu z wykorzystaniem:
//Arduino UNO, czujnikiem ruchu PIR, buzzera, wyświetlacza LED 16x2, klawiatury 4x4, modułu GSM SIM800L
//Jakub Dolata 22419, Filip Baumgart 22412, CDV 2019, Informatyka niestacjonarna, V semestr, III rok, gr 1, specjalizacja aplikacje WWWw

//biblioteki
#include <LiquidCrystal.h> //Dołączenie bilbioteki
#include <Keypad.h>

//constants
const byte BUZZER = 10; // pin 10 BUZZER
const byte PIRSENSOR = 13;   //pin 13 połączony z sygnałem z czujnika
const byte COUNTDOWN_TIME = 3; //liczba sekund do uzbrojenia alarmu

//variables
bool armedFlag = false;
bool alarmFlag = false;
String answer = ""; //kombinacja podawana przez uzytkownika podczas rozbrajania
String hiddenchars = ""; //przechowuje gwiazdki wyswietlane na ekranie zamiast znakow
String password = "1234"; //kod rozbrajajacy alarm
byte i = 0; //iterator

//ekran LCD
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5); //Informacja o podłączeniu nowego wyświetlacza

//Klawiatura
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {5, 4, 3, 2};  //Piny, do których podłączamy wyprowadzenia od rzędów
byte rowPins[ROWS] = {9, 8, 7, 6}; //Piny, do których kolumn wyprowadzenia od rzędów
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);        //inicjalizacja monitora szeregowego
  pinMode(PIRSENSOR, INPUT);   //ustawienie pinu Arduino jako wejście
  pinMode(BUZZER, OUTPUT);
  Serial.println("---- ALARM ----"); 
//LCD
  lcd.begin(16, 2); //Deklaracja typu
//  lcd.setCursor(0, 0); //Ustawienie kursora
//  lcd.print("Projekt ALARM "); //Wyświetlenie tekstu
//  lcd.setCursor(0, 1); //Ustawienie kursora
//  lcd.print("CDV 2019"); //Wyświetlenie tekstu
}

void alarmOn() {
  for( ; i <= COUNTDOWN_TIME ; i++)
  {
    lcd.clear(); //czyszczenie ekranu
    digitalRead(PIRSENSOR); // włączanie czujnika ruchu
    lcd.setCursor(0, 0); //Ustawienie kursora
    lcd.print("Alarm zostanie");
    lcd.setCursor(0, 1);
    lcd.print("uzbrojony za: ");
    lcd.print(COUNTDOWN_TIME - i); //Wyświetlenie czasu do uzbrojenia alarmu
    delay(500);
    Serial.println(i);
    digitalWrite(BUZZER, HIGH); //pikanie buzzera podczas uzbrajania
    delay(500);
    digitalWrite(BUZZER, LOW);

    if(i == COUNTDOWN_TIME)
	{
      digitalWrite(BUZZER, HIGH);  //ostatnie dłuższe piknięcie buzzera
      delay(900);
      digitalWrite(BUZZER, LOW);
    }
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm uzbrojony"); 
  byte ruch = digitalRead(PIRSENSOR);      //odczytanie wartości z czujnika
  
  if(ruch == HIGH)                      //wyświetlenie informacji na monitorze szeregowym
  {                                     //stan wysoki oznacza wykrycie ruchu, stan niski - brak ruchu
    alarmFlag = true;
    Serial.println("RUCH WYKRYTY!");
    Serial.println(ruch);
    Serial.println("Podaj kod: ");
    lcd.clear();
    lcd.begin(16, 2); 
    lcd.setCursor(3, 0);
    lcd.print("***ALARM***");
    lcd.setCursor(0, 1); 
    lcd.print("Podaj kod:"); 
    
    while(alarmFlag) //jesli wykryto ruch to wykonuj ponizsza funkcje
	{
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
		else if (key == '*') //zatwierdzanie wpisanej kombinacji
		{
           lcd.setCursor(0, 0);
           lcd.print("dziala");
          if(answer == password)
		  {
             answer = "";
			 hiddenchars = "";
             armedFlag = false;
             alarmFlag = false;
             ruch = LOW;
             i = 0;
		  }
		  else
		  {
             answer = "";
			 hiddenchars = "";
			 lcd.clear();
             lcd.setCursor(0, 0);
             lcd.print("Kod niepoprawny!");
			 delay(2000);
             lcd.clear();
             lcd.setCursor(3, 0);
             lcd.print("***ALARM***");
             lcd.setCursor(0, 1); 
             lcd.print("Podaj kod:"); 
          }
        }
		else if (key == '#') //kasowanie znaków
		{
			answer = "";
			hiddenchars = "";
            lcd.clear();
            lcd.setCursor(3, 0);
            lcd.print("***ALARM***");
            lcd.setCursor(0, 1); 
            lcd.print("Podaj kod:");  
		}
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
  byte ruch = digitalRead(PIRSENSOR);      //odczytanie wartości z czujnika

  //lcd
  lcd.setCursor(0,0);
  lcd.print("Alarm rozbrojony");
  lcd.setCursor(0,1);
  lcd.print("Uzbrajanie - * ");

  //przycisk do uzbrajania alarmu
  if(key == '*')
  {
    armedFlag = true;
  }

  while(armedFlag)
  {
    alarmOn(); 
  }
}