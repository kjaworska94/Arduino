#include <idDHT11.h>
#include <Wire.h>
#include <VirtualWire.h>
#include <BMP180.h>

int idDHT11pin = 3; // podłącznie DHT
int idDHT11intNumber = 1; 
int nRainIn = A1;  // podłącznie czujnika deszczu
int nRainDigitalIn = 2;   // podłącznie czujnika deszczu
boolean bIsRaining = false;
String strRaining;

void dht11_wrapper(); // must be declared before the lib initialization

idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);

BMP180 barometer;
// float seaLevelPressure = 101000;

void setup()
{
  Serial.begin(9600);
  delay(5000);
  pinMode(2,INPUT);
   
    barometer = BMP180();
  if(barometer.EnsureConnected())
  {
    Serial.println("BMP180 Polaczone.");
    barometer.SoftReset();
    barometer.Initialize();
  }
  else
  { 
    Serial.println("BMP180 Blad polaczenia.");
  }
  
  Serial.print("DHT 11 LIB version: ");
  Serial.println(IDDHT11LIB_VERSION);
  Serial.println("---------------");
  }

void dht11_wrapper() 
{  DHT11.isrCallback();  }

void loop()
{
  Serial.print("\nRetrieving information from sensor: ");
    //delay(100);
  DHT11.acquire();
  while (DHT11.acquiring());
  int result = DHT11.getStatus();
  switch (result)
  {
  case IDDHTLIB_OK: 
    Serial.println("OK"); 
    break; }

int a = (DHT11.getHumidity());

if(barometer.IsConnected)
  {
    // Retrive the current pressure in Pascals.
    long currentPressure = barometer.GetPressure();
  }}

