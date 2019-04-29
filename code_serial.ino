//  Auto Generated Code for Arduino IDE
//  Created using keuwlsofts Bluetooth Electronics App
//  www.keuwl.com

//  Steps: 
//  1) Upload code and power your Arduino project 
//  2) Run Bluetooth Electronics App and connect to your device 
//  3) Press the reset button on your Arduino 

//  This auto generated code template is only a suggestion to get you started.
//  It is incomplete and will need modification specific to your project.

int update_interval=1000; // time interval in ms for updating panel indicators 
unsigned long last_time=0; // time of last update
char data_in; // data received from serial link
int slider_value; // Received Slider Values
String text; // String for text elements

void setup() {

  Serial.begin(9600); //Change baud rate as required
}

void loop() {

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
////////////////////////////////////////////////////////////
    if(data_in=='C'){ //Switch On led lit
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

  /////////////  Send Data to Android device

  unsigned long t=millis();
  if ((t-last_time)>update_interval)
  {
    last_time=t;

    // Update Text Element 
    text=h_alarm; // <--- Set text to send here 
    Serial.print("*H"+text+"*");

    // Update Text Element 
    text=h_alarm_stop; // <--- Set text to send here 
    Serial.print("*h"+text+"*");

    // Update Text Element 
    text=m_alarm; // <--- Set text to send here 
    Serial.print("*M"+text+"*");

    // Update Text Element 
    text=m_alarm_stop; // <--- Set text to send here 
    Serial.print("*m"+text+"*");

  }

}
