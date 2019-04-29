/*

*/
#include <LiquidCrystal.h>
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
DS3231  rtc(SDA, SCL);
Time  t;
int h_alarm = 12;
int m_alarm = 0;
int h_alarm_stop = 12;
int m_alarm_stop = 30;
int h_nuit =5;
int m_nuit =6;
int h_jour =7;
int m_jour =8;
//============================
#define Rh_alarm 1				//reglage et toute ses valeurs possibles
#define Rm_alarm 2
#define Rh_alarm_stop 3
#define Rm_alarm_stop 4
#define Rtemp_max 5
#define Rh_nuit 6
#define Rm_nuit 7
#define Rh_jour 8
#define Rm_jour 9
#define WTF -1
#define R_r -1
int reglage=-1;
int select=0;
int menu=0;
int temp_SDB=12;
int temp_Int=19;
int temp_Ext=-10;
int temp_SDB_max=0;
/** Énumération des boutons utilisables */
enum {
  BUTTON_NONE,  /*!< Pas de bouton appuyé */
  BUTTON_UP,    /*!< Bouton UP (haut) */
  BUTTON_DOWN,  /*!< Bouton DOWN (bas) */
  BUTTON_LEFT,  /*!< Bouton LEFT (gauche) */
  BUTTON_RIGHT, /*!< Bouton RIGHT (droite) */
  BUTTON_SELECT /*!< Bouton SELECT */
};
#define PIN_bouton A0
//============================
int activation=0;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
lcd.begin(20, 4);
rtc.begin();
lcd.createChar(0,newChar);
Serial.begin(9600);
}

void loop() {
t = rtc.getTime();
temp_Int=rtc.getTemp();
bouton();
gestion_loop_variable();
menu_reglage();
menu_afficher();
Serial.println(reglage);
Serial.println(select);
Serial.println(menu);
Serial.println("============");
delay(200);
}
//====================================================================================================== pour savoir l'etat des boutons
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
  int value = analogRead(PIN_bouton);

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
      reglage=Rm_jour;
      else if (reglage>Rm_jour)
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
//================================ pour afficher menu 2 part reglage luminosité
/*void afficher_reglage_lum()
{
lcd.setCursor(0,3);
lcd.print(F("Reglage manuel "));
switch(reglage_lum)
{
	case LUM_manuel
	lcd.print(F("ON"));
	break
	
	case LUM_auto
	lcd.print(F("OFF")
	break
}	
}
*/


//===================================================
/*void update_time(int h, int m, int s)
{
  rtc.setTime(h, m, s);
}
//===================================================
void update_date(int j, int m, int a)
{
  rtc.setDate(j, m, a);   // Set the date to January 1st, 2014
}
//===================================================
int update_brightness()
{
  if ()//change selon l'heure
  return lum;
  }
  */
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
	//	afficher_reglage_lum();
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
		 //   afficher_reglage_lum();
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
	//	afficher_reglage_lum();
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
	//	afficher_reglage_lum();
		break ;
			

    }
  }
   /* else if (menu == WTF)===================== a faire
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(F("J'ai froid remets"));
    lcd.setCursor(0, 2);						
    lcd.print(F("le chauffage"));
  }
*///========================================
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


