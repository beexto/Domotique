/* Dépendances */
#include <VirtualWire.h>
#include <LiquidCrystal.h>
#include <DS3231.h>
const int BROCHE_RELAIS=12;
const byte BACKLIGHT_PWM_PIN = 10;
int temp_SDB=20;
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Time  t;
byte tableau_rx[15];

int activation=0,override=0;
int brightness=251;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int address;
#define ADDRESS_SDB 1
/** Énumération des boutons utilisables */
enum {
  BUTTON_NONE,  /*!< Pas de bouton appuyé */
  BUTTON_PRESS
};
typedef struct heure heure;
struct heure
{
    char hour;
    char min;
    char sec;
};
heure alarm;
heure alarm_stop; 
int menu = 0;
//======================================

///==============================================================
void decodage()
{
  address=tableau_rx[0];
  if (address==ADDRESS_SDB)
  {
    activation=tableau_rx[1]; 
    alarm.hour= tableau_rx[2] ; 
    alarm.min=tableau_rx[3];
    alarm_stop.hour= tableau_rx[4];
    alarm_stop.min=  tableau_rx[5];
    temp_SDB  =tableau_rx[6];  
  rtc.setTime(tableau_rx[7],tableau_rx[8],tableau_rx[9]);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(tableau_rx[11],  tableau_rx[10], tableau_rx[12]+2019);   // Set the date to January 1st, 2014
  brightness=tableau_rx[13];
  }
}
//====================================================
/** Retourne le bouton appuyé (si il y en a un) */
byte getPressedButton()
{
  /* Lit l'état des boutons */
  int value = analogRead(A0);
  if (value < 1010)
  {
     if (override==0)
      {
        override=1;
      }
      else if (override==1)
      {
        override=0;
      }
  }
}


int alarm_activation()// verifie l'heure et passe activation=1 si correspond à l'alarme
{
if (t.hour==alarm.hour)
  {
    if (t.min==alarm.min)
    {
      
      
      activation=1;
      
      
    }
  }
  
 if (t.hour==alarm_stop.hour)
 {
   if (t.min==alarm_stop.min)
   {
     if (t.sec<3)
     {
      activation=0;
     }
   }
 }
 return activation;
 }

int securite()// vérifie que override=0 et décide si activation du relais
{
  if (activation==1)
 {
   if(override==0)
     {
       digitalWrite(BROCHE_RELAIS, HIGH);   // turn the relay on (HIGH is the voltage level)
       Serial.print("allumé \n");
     }
 }
 if (override==1)
 {
   activation=0;
   Serial.print("override 1");
 }
 if (activation==0)
 {
   digitalWrite(BROCHE_RELAIS, LOW);    // turn the relay off by making the voltage LOW
 }
 return 0;
 }
 
 
 int menu_aff()//permet d'afficher les menus
{
  if (menu == 0)
  {
    if (override==1)
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Heure : "));
    lcd.print(rtc.getTimeStr());
    lcd.setCursor(0, 1);
    lcd.print("Arret d'Urgence");
    }
    else if (activation==0)
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Heure : "));
    lcd.print(rtc.getTimeStr());
    lcd.setCursor(0, 1);
    lcd.print("Activation ");
    lcd.print(alarm.hour,DEC);
    lcd.print(":");
    lcd.print(alarm.min,DEC);
    }
    else if (activation==1)
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Heure : "));
    lcd.print(rtc.getTimeStr());
    lcd.setCursor(0, 1);
    lcd.print("Arret ");
    lcd.print(alarm_stop.hour,DEC);
    lcd.print(":");
    lcd.print(alarm_stop.min,DEC);
    }
  }
 
  //permet de loop dans les menus
  if (menu < 0)
  {
    menu = 5;
  }
  if (menu > 5)
  {
    menu = 0;
  }
} 

/** Fonction setup() */
void setup() {
  lcd.begin(16, 2);
  vw_set_rx_pin(13);
  vw_setup(2000);
  vw_rx_start();
  // Setup Serial connection
  Serial.begin(9600);
 
  // Initialize the rtc object
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(17, 18, 50);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(17, 1, 2019);   // Set the date to January 1st, 2014
  alarm.hour=12 ; 
    alarm.min=0;
    alarm_stop.hour=12;
    alarm_stop.min=30;
  pinMode(BROCHE_RELAIS, OUTPUT);
  
}
//====================================
/** Fonction loop() */
void loop() {

 // Get data from the DS3231
  t = rtc.getTime();
  
//Groupe d'activation du relais
 activation=alarm_activation();
//bloc de sécurité
 securite();
//affiche le bon menu
  menu_aff();
 
/* Affiche le bouton appuyé et effectue l'action approprié*/
getPressedButton();
//controle le backlight
if (brightness<10)
digitalWrite(BACKLIGHT_PWM_PIN,LOW);
else
digitalWrite(BACKLIGHT_PWM_PIN,HIGH);

  byte taille_tableau_rx=sizeof(tableau_rx);
  vw_wait_rx_max(200);

   if (vw_get_message(tableau_rx, &taille_tableau_rx)) 
  {
 Serial.println("reception");
  int  t0_RX=0;
   decodage();
  
  }
} 
