/*
Code de Pierre Cazenave, étudiant GEII A3a
*/
#include <VirtualWire.h>
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <SoftwareSerial.h>                                // appel de la bibliothèque
SoftwareSerial HC06(13, 12);                                   // RX, TX
//========================creer craractere degree
byte newChar[8]={
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
//================== variable de temps
unsigned long modif=0; //compte le temps depuis la derniere modif
Time  t;
unsigned long t0_TX;
unsigned long t_actual;
static unsigned char update_interval_TX=500;
int jour_rSDB,mois_rSDB,annee_rSDB;
unsigned long last_time=0;
static int update_interval=500;
typedef struct heure heure;
struct heure
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
};
heure alarm;
heure alarm_stop;
heure jour;
heure nuit;
heure RX_SDB;
//============================
byte tableau_tx[15];
//============================
int temp_SDB=12;
float temp_Int=19;
int temp_Ext=-10;
int temp_SDB_max=40;
//============================
#define Rh_alarm 1        //reglage et toute ses valeurs possibles
#define Rm_alarm 2
#define Rh_alarm_stop 3
#define Rm_alarm_stop 4
#define Rtemp_max 5
#define Rh_nuit 6
#define Rm_nuit 7
#define Rh_jour 8
#define Rm_jour 9
#define Rlum_control 11
#define Rlum 10
#define WTF -5
#define R_r -1
#define MANUEL 0
#define AUTO 1
#define ADDRESSE_SDB 1
//============================
int lum_control=1;
int reglage=-1;
int select=0;
int menu=0;
int activation=0;
int t0_RX=0;
unsigned long T1_loop=0;
//============================
#define PIN_BRIGHTNESS 5
int lum=255;
int nuitencours=0;
//============================
/** Énumération des boutons utilisables */
enum {
  BUTTON_NONE,  /*!< Pas de bouton appuyé */
  BUTTON_UP,    /*!< Bouton UP (haut) */
  BUTTON_DOWN,  /*!< Bouton DOWN (bas) */
  BUTTON_LEFT,  /*!< Bouton LEFT (gauche) */
  BUTTON_RIGHT, /*!< Bouton RIGHT (droite) */
  BUTTON_SELECT /*!< Bouton SELECT */
};
#define PIN_BTN_UP 5
#define PIN_BTN_DOWN 8
#define PIN_BTN_LEFT 4
#define PIN_BTN_RIGHT 6
#define PIN_BTN_SELECT 11
#define PIN_LUMIERE 2
//============================

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DS3231  rtc(SDA, SCL);
void setup() {
lcd.init();                      // initialize the lcd 
lcd.backlight();
rtc.begin();
lcd.createChar(0,newChar);
Serial.begin(9600);
HC06.begin(9600);
vw_set_tx_pin(10);
vw_setup(1000);
pinMode(PIN_BTN_UP,INPUT_PULLUP);
pinMode(PIN_BTN_DOWN,INPUT_PULLUP);
pinMode(PIN_BTN_LEFT,INPUT_PULLUP);
pinMode(PIN_BTN_RIGHT,INPUT_PULLUP);
pinMode(PIN_BTN_SELECT,INPUT_PULLUP);
pinMode(PIN_LUMIERE,OUTPUT);
digitalWrite(PIN_LUMIERE,HIGH);
//===================================================
alarm.hour = 13;
alarm.min = 1;
alarm_stop.hour = 22;
alarm_stop.min = 32;
nuit.hour = 3;
nuit.min = 18;
jour.hour = 7;
jour.min = 8;
}

void loop() {
unsigned long T0_loop=millis();
recup_RTC();
alarm_activation_SDB();
bouton();
gestion_loop_variable();
menu_reglage();
if (T0_loop-T1_loop>200)
{
  menu_afficher();
  T1_loop=millis();
}
auto_brightness();
COMM_BL();
Serial.println(reglage);
Serial.println(select);
Serial.println(menu);
Serial.println("============");
comm_SDB();
}
//============================================================== Prends les variables du RTC(temp,heure)
void recup_RTC()
{
  t = rtc.getTime();
  temp_Int=rtc.getTemp();
}
//======================================================================= pour savoir l'etat des boutons
byte getPressedButton()
{
  /*pin vers le haut
1023=rien
716=SELECT
465=droite
281=bas
133=haut
0=gauche

Lit l'état des boutons */
  int value = analogRead(A0);
Serial.println(value);
  /* Calcul l'état des boutons */
  if (value < 133)
    return BUTTON_LEFT;
  else if (value < 282)
    return BUTTON_UP;
  else if (value < 465)
    return BUTTON_DOWN;
  else if (value < 716)
    return BUTTON_RIGHT;
  else if (value < 1000)
    return BUTTON_SELECT;
  else
    return BUTTON_NONE;
  
}
//================================================fonction qui gere les variables et evite les negatifs et qui loop entre les bonnes valeurs
void gestion_loop_variable()
{
  if (alarm.hour<0) {
   alarm.hour=23;
  }
  else if(alarm.hour>23) {
    alarm.hour=0;
  }
 //====================
  if (alarm_stop.hour<0) {
   alarm_stop.hour=23;
  }
  else if(alarm_stop.hour>23) {
    alarm_stop.hour=0;
  }
 //====================
 //====================
  if (alarm.min<0) {
   alarm.min=59;
  }
  else if(alarm.min>59) {
    alarm.min=0;
  }
 //==================== 
  if (alarm_stop.min<0) {
   alarm_stop.min=59;
  }
  else if(alarm_stop.min>59) {
    alarm_stop.min=0;
  }
  //=====================
  //====================
  if (lum<0) {
   lum=255;
  }
  else if(lum>255) {
    lum=0;
  }
}
//=======================================================================fonction qui gere le rapport entre menu et reglage suivant SELECT
void menu_reglage()
{
  switch (menu)
  {
    case 0:
      reglage=R_r;
    break;
  
    case 1 :
      if (reglage==R_r)
        reglage=Rh_alarm;
      else if (reglage<Rh_alarm)
          reglage=Rtemp_max;
      else if (reglage>Rtemp_max)
          reglage=Rh_alarm;
      if (select==0)
        reglage=R_r;
    break ;

    case 2 : 
     if (reglage==R_r)
        reglage=Rh_nuit;
      else if (reglage<Rh_nuit)
          reglage=Rlum;
      else if (reglage>Rlum)
          reglage=Rh_nuit;
      if (select==0)
        reglage=R_r;
    break ;
  
  }
}  
//==================================pour decider quoi faire selon menu et bouton pressé
void bouton()
{
  switch(getPressedButton())
  {
    case BUTTON_LEFT :
    switch(select)
    {
      case 0 :
        menu--;
      break;

      case 1 :
        reglage--;
      break ;

    }
    
    break ;
    
    case BUTTON_RIGHT :
    switch(select)
    {
      case 0 :
        menu++;
      break;

      case 1 :
        reglage++;
      break ;

    }
    break ;
    
    case BUTTON_UP :
    if (select==1)
    {
      switch(reglage)
      {
        case Rh_alarm :
          alarm.hour++;
        break;

        case Rh_alarm_stop :
          alarm_stop.hour++;
        break ;

        case Rh_jour :
          jour.hour++;
        break ;

        case Rh_nuit :
          nuit.hour++;
        break ;

        case Rm_alarm :
          alarm.min++; 
        break ;

        case Rm_alarm_stop :
          alarm_stop.min++;
        break ;

        case Rm_jour :
          jour.min++;
        break ;

        case Rm_nuit :
          nuit.min++; 
        break ;

        case Rtemp_max :
          temp_SDB_max++; 
        break ;
        
        case Rlum :
          lum=lum+5; 
        break ;
        
        case Rlum_control :
          lum_control++; 
        break ;
      }
    }
    break ;
    
    case BUTTON_DOWN :
   if (select==1)
    {
      switch(reglage)
      {
        case Rh_alarm :
          alarm.hour--;
        break;

        case Rh_alarm_stop :
          alarm_stop.hour--;
        break ;

        case Rh_jour :
          jour.hour--;
        break ;

        case Rh_nuit :
          nuit.hour--;
        break ;

        case Rm_alarm :
          alarm.min--; 
        break ;

        case Rm_alarm_stop :
          alarm_stop.min--;
        break ;

        case Rm_jour :
          jour.min--;
        break ;

        case Rm_nuit :
          nuit.min--; 
        break ;

        case Rtemp_max :
          temp_SDB_max--; 
        break ;
        
        case Rlum :
          lum=lum-5; 
        break ;
        
        case Rlum_control :
          lum_control--; 
        break ;
      }
    }
    break ;
    
    case BUTTON_SELECT :
    switch(select)
    {
      case 0 :
        select=1;
      break ;

      case 1 :
        select=0;
      break ;
    }
    break ;
  }
}
//================================== pour afficher le menu 1 part alarm_stop
void afficher_alarm_stop()
{
  lcd.setCursor(0, 2);
  lcd.print(F("Extinction a "));
  lcd.print(alarm_stop.hour,DEC);
  lcd.print(F(":"));
  if (alarm_stop.min < 10)
  {
    lcd.print(F("0"));
  }
  lcd.print(alarm_stop.min,DEC);
}
//================================== pour afficher le menu 1 part alarm
void afficher_alarm()
{
  lcd.setCursor(0, 1);
  lcd.print(F("Allumage a "));
  lcd.print(alarm.hour,DEC);
  lcd.print(F(":"));
  if (alarm.min < 10)
  {
    lcd.print(F("0"));
  }
  lcd.print(alarm.min,DEC);
}
//================================== pour afficher le menu 1 part alarm
void afficher_temp()
{
  lcd.setCursor(0, 3);
  lcd.print(F("Temp de seuil "));
  lcd.print(temp_SDB_max);
   lcd.write((byte)0);
  lcd.print(F("C"));

}
//================================ pour afficher menu 2 part heure de nuit
void afficher_heure_nuit()
{
lcd.setCursor(0,1);
lcd.print(F("Heure de nuit "));
lcd.print(nuit.hour,DEC);
lcd.print(F(":"));
if (nuit.min < 10)
        {
          lcd.print(F("0"));
        }
lcd.print(nuit.min,DEC);  
}
//================================ pour afficher menu 2 part heure de jour
void afficher_heure_jour()
{
lcd.setCursor(0,2);
lcd.print(F("Heure de jour "));
lcd.print(jour.hour,DEC);
lcd.print(F(":"));
if (jour.min < 10)
        {
          lcd.print(F("0"));
        }
lcd.print(jour.min,DEC);  
}

//===================================================
void afficher_lum()
{
  lcd.setCursor(0,3);
  lcd.print(F("Luminositee "));
  lcd.print(lum);
}
//======================================================= pour afficher menu 2 part reglage luminosité
/* void afficher_control_lum()
{
lcd.setCursor(0,3);
lcd.print(F("Reglage manuel "));
switch(lum_control)
{
  case MANUEL :
  lcd.print(F("ON"));
  break ;
  
  case AUTO :
  lcd.print(F("OFF"));
  break ;
} 
} */
//===================================================change selon l'heure
void auto_brightness()
{
  if (t.hour == jour.hour)
  {
   Serial.println("jour") ;
    if (t.min == jour.min)
    {
      //analogWrite(PIN_BRIGHTNESS,(255/60*t.sec));
      lum = ((250 / 60 * t.sec) + 5);
    }
    else
    {
      lum = 255;
    }
  }

  if (t.hour == nuit.hour)
  {
    Serial.println("nuit");
    if (t.min == nuit.min)
    {
      //analogWrite(PIN_BRIGHTNESS,(255-(255/60*t.sec)));
      lum = (250 - (250 / 60 * t.sec) + 5);
    }
    else
    {
      lum = 5;  
    }
  }
 
  analogWrite(PIN_BRIGHTNESS, lum);
      
}
//==================================================
/*void brightness()
{
  if(lum_control==AUTO)
  auto_brightness();
}*/
//==================================================
int menu_afficher()//permet d'afficher les menus
{
  if (menu == 0)
  {
    if (activation == 0) //si chauffage SDB eteint
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(rtc.getTimeStr());
      lcd.print(F("  "));
      lcd.print(rtc.getDateStr());
      //================ premiere ligne
      lcd.setCursor(0, 1);
      lcd.print(F("Allumage a "));
      if (alarm.hour < 10)
      {
        lcd.print(F(" "));
      }
      lcd.print(alarm.hour, DEC);
      lcd.print(F(":"));
      if (alarm.min < 10)
      {
        lcd.print(F("0"));
      }
      lcd.print(alarm.min, DEC);
      //================= deuxieme ligne
      lcd.setCursor(0, 2);
      lcd.print(F("SDB:   Int:    Ext:"));
      //=================troisieme ligne
      lcd.setCursor(0, 3);
      if (temp_SDB < 10)
      {
        lcd.setCursor(1, 3);
      }
      lcd.print(temp_SDB);
      lcd.write((byte)0);
      lcd.print(F("C"));
      lcd.setCursor(6, 3);
      if (temp_Int < 10)
      {
        lcd.setCursor(7, 3);
      }
      lcd.print(temp_Int);
       lcd.write((byte)0);
      lcd.print(F("C"));
      lcd.setCursor(16, 3);
      if (temp_Ext < 10 && temp_Ext >= 0)
      {
        lcd.setCursor(17, 3);
      }
        else if(temp_Ext<0)
        {
          lcd.setCursor(15, 3);
        }
     
      lcd.print(temp_Ext);
       lcd.write((byte)0);
      lcd.print(F("C"));
    }
    else if (activation == 1) //si chauffage allumé SDB
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(rtc.getTimeStr());
      lcd.print(F("  "));
      lcd.print(rtc.getDateStr());
      //================ premiere ligne
      lcd.setCursor(0, 1);
      lcd.print(F("Extinction a "));
      if (alarm_stop.hour < 10)
      {
        lcd.print(F(" "));
      }
      lcd.print(alarm_stop.hour, DEC);
      lcd.print(F(":"));
      if (alarm_stop.min < 10)
      {
        lcd.print(F("0"));
      }
      lcd.print(alarm_stop.min, DEC);
      //================= deuxieme ligne
      lcd.setCursor(0, 2);
      lcd.print(F("SDB:   Int:   Ext:"));
      //=================troisieme ligne
      lcd.setCursor(1, 3);
      if (temp_SDB < 10)
      {
        lcd.setCursor(2, 3);
      }
      lcd.print(temp_SDB);
      lcd.write((byte)0);
      lcd.print(F("C"));
      lcd.setCursor(6, 3);
      if (temp_Int < 10)
      {
        lcd.setCursor(7, 3);
      }
      lcd.print(temp_Int);
      lcd.write((byte)0);
      lcd.print(F("C"));
      lcd.setCursor(16, 3);
      if (temp_Ext < 10 && temp_Ext >= 0)
      {
        lcd.setCursor(17, 3);
      }
        else if(temp_Ext<0)
        {
          lcd.setCursor(15,3);
        }
      lcd.print(temp_Ext);
      lcd.write((byte)0);
      lcd.print(F("C"));
      //===============================quatrieme ligne
    }
    else if (activation == -1) //si perte de connexion SDB depuis plusieurs cycles
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(rtc.getTimeStr());
      lcd.print(F(" "));
      lcd.print(rtc.getDateStr());
      //================ premiere ligne
      lcd.setCursor(0, 1);
      lcd.print(F("Erreur de comm SDB "));
      //================= deuxieme ligne
      lcd.setCursor(0, 2);
      lcd.print(F("SDB:   Int:   Ext:"));
      //=================troisieme ligne
      lcd.setCursor(1, 3);
      if (temp_SDB < 10)
      {
        lcd.setCursor(2, 3);
      }
      lcd.print(temp_SDB);
      lcd.setCursor(8, 3);
      if (temp_Int < 10)
      {
        lcd.setCursor(9, 3);
      }
      lcd.print(temp_Int);
      lcd.setCursor(16, 3);
      if (temp_Ext < 10 && temp_Ext >= 0)
      {
        lcd.setCursor(17, 3);
      }
      lcd.print(temp_Ext);
      //===============================quatrieme ligne
    }
  }
  else if (menu == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("    Reglages SDB"));
    switch (reglage)
    {
      case R_r :
        afficher_alarm();
        afficher_alarm_stop();
        afficher_temp();
      break ;

      case Rh_alarm :
        lcd.setCursor(0,1);
        lcd.print(F("Allumage a >"));
        lcd.print(alarm.hour,DEC);
        lcd.print(F(":"));
        if (alarm.min < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(alarm.min,DEC);
        afficher_alarm_stop();
        afficher_temp();
      break ;

      case Rm_alarm :
        lcd.setCursor(0, 1);
        lcd.print(F("Allumage a "));
        lcd.print(alarm.hour,DEC);
        lcd.print(F(":>"));
        if (alarm.min < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(alarm.min,DEC);
        afficher_alarm_stop();
        afficher_temp();
      break ;

      case Rh_alarm_stop :
        afficher_alarm();
        lcd.setCursor(0, 2);
        lcd.print(F("Extinction a>"));
        lcd.print(alarm_stop.hour,DEC);
        lcd.print(F(":"));
        if (alarm_stop.min < 10)
        {
        lcd.print(F("0"));
        }
        lcd.print(alarm_stop.min,DEC);
        afficher_temp();
        break ;

      case Rm_alarm_stop :
        afficher_alarm();
        lcd.setCursor(0, 2);
        lcd.print(F("Extinction a "));
        lcd.print(alarm_stop.hour,DEC);
        lcd.print(F(":>"));
        if (alarm_stop.min < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(alarm_stop.min,DEC);
        afficher_temp();
      break ;
      
      case Rtemp_max :
        afficher_alarm();
        afficher_alarm_stop();
        lcd.setCursor(0, 3);
        lcd.print(F("Temp de seuil >"));
        lcd.print(temp_SDB_max);
        lcd.write((byte)0);
        lcd.print(F("C"));

        
      break ;
    }
  }
  else if (menu == 2)
  {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("  Retro-eclairage"));
   
    switch(reglage)
    {
      case R_r :
        afficher_heure_nuit();
        afficher_heure_jour();
        //afficher_control_lum();
        afficher_lum();
      break ;
      case Rh_nuit :
        lcd.setCursor(0, 1);
        lcd.print(F("Heure de nuit>"));
        lcd.print(nuit.hour, DEC);
        lcd.print(F(":"));
        if (nuit.min < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(nuit.min, DEC);
        afficher_heure_jour();
       //afficher_control_lum();
        afficher_lum();
      break ;
      
      case Rm_nuit :
        lcd.setCursor(0, 1);
        lcd.print(F("Heure de nuit "));
        lcd.print(nuit.hour, DEC);
        lcd.print(F(":>"));
        if (nuit.min < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(nuit.min, DEC);
        afficher_heure_jour();
        //afficher_control_lum();
        afficher_lum();
      break ;
    
    case Rh_jour :
    afficher_heure_nuit();
    lcd.setCursor(0, 2);
        lcd.print(F("Heure de jour>"));
        lcd.print(jour.hour, DEC);
        lcd.print(F(":"));
        if (jour.min < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(jour.min, DEC);
        //afficher_control_lum();
        afficher_lum();
    break ;
    
    case Rm_jour :
    afficher_heure_nuit();
    lcd.setCursor(0, 2);
        lcd.print(F("Heure de jour "));
        lcd.print(jour.hour, DEC);
        lcd.print(F(":>"));
        if (jour.min< 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(jour.min, DEC);
        //afficher_control_lum();
        afficher_lum();
    break ;
      
    case Rlum :
    afficher_heure_nuit();
    afficher_heure_jour();
    //afficher_control_lum();
    lcd.setCursor(0,3);
    lcd.print(F("luminositee > "));
    lcd.print(lum);
    break ;
    
    /*case Rlum_control :
    afficher_heure_nuit();
    afficher_heure_jour();
      lcd.setCursor(0,3);
      lcd.print(F("Reglage manuel >"));
        switch(lum_control)
        {
          case MANUEL :
          lcd.print(F("ON"));
          break ;
          
          case AUTO :
          lcd.print(F("OFF"));
          break ;
        }
    afficher_lum();
    break ;*/

    }
  }
    else if (menu == WTF)
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(F("J'ai froid remets"));
    lcd.setCursor(0, 2);            
    lcd.print(F("le chauffage"));
    lcd.setCursor(0, 3);            
    lcd.print(F("on se les geles"));
  }
//========================================
  //permet de loop dans les menus
  if (menu < 0)
  {
    menu = 2;
  }
  if (menu > 2)
  {
    menu = 0;
  }
}
//===================calcul de si sdb allumé
void alarm_activation_SDB()// verifie l'heure et passe activation=1 si correspond à l'alarme
{
if (t.hour==alarm.hour)
  {
    if (t.min==alarm.min)
    {
      
      
      activation=1;
      
      
    }
  }
  
 if (t.hour=alarm_stop.hour)
 {
   if (t.min==alarm_stop.min)
   {
     if (t.sec<3)
     {
      activation=0;
     }
   }
 }
 }

//=======================================================================================================
//=======================================================================================================
//PARTIE COMMUNICATION
//=======================================================================================================
//=======================================================================================================
void comm_SDB()
{
  unsigned long t_avant=millis();
  unsigned long t_actual=millis();
 /* byte tableau_rx[27];
  taille_tableau_rx=sizeof(tableau_rx)*/
  //vw_wait_rx_max(200);
 //delay(200);
  /* if (vw_get_message(tableau_rx, &taille_tableau_rx)) 
  {
 
   t0_RX=0;
   decodage(&tableau_rx);
   update_date(RX_SDB.hour,minute,seconde);
    update_time(jour,mois,annee); 
  }*/
  //if((t_actual-t0_TX)>update_interval_TX)
 /// {
   
   
   //t_actual=millis();
    encodage();
    
    //vw_send(tableau_tx, sizeof(tableau_tx)); // On envoie le message
    //vw_wait_tx();
    Serial.println("envoye");
    //t0_TX=millis();
   
 // }
   

}

//===================================================mets a jour l'heure par moyenne avec celle reçu par
/*void update_time(int h, int m, int s)           // l'autre RTC (SDB)
{
  rtc.setTime(((h+t.hour)/2),((m+t.min)/2) ,((s+t.sec)/2));
}
//===================================================met a jour la date par moyenne de celle reçu par 
void update_date(int j, int m, int a)       // l'autre module RTC (celui de SDB)
{
  rtc.setDate(((j+t.date)/2),((m+t.mon)/2), ((a+t.year)/2)));   // Set the date to January 1st, 2014
}*/
//==============================
void encodage()
{
  Serial.println("encodage");
  t = rtc.getTime();
  tableau_tx[0]=ADDRESSE_SDB;
  tableau_tx[1]=activation;
  tableau_tx[2] =alarm.hour;
  tableau_tx[3]=alarm.min;
  tableau_tx[4]=alarm_stop.hour;
  tableau_tx[5]=alarm_stop.min;
  tableau_tx[6]=temp_SDB_max;
  tableau_tx[7]=t.hour;
  tableau_tx[8]=t.min;
  tableau_tx[9]=t.sec;
  tableau_tx[10]=t.date;
  tableau_tx[11]=t.mon;
  tableau_tx[12]=(t.year-2019);
  tableau_tx[13]=lum;
  //tableau_tx[14]=ADDRESSE_SERV;//addresse de l'emetteur
}/*
//===================================
void decodage(*tableau_rx)
{
  address=*tableau_rx[0];
  if (address==ADDRESSE_SERV)
  {
    activation=*tableau_rx[1]; 
    alarm.hour= *tableau_rx[2] ; 
    alarm.min=*tableau_rx[3];
    alarm_stop.hour= *tableau_rx[4];
    alarm_stop.min=  *tableau_rx[5];
    temp_SDB  =*tableau_rx[6];
    h_sdb=*tableau_rx[7];
    m_sdb=*tableau_rx[8];
    s_sdb=*tableau_rx[9];
    day_sdb= *tableau_rx[10];
    month=*tableau_rx[11];
    a= *tableau_rx[12]+2019;
    addresse_emetteur=*tableau_rx[13];
  }
}*/

void COMM_BL()
{
  int data_in;
  /////////////   Receive and Process Data

  if (HC06.available()){
    data_in=HC06.read();  //Get next character 

    if(data_in=='U'){ //Button Pressed up alarm.hour
      alarm.hour++;
    }

    if(data_in=='D'){ //Button Pressed alarm.hour
      alarm.hour--;
    }

    if(data_in=='u'){ //Button Pressed alarm.min
      alarm.min++;
    }

    if(data_in=='d'){ //Button Pressed alarm.min
      alarm.min--;
    }

     if(data_in=='V'){ //Button Pressed up alarm_stop.hour
      alarm_stop.hour++;
    }

    if(data_in=='E'){ //Button Pressed alarm_stop.hour
      alarm_stop.hour--; 
    }

    if(data_in=='v'){ //Button Pressed alarm_stop.min
      alarm_stop.min++;
    }

    if(data_in=='e'){ //Button Pressed alarm_stop.min
      alarm_stop.min--;
    }
  }
////////////////////////////////////////////////////////////
 /*   if(data_in=='C'){ //Switch On led lit
      modif_led_lit=1;
      modif_led_volet=0;
    }
    if(data_in=='c'){ // Switch on led volet 
      modif_led_lit=0;
      modif_led_volet=1;
    }

    if(data_in=='B'){ //Switch On led volet
      act_led_volet=1; 
    }
    if(data_in=='b'){ // Switch Off led volet
      act_led_volet=0;
    }

    if(data_in=='A'){ //Switch On led lit
      act_led_lit=1;
    }
    if(data_in=='a'){ // Switch Off led lit
      act_led_lit=0;
    }
*/
    if(data_in=='W'){ //Switch On lumiere
      digitalWrite(PIN_LUMIERE,LOW);
    }
    if(data_in=='w'){ // Switch Off lumiere
     digitalWrite(PIN_LUMIERE,HIGH);
    }
/*
    if(data_in=='R'){ //  Slider
      slider_value=Serial.parseInt();
      //<--- Perhaps do something with slider_value here
    }

    if(data_in=='B'){ //  Slider
      slider_value=Serial.parseInt();
      //<--- Perhaps do something with slider_value here
    }

    if(data_in=='G'){ //  Slider
      slider_value=Serial.parseInt();
      //<--- Perhaps do something with slider_value here
    }

    */if(data_in=='j'){ //  Slider
          lum=HC06.parseInt();
          
    }
  //}

  /////////////  Send Data to Android device
int text;
  unsigned long t=millis();
  if ((t-last_time)>update_interval)
  {
    last_time=t;

    // Update Text Element 
    text=alarm.hour; // <--- Set text to send here 
    HC06.print((String)"*H"+text+"*");

    // Update Text Element 
    text=alarm_stop.hour; // <--- Set text to send here 
    HC06.print((String)"*h"+text+"*");

    // Update Text Element 
    text=alarm.min; // <--- Set text to send here 
    HC06.print((String)"*M"+text+"*");

    // Update Text Element 
    text=alarm_stop.min; // <--- Set text to send here 
    HC06.print((String)"*m"+text+"*");

  }

}