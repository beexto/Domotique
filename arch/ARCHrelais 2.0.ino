
/* Dépendances */
#include <LiquidCrystal.h>
#include <DS3231.h>
const int BROCHE_RELAIS=12;
const byte BACKLIGHT_PWM_PIN = 10;
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Time  t;
int h_alarm = 12;
int m_alarm = 0;
int h_alarm_stop = 12;
int m_alarm_stop = 30;
int activation=0,override=0;
int brightness=251;
/** Objet LiquidCrystal pour communication avec l'écran LCD */
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


/** Énumération des boutons utilisables */
enum {
  BUTTON_NONE,  /*!< Pas de bouton appuyé */
  BUTTON_UP,    /*!< Bouton UP (haut) */
  BUTTON_DOWN,  /*!< Bouton DOWN (bas) */
  BUTTON_LEFT,  /*!< Bouton LEFT (gauche) */
  BUTTON_RIGHT, /*!< Bouton RIGHT (droite) */
  BUTTON_SELECT /*!< Bouton SELECT */
};

int menu = 0;
/** Fonction setup() */
void setup() {
  lcd.begin(16, 2);
  
  // Setup Serial connection
  Serial.begin(115200);
  
  // Initialize the rtc object
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(17, 18, 50);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(17, 1, 2019);   // Set the date to January 1st, 2014
  
  
  pinMode(BROCHE_RELAIS, OUTPUT);
  
}

/** Fonction loop() */
void loop() {

 // Get data from the DS3231
  t = rtc.getTime();
  /*Serial.print(t.hour, DEC);
  Serial.print(" hour(s), ");
  Serial.print(t.min, DEC);
  Serial.print(" minute(s) and ");
  Serial.print(t.sec, DEC);*/
  
//Groupe d'activation du relais
 activation=alarm_activation();
//bloc de sécurité
 securite();
//affiche le bon menu
  menu_aff();
 
/* Affiche le bouton appuyé et effectue l'action approprié*/
 bouton();
//controle le backlight
backlight();
/* Délai pour l'affichage */
  delay(100);
}

/** Retourne le bouton appuyé (si il y en a un) */
byte getPressedButton()
{

  /* Lit l'état des boutons */
  int value = analogRead(A0);

  /* Calcul l'état des boutons */
  if (value < 99)
    return BUTTON_RIGHT;
  else if (value < 255)
    return BUTTON_UP;
  else if (value < 409)
    return BUTTON_DOWN;
  else if (value < 639)
    return BUTTON_LEFT;
  else if (value < 1010)
    return BUTTON_SELECT;
  else
    return BUTTON_NONE;
  
}


int alarm_activation()// verifie l'heure et passe activation=1 si correspond à l'alarme
{
if (t.hour==h_alarm)
  {
    if (t.min==m_alarm)
    {
      
      
      activation=1;
      
      
    }
  }
  
 if (t.hour==h_alarm_stop)
 {
   if (t.min==m_alarm_stop)
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
    lcd.print(t.hour,DEC);
    lcd.print(":");
    lcd.print(t.min,DEC);
    lcd.print(":");
    lcd.print(t.sec,DEC);
    lcd.setCursor(0, 1);
    lcd.print("Arret d'Urgence");
    }
    else if (activation==0)
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Heure : "));
    lcd.print(t.hour,DEC);
    lcd.print(":");
    lcd.print(t.min,DEC);
    lcd.print(":");
    lcd.print(t.sec,DEC);
    lcd.setCursor(0, 1);
    lcd.print("Activation ");
    lcd.print(h_alarm,DEC);
    lcd.print(":");
    lcd.print(m_alarm,DEC);
    }
    else if (activation==1)
    {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Heure : "));
    lcd.print(t.hour,DEC);
    lcd.print(":");
    lcd.print(t.min,DEC);
    lcd.print(":");
    lcd.print(t.sec,DEC);
    lcd.setCursor(0, 1);
    lcd.print("Arret ");
    lcd.print(h_alarm_stop,DEC);
    lcd.print(":");
    lcd.print(m_alarm_stop,DEC);
    }
  }
  else if (menu == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("   Activation   "));
    lcd.setCursor(0, 1);
    lcd.print("    >");
    lcd.print(h_alarm,DEC);
    lcd.print(":");
    lcd.print(m_alarm,DEC);
    
  }
  else if (menu == 2)
  {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("   Activation   "));
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(h_alarm,DEC);
    lcd.print(":>");
    lcd.print(m_alarm,DEC);
    
  }
  else if (menu == 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("     Arret      "));
    lcd.setCursor(0, 1);
    lcd.print("    >");
    lcd.print(h_alarm_stop,DEC);
    lcd.print(":");
    lcd.print(m_alarm_stop,DEC);
    
  }
  else if (menu == 4)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("     Arret      "));
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(h_alarm_stop,DEC);
    lcd.print(":>");
    lcd.print(m_alarm_stop,DEC);
    
  }
  else if (menu==5)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("   Luminosite"));
    lcd.setCursor(0, 1);
    lcd.print("      >");
    lcd.print(brightness,DEC);
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


int bouton()//permet de savoir quel bouton a été appuyé et effetue l'action approprié
{
  switch (getPressedButton())
  {
    
    case BUTTON_UP:
     if (menu==5)
     {
       brightness=brightness+5;
     }
        else if (menu==4)
        {
         m_alarm_stop=m_alarm_stop+1;
        if (m_alarm_stop>59)
        {
         m_alarm_stop=0;
        }
      }
      else if (menu==3)
      {
        h_alarm_stop=h_alarm_stop+1;
        if (h_alarm_stop>23)
        {
          h_alarm_stop=0;
        }
      }
        else if (menu==2)
        {
          m_alarm=m_alarm+1;
          if (m_alarm>59)
          {
            m_alarm=0;
          }
        }
          else if (menu==1)
          {
            h_alarm=h_alarm+1;
            if (h_alarm>23)
            {
              h_alarm=0;
            }
          }
          
      
      
      break;

    case BUTTON_DOWN:
     if (menu==5)
          {
            brightness=brightness-5;
          }
      if (menu==4)
      {
        m_alarm_stop=m_alarm_stop-1;
        if (m_alarm_stop<0)
        {
         m_alarm_stop=59;
        }
      }
      else if (menu==3)
      {
        h_alarm_stop=h_alarm_stop-1;
        if (h_alarm_stop<0)
        {
          h_alarm_stop=23;
        }
      }
        else if (menu==2)
        {
          m_alarm=m_alarm-1;
          if (m_alarm<0)
          {
            m_alarm=59;
          }
        }
          else if (menu==1)
          {
            h_alarm=h_alarm-1;
            if (h_alarm<0)
            {
              h_alarm=23;
            }
          }
          
        
    
      break;

    case BUTTON_LEFT:
      menu = menu - 1;
      break;

    case BUTTON_RIGHT:
      menu = menu + 1;
      break;
    
    case BUTTON_SELECT:
      if (override==0)
      {
        override=1;
      }
      else if (override==1)
      {
        override=0;
      }
      break;
    
 }
}


int backlight()
{
 
  analogWrite(BACKLIGHT_PWM_PIN, brightness);
  if (brightness>255)
  {
    brightness=0;
  }
  
  if (brightness<0)
  {
    brightness=255;
  }
}