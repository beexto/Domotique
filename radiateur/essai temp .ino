#include  <Wire.h>
#include  <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
int in=0;
void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  Serial.begin(9600);
pinMode(A0, INPUT_PULLUP);
}

void loop()
{
  in=analogRead(A0);
  float T = 20.4 + (276 -in)*0.250;
  // when characters arrive over the serial port...
      delay(500);
    // clear the screen
    lcd.clear();
    // read all the available characters
 
   lcd.print(in);
   lcd.print("---");
   lcd.print(T);
   Serial.println(in);
   Serial.println(T);
    
  
}

