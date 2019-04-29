/*
Code de Pierre Cazenave, étudiant GEII A3a
*/
#include <VirtualWire.h>
#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
#include <DS3231.h>
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
char modif=0; //compte le temps depuis la derniere modif
Time  t;
int h_alarm = 12,m_alarm = 0;
int h_alarm_stop = 12, m_alarm_stop = 30;
int h_nuit =5, m_nuit =6;
int h_jour =7, m_jour =8;
int heure_rSDB,minute_rSDB,seconde_rSDB;
int jour_rSDB,mois_rSDB,annee_rSDB;
unsigned long last_time=0;
static int update_interval=1000;
//============================
int temp_SDB=12;
float temp_Int=19;
int temp_Ext=-10;
int temp_SDB_max=0;
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
//============================
int lum_control=1;
int reglage=-1;
int select=0;
int menu=0;
int activation=0;
int t0_RX=0;
int taille_tableau_rx;
//============================
#define PIN_BRIGHTNESS 10
int lum=255;
int nuit=0;
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

//============================

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DS3231  rtc(SDA, SCL);
void setup() {
lcd.init();                      // initialize the lcd 
lcd.backlight();
rtc.begin();
lcd.createChar(0,newChar);
Serial.begin(9600);
//vw_set_tx_pin(6);
//vw_setup(2000);
pinMode(PIN_BTN_UP,INPUT_PULLUP);
pinMode(PIN_BTN_DOWN,INPUT_PULLUP);
pinMode(PIN_BTN_LEFT,INPUT_PULLUP);
pinMode(PIN_BTN_RIGHT,INPUT_PULLUP);
pinMode(PIN_BTN_SELECT,INPUT_PULLUP);
//===================================================
}

void loop() {
recup_RTC();
bouton();
gestion_loop_variable();
menu_reglage();
menu_afficher();
auto_brightness();
COMM_BL();
Serial.println(reglage);
Serial.println(select);
Serial.println(menu);
Serial.println("============");
//delay(200);
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
/* if (value<1000)
 modif=(millis/60)-t0_RX; */

  /* Calcul l'état des boutons */
  if (digitalRead(PIN_BTN_RIGHT)==LOW)
  {
    Serial.println("RIGHT");
    return BUTTON_RIGHT;
  }
  else if (digitalRead(PIN_BTN_LEFT)==LOW)
  {
    Serial.println("LEFT");
    return BUTTON_LEFT;
  }
  else if (digitalRead(PIN_BTN_DOWN)==LOW)
  {
    Serial.println("DOWN");
    return BUTTON_DOWN;
  }
  else if (digitalRead(PIN_BTN_UP)==LOW)
  {
    Serial.println("UP");
    return BUTTON_UP;
  }
 /* else if (digitalRead(PIN_BTN_SELECT)==HIGH)
  {
    Serial.println("SELECT");
    return BUTTON_SELECT;
  }*/
  else
  {
    Serial.println("NONE");
    return BUTTON_NONE;
  }  
}
//================================================fonction qui gere les variables et evite les negatifs et qui loop entre les bonnes valeurs
void gestion_loop_variable()
{
  if (h_alarm<0) {
   h_alarm=23;
  }
  else if(h_alarm>23) {
    h_alarm=0;
  }
 //====================
  if (h_alarm_stop<0) {
   h_alarm_stop=23;
  }
  else if(h_alarm_stop>23) {
    h_alarm_stop=0;
  }
 //====================
  if (h_nuit<0) {
   h_nuit=23;
  }
  else if(h_nuit>23) {
    h_nuit=0;
  }
 //====================
  if (h_jour<0) {
   h_jour=23;
  }
  else if(h_jour>23) {
    h_jour=0;
  }
 //====================
  if (m_alarm<0) {
   m_alarm=59;
  }
  else if(m_alarm>59) {
    m_alarm=0;
  }
 //==================== 
  if (m_alarm_stop<0) {
   m_alarm_stop=59;
  }
  else if(m_alarm_stop>59) {
    m_alarm_stop=0;
  }
  //=====================
  if (m_nuit<0) {
   m_nuit=59;
  }
  else if(m_nuit>59) {
    m_nuit=0;
  }
  //==============
  if (m_nuit<0) {
   m_nuit=59;
  }
  else if(m_nuit>59) {
    m_nuit=0;
  }
  //==============
  if (m_jour<0) {
   m_jour=59;
  }
  else if(m_jour>59) {
    m_jour=0;
  }
   //====================
  if (lum<0) {
   lum=255;
  }
  else if(lum>255) {
    lum=0;
  }
 //====================
 /*if (lum_control<0) {
   lum_control=1;
  }
  else if(lum_control>1) {
    lum_control=0;
  }*/
 //====================
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
          h_alarm++;
        break;

        case Rh_alarm_stop :
          h_alarm_stop++;
        break ;

        case Rh_jour :
          h_jour++;
        break ;

        case Rh_nuit :
          h_nuit++;
        break ;

        case Rm_alarm :
          m_alarm++; 
        break ;

        case Rm_alarm_stop :
          m_alarm_stop++;
        break ;

        case Rm_jour :
          m_jour++;
        break ;

        case Rm_nuit :
          m_nuit++; 
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
          h_alarm--;
        break;

        case Rh_alarm_stop :
          h_alarm_stop--;
        break ;

        case Rh_jour :
          h_jour--;
        break ;

        case Rh_nuit :
          h_nuit--;
        break ;

        case Rm_alarm :
          m_alarm--; 
        break ;

        case Rm_alarm_stop :
          m_alarm_stop--;
        break ;

        case Rm_jour :
          m_jour--;
        break ;

        case Rm_nuit :
          m_nuit--; 
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
  lcd.print(h_alarm_stop);
  lcd.print(F(":"));
  if (m_alarm_stop < 10)
  {
    lcd.print(F("0"));
  }
  lcd.print(m_alarm_stop);
}
//================================== pour afficher le menu 1 part alarm
void afficher_alarm()
{
  lcd.setCursor(0, 1);
  lcd.print(F("Allumage a "));
  lcd.print(h_alarm);
  lcd.print(F(":"));
  if (m_alarm < 10)
  {
    lcd.print(F("0"));
  }
  lcd.print(m_alarm);
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
lcd.print(h_nuit);
lcd.print(F(":"));
if (m_nuit < 10)
        {
          lcd.print(F("0"));
        }
lcd.print(m_nuit);  
}
//================================ pour afficher menu 2 part heure de jour
void afficher_heure_jour()
{
lcd.setCursor(0,2);
lcd.print(F("Heure de jour "));
lcd.print(h_jour);
lcd.print(F(":"));
if (m_jour < 10)
        {
          lcd.print(F("0"));
        }
lcd.print(m_jour);  
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
  
    if (h_jour==t.hour) 
    {
    if (t.min==m_jour)
       analogWrite(PIN_BRIGHTNESS,(255/60*t.sec));
       nuit=0;
    }  

    if (h_nuit==t.hour) 
    {
    if (t.min==m_nuit)
      analogWrite(PIN_BRIGHTNESS,(255-(255/60*t.sec)));
      nuit=1;
    }
    if (nuit==0)
      analogWrite(PIN_BRIGHTNESS,lum);
      
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
      if (h_alarm < 10)
      {
        lcd.print(F(" "));
      }
      lcd.print(h_alarm, DEC);
      lcd.print(F(":"));
      if (m_alarm < 10)
      {
        lcd.print(F("0"));
      }
      lcd.print(m_alarm, DEC);
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
      if (h_alarm_stop < 10)
      {
        lcd.print(F(" "));
      }
      lcd.print(h_alarm_stop, DEC);
      lcd.print(F(":"));
      if (m_alarm_stop < 10)
      {
        lcd.print(F("0"));
      }
      lcd.print(m_alarm_stop, DEC);
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
      lcd.setCursor(8, 3);
      if (temp_Int < 10)
      {
        lcd.setCursor(9, 3);
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
        lcd.print(h_alarm);
        lcd.print(F(":"));
        if (m_alarm < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_alarm);
        afficher_alarm_stop();
        afficher_temp();
      break ;

      case Rm_alarm :
        lcd.setCursor(0, 1);
        lcd.print(F("Allumage a "));
        lcd.print(h_alarm);
        lcd.print(F(":>"));
        if (m_alarm < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_alarm);
        afficher_alarm_stop();
        afficher_temp();
      break ;

      case Rh_alarm_stop :
        afficher_alarm();
        lcd.setCursor(0, 2);
        lcd.print(F("Extinction a>"));
        lcd.print(h_alarm_stop);
        lcd.print(F(":"));
        if (m_alarm_stop < 10)
        {
        lcd.print(F("0"));
        }
        lcd.print(m_alarm_stop);
        afficher_temp();
        break ;

      case Rm_alarm_stop :
        afficher_alarm();
        lcd.setCursor(0, 2);
        lcd.print(F("Extinction a "));
        lcd.print(h_alarm_stop);
        lcd.print(F(":>"));
        if (m_alarm_stop < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_alarm_stop);
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
        lcd.print(h_nuit, DEC);
        lcd.print(F(":"));
        if (m_nuit < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_nuit, DEC);
        afficher_heure_jour();
       //afficher_control_lum();
        afficher_lum();
      break ;
      
      case Rm_nuit :
        lcd.setCursor(0, 1);
        lcd.print(F("Heure de nuit "));
        lcd.print(h_nuit, DEC);
        lcd.print(F(":>"));
        if (m_nuit < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_nuit, DEC);
        afficher_heure_jour();
        //afficher_control_lum();
        afficher_lum();
      break ;
    
    case Rh_jour :
    afficher_heure_nuit();
    lcd.setCursor(0, 2);
        lcd.print(F("Heure de jour>"));
        lcd.print(h_jour, DEC);
        lcd.print(F(":"));
        if (m_jour < 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_jour, DEC);
        //afficher_control_lum();
        afficher_lum();
    break ;
    
    case Rm_jour :
    afficher_heure_nuit();
    lcd.setCursor(0, 2);
        lcd.print(F("Heure de jour "));
        lcd.print(h_jour, DEC);
        lcd.print(F(":>"));
        if (m_jour< 10)
        {
          lcd.print(F("0"));
        }
        lcd.print(m_jour, DEC);
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
//=======================================================================================================
//=======================================================================================================
//PARTIE COMMUNICATION
//=======================================================================================================
//=======================================================================================================
void comm_SDB()
{
 /* t=millis();
  byte tableau_tx[27];
  byte tableau_rx[27];
  taille_tableau_rx=sizeof(tableau_rx)*/
  vw_wait_rx_max(200);
 /*
   if (vw_get_message(tableau_rx, &taille_tableau_rx)) 
  {
 
   t0_RX=0;
   decodage(&tableau_rx);
   update_date(heure_rSDB,minute,seconde);
    update_time(jour,mois,annee); 
  }
  if((t-last_time)>update_interval)
  {
    encodage(&tableau_tx);
    vw_send(tableau_tx, sizeof(tableau_tx)); // On envoie le message
    vw_wait_tx();
  }
   
*/
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
/*void encodage(byte *tableau_tx)
{
  
  t = rtc.getTime();
  *tableau_tx[0]=address;
  *tableau_tx[1]=activation;
  *tableau_tx[2] =h_alarm;
  *tableau_tx[3]=m_alarm;
  *tableau_tx[4]=h_alarm_stop;
  *tableau_tx[5]=m_alarm_stop;
  *tableau_tx[6]=temp_SDB;
  *tableau_tx[7]=t.hour;
  *tableau_tx[8]=t.min;
  *tableau_tx[9]=t.sec;
  *tableau_tx[10]=t.date;
  *tableau_tx[11]=t.mon;
  *tableau_tx[12]=(t.year-2019);
  *tableau_tx[13]=ADDRESSE_SERV;//addresse de l'emetteur
}
//===================================
void decodage(*tableau_rx)
{
  address=*tableau_rx[0];
  if (address==ADDRESSE_SERV)
  {
    activation=*tableau_rx[1]; 
    h_alarm= *tableau_rx[2] ; 
    m_alarm=*tableau_rx[3];
    h_alarm_stop= *tableau_rx[4];
    m_alarm_stop=  *tableau_rx[5];
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

  if (Serial.available()){
    data_in=Serial.read();  //Get next character 

    if(data_in=='U'){ //Button Pressed up h_alarm
      h_alarm++;
    }

    if(data_in=='D'){ //Button Pressed h_alarm
      h_alarm--;
    }

    if(data_in=='u'){ //Button Pressed m_alarm
      m_alarm++;
    }

    if(data_in=='d'){ //Button Pressed m_alarm
      m_alarm--;
    }

     if(data_in=='V'){ //Button Pressed up h_alarm_stop
      h_alarm_stop++;
    }

    if(data_in=='E'){ //Button Pressed h_alarm_stop
      h_alarm_stop--; 
    }

    if(data_in=='v'){ //Button Pressed m_alarm_stop
      m_alarm_stop++;
    }

    if(data_in=='e'){ //Button Pressed m_alarm_stop
      m_alarm_stop--;
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

    if(data_in=='D'){ //Switch On lumiere
      act_lumiere=1;
    }
    if(data_in=='d'){ // Switch Off lumiere
      act_lumiere=0;
    }

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

  }
*/
  /////////////  Send Data to Android device
int text;
  unsigned long t=millis();
  if ((t-last_time)>update_interval)
  {
    last_time=t;

    // Update Text Element 
    text=h_alarm; // <--- Set text to send here 
    Serial.print((String)"*H"+text+"*");

    // Update Text Element 
    text=h_alarm_stop; // <--- Set text to send here 
    Serial.print((String)"*h"+text+"*");

    // Update Text Element 
    text=m_alarm; // <--- Set text to send here 
    Serial.print((String)"*M"+text+"*");

    // Update Text Element 
    text=m_alarm_stop; // <--- Set text to send here 
    Serial.print((String)"*m"+text+"*");

  }

}