#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define sensor A0
#define AlcoTresh 1.0
SoftwareSerial GPS_SoftSerial(4, 3);/* (Rx, Tx) */
SoftwareSerial gsm(5,6);//(Tx,Rx)
TinyGPSPlus gps;
double lat_val, lng_val;  
const int xPin = A1;    // X output of the accelerometer
const int yPin = A2;    // Y output of the accelerometer
const int zPin = A3;    // Z output of the accelerometer
int xr,yr,zr;
volatile float minutes, seconds;
volatile int degree, secs, mins;

void setup() {
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(zPin, INPUT);
  pinMode(sensor,INPUT);
  Serial.begin(9600); /* Define baud rate for serial communication */
  gsm.begin(9600);
  delay(2000);
  GPS_SoftSerial.begin(9600); /* Define baud rate for software serial communication */
}

void loop() {
/////////////////////////////////// SENSING PART  //////////////////////////////////////
    int x=0;
    float adcValue=0;
    for(int i=0;i<10;i++)
    {
      adcValue+= analogRead(sensor);
      delay(10);
    }
    xr = analogRead(xPin);
    yr = analogRead(yPin);
    zr = analogRead(zPin);
    float v=(adcValue/10)*(5.0/1024.0);
    float mgL= 0.67*v;
        smartDelay(1000); /* Generate precise delay of 1ms */
        unsigned long start;
        //double lat_val, lng_val;
        uint8_t hr_val, min_val, sec_val;
        bool loc_valid, alt_valid, time_valid;
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
        //alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
        alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
        hr_val = gps.time.hour(); /* Get hour */
        min_val = gps.time.minute();  /* Get minutes */
        sec_val = gps.time.second();  /* Get seconds */
        time_valid = gps.time.isValid();  /* Check if valid time data is available */

/******************************          
int q[10];
int front=0,rear=0,r=0,n=10;
    if(rear==n)
        rear=0;

    q[rear]=secs;
    rear++;
    
int traverse()
{
    for(int i=0;i<n;i++)
    {
        if(q[i]==secs)
            return 1;
    }
    return 0;
}
    int z;
    z=traverse();
*******************************/    
  //////////////////////////////  PRINTING ALL VALUES   /////////////////////////////////////
    Serial.print("BAC:");
    Serial.print(mgL);
    Serial.print(" mg/L\n");
    Serial.print(xr);
    Serial.print(",");
    Serial.print(yr);
    Serial.print(",");
    Serial.print(zr);
    Serial.print("\n");
    if (!loc_valid)
    {          
          Serial.print("Latitude : ");
          Serial.println("*****");
          Serial.print("Longitude : ");
          Serial.println("*****");
    }
    else
    {
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val, 6);
          Serial.print("Latitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val, 6);
          Serial.print("Longitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
          Serial.println("");
    }
//////////////////////////////   END OF SENSING AND PRINTING  ///////////////////////////////////////
    //////////////////////////    CONDITIONS    /////////////////////////////////////////////////    
    if(mgL>AlcoTresh) 
    {
      x = TriggerSMS(1);
      if(x==1)
      {
        Serial.println("Caution SMS Sent Successfully...");
      }
    }

    if((((xr>435) && (xr<480))  &&  ((zr>450) && (zr<480))) || (((xr>360) && (xr<380))  &&  ((zr>450) && (zr<480)))) 
    {
      Serial.println("Tilting!!!!!!!!!!");
      x = TriggerSMS(2);
      if(x==1)
      {
        Serial.println("SOS SMS Sent Successfully...");
      }
    }
    delay(1000);////////DELAY FOR WHOLE LOOP.....
}


////////////////////FUNCTIONS BLOCK////////////////////////////////////////////////////////////
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SoftSerial.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(GPS_SoftSerial.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}

void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{  
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
}

int TriggerSMS(int param)
{
  delay(10000);
  Serial.begin(9600);
  gsm.begin(9600);

  if(param == 1)
  {
    gsm.println("AT+CMGF=1");
    delay(1000);
    gsm.println("AT+CMGS=\"+918553362552\"\r"); //replace x by your number
    delay(1000);
    gsm.print("!!!CAUTION!!! Drink And Drive, Please Stop Driving...");
    delay(100);
    gsm.println((char)26);
  }
  else
  {
    gsm.println("AT+CMGF=1");
    delay(1000);
    gsm.println("AT+CMGS=\"+919740344922\"\r"); //replace x by your number
    delay(1000);
    gsm.print("!!!ALERT!!!Your friend met with an accident at https://www.google.com/maps/search/");
    gsm.print(lat_val,6);
    gsm.print(",");
    gsm.print(lng_val,6);
    delay(100);
    gsm.println((char)26); 
  }
  delay(1000);   
  return 1;
}

////////////////////////////////////BUFFER BLOCK///////////////////////////////////////////////////////


