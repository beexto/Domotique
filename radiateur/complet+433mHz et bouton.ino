#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>
#include <IRremote.h>
//============================================================ definition PIN
#define PIN_RELAIS1 2
#define PIN_RELAIS2 3
#define PIN_TEMP A0
#define PIN_LUM 5
#define R_r 0
#define ADDRESS_RAD 2
const int RECV_PIN = 12;
//============================================================definition cas pour tlécommmande

enum Touche
{
  ON, NB_RELAIS, TEMP, HEURE, PLUS, MOINS
};
//============================================================
#define RTEMP 1
#define RHEURE 2
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int in=0;
int alarm=0;
int t0;
int brightness=255;
int temp_voulue=20 ;
int timer = 0;
int nb_relais=1;
int on=1;
int t1;
int t2=0;
int reglage=R_r;
float temp_in=19;
int h_alarm=0;
int m_alarm=0;
IRrecv irrecv(RECV_PIN);
decode_results results;
byte tableau_rx[3];

byte newChar[8]={
  B11111,
  B11111,
  B11111,
  B11111,
  B01110,
  B01110,
  B01110,
  B00100
};

byte newChar2[8]={
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111
};



//============================================================ decode pour savoir la touche
byte decodeur()
{
  switch (results.value)
  {
    case 0xA32AB931 :
      return ON;
    break ;

    case 0x39D41DC6 :
      return NB_RELAIS;
    break ;

    case 0xE0984BB6 :
      return TEMP;
    break ;

    case 0x371A3C86 :
      return HEURE;
    break ;

    case 0x143226DB :
      return PLUS;
    break ;

    case 0x1D2FEFF6 :
      return MOINS;
    break ;
   //Serial.println(results.value);
  }
}
//============================================================
void luminositee()
{
  if (brightness>255)
  brightness=0;

  if (brightness<0)
  brightness=255;

  if (on==0)
    analogWrite(PIN_LUM,0);
  else if (on==1) 
  {
    analogWrite(PIN_LUM,brightness);
  }
  

}
//============================================================
void bouton()
{
  switch (decodeur())
  {
    case ON:
    //Serial.println("on");
      if (on==1)
      {
        brightness=0;
      on=0;
      alarm=0;
      }
      else
      {
      brightness=255;
      on=1;
      //alarm=0;
      }
    break;
    
    case NB_RELAIS:
     //Serial.println("nb relais");
      if (nb_relais==1)
      {
        nb_relais=2;
      }
      else
      nb_relais=1;
      break;

   case TEMP:
    //Serial.println("temp");
      if (reglage!=RTEMP)
      {
        reglage=RTEMP;
      }
      else
      {
        reglage=R_r;
      }
      //Serial.println(reglage);
    break;

   case HEURE:
    //Serial.println("heure");
      if (reglage!=RHEURE)
      {
        reglage=RHEURE;
      }
      else
      {
        reglage=R_r;
      }
      //Serial.println(reglage);
    break;

    case PLUS:
     //Serial.println("+");
      switch (reglage)
      {
        case RTEMP:
          temp_voulue++;
        break;
      
        case RHEURE :
         m_alarm++;
        break;

        case R_r:
          brightness+=51;
        break;
      }
    break;
  
    case MOINS:
     //Serial.println("-");
      switch (reglage)
      {
        case RTEMP:
          temp_voulue--;
        break;
      
        case RHEURE :
         m_alarm--;
        break;

        case R_r:
          brightness-=51;
        break;
      }
    break;
  }
}
//============================================================
void afficher_heure()
{
  lcd.setCursor(10,0);
  lcd.print(F("Timer"));
  lcd.setCursor(10,1);
  if (reglage==RHEURE)
  {
    lcd.print(F(">"));
  }
  else
  {
    lcd.print(F(" "));
  }
  lcd.print(h_alarm);
  lcd.print(F(":"));
  if (m_alarm<10)
  {
    lcd.print("0");
  }
  lcd.print(m_alarm);
  
}
//============================================================
void afficher_temp()
{
  lcd.setCursor(0,0);
  lcd.print(F("Int:"));
  lcd.print(temp_in);
  lcd.setCursor(0,1);
  if (reglage==RTEMP)
  {
    lcd.print(F("Rad:>"));
  }
  else
  {
    lcd.print(F("Rad:"));
  }
  lcd.print(temp_voulue);

}
//============================================================
void afficher_nb_relais()
{
  lcd.setCursor(15,1);
  if (nb_relais==2)
  {
  lcd.write(0);
  lcd.setCursor(15,0);
  lcd.write(1);
  }
  else
  lcd.write(0);
  
  
}
//============================================================

void affichage()
{
  lcd.clear();
  luminositee();
  afficher_heure();
  afficher_temp();
  afficher_nb_relais();
}
//============================================================
void gestion_relais()
{
  if (on==1 && temp_in<temp_voulue)
  {
    if (nb_relais==1)
    {
    Serial.println("1r");
    digitalWrite(PIN_RELAIS1,HIGH);
    }
    else 
    {
    Serial.println("2r");
    digitalWrite(PIN_RELAIS1,HIGH);
    digitalWrite(PIN_RELAIS2,HIGH);
    }
  }
  else 
  {
    Serial.println("0r");
    digitalWrite(PIN_RELAIS1,LOW);
    digitalWrite(PIN_RELAIS2,LOW);
  }
}
//============================================================
void gestion_variable()
{
  if (m_alarm>59)
  {
    h_alarm++;
    m_alarm=0;
  }

  if (m_alarm<0)
  {
    h_alarm--;
    m_alarm=59;
  }

  if (h_alarm<0)
  h_alarm=0;
  //============
  if ((t1-t2)>10000)
  reglage=R_r;
  
  if(on==0)
  {
    m_alarm=0;
    h_alarm=0;
  }
}
//============================================================
void gestion_temps()
{
  
  if (alarm==1)
  {
    m_alarm=m_alarm-((millis()-t0)/1000/73);
    if (m_alarm==0 && h_alarm==0)
    {
    on=0;
    alarm=0;
    t0=millis();
    Serial.println("alarm 0");
    }
  }
  else
  {
     t0=millis();
   
  }
  if(m_alarm>0)
   alarm=1;
 Serial.println("alarm");
 Serial.println(alarm);
}
//============================================================
/*void decodage()
{
  address=tableau_rx[0];
  if (address==ADDRESS_RAD)
  {
    temp_voulue=tableau_rx[1]; 
    brightness=tableau_rx[2];
  }
}*/
//============================================================
void setup()
{
  lcd.init();                     
  lcd.backlight();
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(A0, INPUT_PULLUP);
  lcd.createChar(0,newChar);
  lcd.createChar(1,newChar2);



}

void loop()
{
  //in=analogRead(A0);
  //temp_in = 20.4 + (268 -in)*0.250; //calcul de la temperature par la tension de la diode
   
  

  gestion_temps();
  if (irrecv.decode(&results))
  {
    t2=millis();

  bouton();
  irrecv.resume();
  //Serial.println("RCV");
 
  }
  affichage();
 
  gestion_variable();
  gestion_relais();
  
  /*
  Serial.println("reglage");
  Serial.println(reglage);
  Serial.println("on");
  Serial.println(on);
  Serial.println("nb relais");
  Serial.println(nb_relais);*/
  //Serial.println("=======");

  /*
   byte taille_tableau_rx=sizeof(tableau_rx);
  vw_wait_rx_max(300);

   if (vw_get_message(tableau_rx, &taille_tableau_rx)) 
  {
 Serial.println("reception");
  int  t0_RX=0;
   decodage();
  t1=millis();
  }
  */
}