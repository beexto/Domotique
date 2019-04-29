#include <DS3231.h>
#include <VirtualWire.h>



int h_alarm = 0;
int m_alarm = 0;
int h_alarm_stop = 0;
int m_alarm_stop = 0;
int h_nuit =0;
int m_nuit =0;
int h_jour =0;
int m_jour =0;
int temp_SDB;
int h=0,m=0,s=0,a=0,month=0,day;
int activation=0;
int address=0;


//============================

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
DS3231  rtc(SDA, SCL);
void setup() {
lcd.init();                      // initialize the lcd 
lcd.backlight();
rtc.begin();
Serial.begin(9600);
vw_set_tx_pin(6);
vw_setup(2000);
Time  t;
}
//===================================================
void loop()
{
  comm_SDB()
  t = rtc.getTime(); 
}
//=====================================
 void comm_SDB()
{
  t=millis();
  byte tableau_tx[27];
  byte tableau_rx[27];
  taille_tableau_rx=sizeof(tableau_rx)
  vw_wait_rx_max(200);
 
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
   

}
//==============================
void encodage(byte *tableau_tx)
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
}

void debug()
{
  Serial.println("année"); 
  Serial.println(a); 
  Serial.println("mois"); 
  Serial.println(month); 
  Serial.println("jour"); 
  Serial.println(day); 
  Serial.println("heure"); 
  Serial.println(h); 
  Serial.println("minute"); 
  Serial.println(m); 
  Serial.println("seconde"); 
  Serial.println(s); 
  Serial.println("h_alarm"); 
  Serial.println(h_alarm); 
   Serial.println("m_alarm"); 
  Serial.println(m_alarm); 
   Serial.println("h_alarm_stop"); 
  Serial.println(h_alarm_stop); 
   Serial.println("m_alarm_stop"); 
  Serial.println(m_alarm_stop); 
   Serial.println("temp_SDB"); 
  Serial.println(temp_SDB); 
   Serial.println("adress"); 
  Serial.println(address);
    Serial.println("activation"); 
  Serial.println(activation);
    Serial.println("adress"); 
  Serial.println(address);
  Serial.println("=================");
}
