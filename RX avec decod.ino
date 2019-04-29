#include <DS3231.h>
#include <VirtualWire.h>

DS3231  rtc(SDA, SCL);
Time  t;
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

  byte tableau[15];
void decodage()
{
  address=tableau[0];
  activation=tableau[1]; 
  h_alarm= tableau[2] ; 
  m_alarm=tableau[3];
  h_alarm_stop= tableau[4];
  m_alarm_stop=  tableau[5];
  temp_SDB  =tableau[6];
  h=tableau[7];
  m=tableau[8];
  s=tableau[9];
  day= tableau[10];
  month=tableau[11];
  a= tableau[12]+2019;
  
}

void setup() {
  Serial.begin(9600);
  vw_set_rx_pin(12);
  vw_setup(2000);
  vw_rx_start();

  Serial.println("Go !"); 
}

void loop() {
  byte taille_message = sizeof(tableau);
  vw_wait_rx();
  if (vw_get_message(tableau, &taille_message)) {
  decodage();
  debug();  
  }
  delay(5000);
}

/*
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
*/