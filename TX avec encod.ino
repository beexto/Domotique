#include <VirtualWire.h>
#include <DS3231.h>

DS3231  rtc(SDA, SCL);
Time  t;
int h_alarm = 14;
int m_alarm = 3;
int h_alarm_stop = 12;
int m_alarm_stop = 30;
int h_nuit =5;
int m_nuit =6;
int h_jour =7;
int m_jour =8;
int lum=255;
int activation=1;
int address=0;
int temp_SDB=23;
byte tableau[15];

void setup() {
  Serial.begin(9600);
  vw_set_tx_pin(6);
  vw_setup(2000);
  rtc.begin();
  
  Serial.println("Go !"); 
}
 
void loop() {
  encodage();
  vw_send(tableau, sizeof(tableau)); // On envoie le message
  vw_wait_tx(); // On attend la fin de l'envoi
  Serial.println("ok TX"); 
}

void encodage()
{
  t = rtc.getTime();
  tableau[0]=address;
  tableau[1]=activation;
  tableau[2]=h_alarm;
  tableau[3]=m_alarm;
  tableau[4]=h_alarm_stop;
  tableau[5]=m_alarm_stop;
  tableau[6]=temp_SDB;
  tableau[7]=t.hour;
  tableau[8]=t.min;
  tableau[9]=t.sec;
  tableau[10]=t.date;
  tableau[11]=t.mon;
  tableau[12]=(t.year-2019);
}