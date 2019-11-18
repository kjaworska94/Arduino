#include <Wire.h>
#include <OneWire.h>
#include <VirtualWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //0x27 is the LCD Address
#define DS3231_I2C_ADDRESS 0x68


#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float  TempInC;

byte temp[8] = {B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110}; //ikona termometru
byte ss[8] = {0b00100, 0b01110, 0b10101, 0b10000, 0b01110, 0b00001, 0b11110, 0b00000}; // literka "Ś"

// Sensory płytki wysyłającej
int wil=0;
int tem=0;
int cis =0;
int jas=0;
char MsgReceived[21]; 
int led = 13; //pin for LED

void setup()
{
  Serial.begin(9600);
  sensors.begin();
  pinMode(led, OUTPUT);
  // pinMode(12, INPUT_PULLUP);
  Wire.begin(); //initialise the TWI
  lcd.begin(20, 4);  //20 x 4 LCD (Change if required)
  lcd.backlight(); // finish with backlight on
  lcd.createChar(1, temp);
  lcd.createChar(2, ss);
  lcd.write((uint8_t)0);  

          // EKRAN POWITALNY //

lcd.setCursor(3,0); //set cursor to top left corner
lcd.print("LADOWANIE"); //print the text to the lcd
lcd.setCursor(2,2); //set cursor to top left corner
lcd.print("Stacja Pogodowa"); //print the text to the lcd

for (int i = 0; i <= 80; i++){  // you can change the increment value here
lcd.setCursor(13,0);
if (i<80) lcd.print(" ");  //print a space if the percentage is < 100 
if (i<10) lcd.print(" ");  //print a space if the percentage is < 10
lcd.print(i);
lcd.print("%");
delay(100);  //change the delay to change how fast the boot up screen changes 
  }
lcd.clear();
          // KONIEC EKRAN POWITALNY //

  // VirtualWire 
  // Bits per sec
  vw_setup(2000);
  // set pin for connect receiver module 
  vw_set_rx_pin(11);  
  // Start the receiver PLL running
  vw_rx_start();

}
//***********************************************************

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  // pobieramy wiadomość 
  if (vw_get_message(buf, &buflen)) 
  {
    digitalWrite(led, HIGH);
    delay(100);
    int i;
    // kopiujemy ją do zmiennej wiadomosc
    for (i = 0; i < buflen; i++)
    {            
      // Fill Msg Char array with corresponding 
      // chars from buffer.   
      MsgReceived[i] = char(buf[i]);
      //Serial.print(MsgReceived[i]);
    }

    sscanf(MsgReceived, "%d,%d,%d,%d",&wil, &tem, &cis, &jas); // Converts a string to an array
    digitalWrite(led, LOW); 
    memset( MsgReceived, 0, sizeof(MsgReceived));// This line is for reset the StringReceived
  }

displayTime(); //calls the display time function
sensors.requestTemperatures();
TempInC = sensors.getTempCByIndex(0);

  // wyświetlanie temperatury
TempInC = sensors.getTempCByIndex(0);

lcd.setCursor(1, 1);
lcd.print(char(1));
lcd.setCursor(3, 1);
lcd.print("Z:"); 
lcd.print(tem);
lcd.print((char)223);     // symbol stopni
lcd.print("C");
lcd.setCursor(11, 1);
lcd.print(char(1));
lcd.setCursor(13, 1);
lcd.print("W:"); 
lcd.print(TempInC,0);
lcd.setCursor(17, 1);
lcd.print((char)223);     // symbol stopni
lcd.print("C");

// wyświetlanie ciśnienia

lcd.setCursor(0, 2);
lcd.print("CI");
lcd.print(char(2));
lcd.print("NIENIE:");
lcd.print(" ");
lcd.print(cis);
lcd.print(" ");
lcd.print("hPa");

// wyświetlanie wilgotności i nasłonecznienie

lcd.setCursor(0, 3);
lcd.print("WILGO.");
lcd.print(wil);
lcd.print("%");
lcd.print(" ");
lcd.print("JASN.");
lcd.print(jas/10);
lcd.print("%");
delay(1000); 
}

byte decToBcd(byte val){ return( (val/10*16) + (val%10) ); }
byte bcdToDec(byte val){ return( (val/16*10) + (val%16) ); }


void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); 
  // okieśla, że następna wartość ma trafić do rejestru sekund
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
 
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
                 &year);
  if (hour < 10)
  {
    lcd.setCursor(1, 0);
    lcd.print("0");
    lcd.setCursor(2, 0);
    lcd.print(hour , DEC);
  }
  if (hour > 9)
  {
    lcd.setCursor(1, 0);
    lcd.print(hour , DEC);
  }
  lcd.setCursor(3, 0);
  lcd.print(":");
  if (minute < 10)
  {
    lcd.setCursor(4, 0);
    lcd.print("0");
    lcd.setCursor(5, 0);
    lcd.print(minute , DEC);
  }
  if (minute > 9)
  {
    lcd.setCursor(4, 0);
    lcd.print(minute , DEC);
  }
  lcd.setCursor(6, 0);
  lcd.print(":");
  if (second < 10)
  {
    lcd.setCursor(7, 0);
    lcd.print("0");
    lcd.setCursor(8, 0);
    lcd.print(second, DEC);
  }
  if (second > 9)
  {
    lcd.setCursor(7, 0);
    lcd.print(second, DEC);
  }
  if (dayOfMonth < 10)
  {
    lcd.setCursor(10, 0);
    lcd.print("0");
    lcd.setCursor(11, 0);
    lcd.print(dayOfMonth, DEC);
    lcd.setCursor(12, 0);
    lcd.print("-");
  }
  if (dayOfMonth > 9)
  {
    lcd.setCursor(10, 0);
    lcd.print(dayOfMonth, DEC);
    lcd.setCursor(12, 0);
    lcd.print("-");
  }
  if (month < 10)
  {
    lcd.setCursor(13, 0);
    lcd.print("0");
    lcd.setCursor(14, 0);
    lcd.print(month, DEC);
    lcd.setCursor(15, 0);
    lcd.print("-");
  }
  if (month > 9)
  {
    lcd.setCursor(13, 0);
    lcd.print(month, DEC);
    lcd.setCursor(15, 0);
    lcd.print("-");
  }
  lcd.setCursor(16, 0);
  lcd.print("20");
  lcd.setCursor(18, 0);
  lcd.print(year, DEC);
  } 

