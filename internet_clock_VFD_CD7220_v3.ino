// Arduino-Pin-Verbindungsmethode, die folgenden Nummern werden entsprechend dem IO des Geräts geändert
// Dieses Beispiel verwendet ESP8266-12
// In diesem Beispiel wird eine 6-stellige, 8-stellige, 12-stellige, 16-stellige 5x7-Anzeige verwendet
// https://mikroshop.ch/LED_LCD.html?gruppe=7&artikel=2546
// https://mikroshop.ch//data/VFD_SPI.zip
// extra info: https://github.com/slabua/micropython-futaba-vfd-driver?tab=readme-ov-file
// This example uses a 6-digit, 8-digit, 12-digit, and 16-digit 5x7 display.
// This copy was uploaded from https://github.com/tehniq3/VFD_SPI

/*
 * NTP clock on Futaba 000FV959IN and BOE CIG14-0604B VFD display 
 * Nicu FLORICA (niq_ro) created this NTP clock based on info from  https://thesolaruniverse.wordpress.com/2022/11/01/an-internet-synced-clock-circular-display-with-gc9a01-controller-powered-by-an-esp8266/
 * v.1 - initial version for NTP clock
 * v.1a - added date as scroll text, datee requested as at https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
 * v.2 - clock scroll as date (intro and the end) + moved RESET od display to D1 (not RST)
 * v.2a - real update the seconds as real time
 * v.3  changed display to CD7220 (serial VFD display)
 */

   #include <NTPClient.h>
   #include <ESP8266WiFi.h>
   #include <WiFiUdp.h>

   #include <SoftwareSerial.h>
SoftwareSerial mySerial(99, D1); // RX, TX

const char* ssid =            "bbk2";                                                            // network wifi credentials  - fill in your wifi network name
const char* password =        "internet2";                                                            // network wifi credentials  - fill in your wifi key

const long utcOffsetInSeconds = 3*3600;                                                                        // 3600 = western europe winter time - 7200 = western europe summer time
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

char daysOfTheWeek[7][12] = {"     Sunday", "     Monday", "    Tuesday", "  Wednesday", "   Thursday", "     Friday", "   Saturday"};
//char daysOfTheWeek[7][12] =   {"   Duminica", "       Luni", "      Marti", "   Miercuri", "        Joi", "     Vineri", "    Sambata"};

String intro = "       NTP clock v.3 by niq_ro      ";

int monthDay;
String weekDay;
int currentMonth;
String currentMonthName;
int currentYear;
String currentDate;

   uint32_t targetTime = 0;                                                            // for next 1 second timeout

   int hh = 0;                                                                         // hours variable
   int mm = 0;                                                                         // minutes variable
   int ss = 0;                                                                         // seconds variable
   int ss1, ss2, ss3;
   int yy, ll, dd, zz;
 
   WiFiUDP ntpUDP;                                                                     // define NTP client to get time
   NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);  

bool initial = 1;
 int bright = 1;              // 1 < n < 4
 int rev = 1;                 // 0-normal; 1-reverse
 int blRate = 150;
 byte debuging = 1;           // 1 - serial monitor, 0 - no serial monitor


void setup() {
   Serial.begin (115200);
   Serial.println ();
   Serial.println (intro);
   mySerial.begin(9600);
    delay(10);
   // setDispMode();
         mySerial.write(0x0C);     // Clear ALL
          delay(10);
                                   // Display init
                 mySerial.write(0x1B);
                 mySerial.write(0x40);
         howBright();              // Display brightness

   WiFi.begin (ssid, password);

   while (WiFi.status() != WL_CONNECTED ) 
      {
      delay (500);
      Serial.print (".");
      }
   Serial.print ("connection with ");
   Serial.println (ssid);  
   Serial.println ("-------------------------------"); 
   
 
   timeClient.begin();
   timeClient.update ();

   Serial.print ("internet server time: ");   

   time_t epochTime = timeClient.getEpochTime();
   Serial.print("Epoch Time: ");
   Serial.println(epochTime);
   
   Serial.println(timeClient.getFormattedTime());
 //  Serial.print("Formatted Time: ");
 //  Serial.println(formattedTime);  

   hh = timeClient.getHours ();
   mm = timeClient.getMinutes ();
   ss = timeClient.getSeconds ();
   Serial.print(hh);
   Serial.print(":");
   Serial.print(mm);
   Serial.print(":");
   Serial.println(ss);


   zz = timeClient.getDay(); 
   weekDay = weekDays[zz];
   Serial.print("Week Day: ");
   Serial.println(weekDay);  

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  dd = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(dd);

  ll = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(ll);

  currentMonthName = months[ll-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  yy = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(yy);

  //Print complete date:
  currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.print("Current date: ");
  Serial.println(currentDate);

   ss = timeClient.getSeconds ();  // updte second before loop
   Serial.print("ss => ");
   Serial.println(ss);
   targetTime = millis();  // time to be updated just if it need
}

void loop() {
   if (targetTime < millis())
      {
      targetTime += 1000;
      ss++;                                                                            // advance second
      if (ss>60)
         {
         ss=ss%60;
         mm++;                                                                         // advance minute
         if(mm>59)
            {
            mm=0;
            hh++;                                                                      // advance hour
            if (hh>23) 
               {
               hh=0;
               timeClient.update ();  // update at midnight

   time_t epochTime = timeClient.getEpochTime();
   Serial.print("Epoch Time: ");
   Serial.println(epochTime);
   
   hh = timeClient.getHours ();
   mm = timeClient.getMinutes ();
   ss = timeClient.getSeconds ();
   Serial.print(hh);
   Serial.print(":");
   Serial.print(mm);
   Serial.print(":");
   Serial.println(ss);

   zz = timeClient.getDay();
   weekDay = weekDays[zz];
   Serial.print("Week Day: ");
   Serial.println(zz);  

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  dd = ptm->tm_mday;
  Serial.print("Month day: ");
  Serial.println(dd);

  ll = ptm->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(ll);

  currentMonthName = months[ll-1];
  Serial.print("Month name: ");
  Serial.println(currentMonthName);

  yy = ptm->tm_year+1900;
  Serial.print("Year: ");
  Serial.println(yy);

  //Print complete date:
  currentDate = String(yy) + "-" + String(ll) + "-" + String(dd);
  Serial.print("Current date: ");
  Serial.println(currentDate); 
               }
            }
         }
   Serial.print("ss = ");
   Serial.println(ss);
  String ora = " ";
  if (hh/10 == 0)
    ora = ora + " ";
   else
    ora = ora + hh/10;
  ora = ora + hh%10;
  if (ss%2 == 0)
    ora = ora + ":";
   else
    ora = ora + " ";
  ora = ora + mm/10;
  ora = ora + mm%10;

 String data = "     ";
  data = data + weekDay;
  data = data + ", ";
  data = data + monthDay;
  data = data + "-";
  data = data + currentMonthName;
  data = data + "-";
  data = data + currentYear;
  data = data + "      ";

  String ora1 = ora + "      "; 

    dateVFD();
    dayVFD();
    loctimeVFD();
    EEST();
      }


delay(10);
} // end of main loop


void setDispMode(){
  mySerial.write(0x02);  //  STX
  mySerial.write(0x05);  //  05
  mySerial.write(0x43);  //  C
  mySerial.write(0x31);  //  31h - ESC/POS mode
  mySerial.write(0x03);  //  ETX
}

// Set brightness of the VFD
void howBright(){
         mySerial.write(0x1B);
         mySerial.write(0x2A);
         mySerial.write(bright);
}

void mop()  // clear the display
{
  mySerial.write(0x0C);     // Clear ALL
  delay(10);  
  mySerial.write(0x1F);    // Set cursor
  mySerial.write(0x24);    // Set Cursor
  mySerial.write(1);      // position x
  mySerial.write(1);       // position y
  mySerial.print("                    ");
  mySerial.print("                    ");
}

void dayVFD(){
                            // set cursor for  Day as name
    mySerial.write(0x1F);   // Set cursor
    mySerial.write(0x24);   // Set Cursor
    mySerial.write(10);      // position x
    mySerial.write(1);      // position y
                            // Day
    mySerial.println(daysOfTheWeek[timeClient.getDay()]);
                        
    if (debuging == 1)
    {
      Serial.print("Name of the Day: ");
      Serial.println(daysOfTheWeek[zz]);
    }        
    }

// Print DATE DD-MMM-YYYY on the upper line of VFD


void dateVFD(){
                            // set cursor for  DATE   DD MMM YY 
    mySerial.write(0x1F);   // Set cursor
    mySerial.write(0x24);   // Set Cursor
    mySerial.write(10);      // position x
    mySerial.write(2);      // position y
                            // Day
        niceTime(dd);
    if (debuging == 1)
    {
      Serial.print("Day: ");
      Serial.println(dd);
    }
        

    if (debuging == 1)
    {
      Serial.print("Month: ");
      Serial.println(ll);
    }
        
                            // make Month name from Month number     
    switch(ll){
      case 1: 
      mySerial.print("-Jan-");
      break;
      case 2: mySerial.print("-Feb-");
      break;
      case 3: mySerial.print("-Mar-");
      break;
      case 4: mySerial.print("-Apr-");
      break;
      case 5: mySerial.print("-May-");
      break;
      case 6: mySerial.print("-Jun-");
      break;
      case 7: mySerial.print("-Jul-");
      break;
      case 8: mySerial.print("-Aug-");
      break;
      case 9: mySerial.print("-Sep-");
      break;
      case 10: mySerial.print("-Oct-");
      break;
      case 11: mySerial.print("-Nov-");
      break;
      case 12: mySerial.print("-Dec-");
      //default: mySerial.print("/ERR/");
      break; }
                            // Year
      mySerial.println(yy);
      if (debuging == 1)
    {
      Serial.print("Year: ");
      Serial.println(yy+2000);
    }
    }

// Print Local time on VFD
   
void loctimeVFD(){    
//    dateVFD();
// set cursor for  Local TIME 
    mySerial.write(0x1F);    // Set cursor
    mySerial.write(0x24);    // Set Cursor
    mySerial.write(1);      // position x
    mySerial.write(1);       // position y

// print UTC TIME on VFD with nice 0 when only one digit
    niceTime(hh);
    mySerial.write(':');
    niceTime(mm);
    mySerial.write(':');
    niceTime(ss);

if (debuging == 1)
    {
    Serial.print(hh);
    Serial.print(":");
    Serial.print(mm);
    Serial.print(":");
    Serial.println(ss);
    }      }


// Common function to format hour and minutes

void niceTime(int number) {
     if (number >= 0 && number < 10)
         {
         mySerial.write('0');
         }
     mySerial.print(number);
}

void EEST(){
                            // set cursor for  Day as name
    mySerial.write(0x1F);   // Set cursor
    mySerial.write(0x24);   // Set Cursor
    mySerial.write(1);      // position x
    mySerial.write(2);       // position y
                            // UTC
    mySerial.print("UTC+");
    mySerial.print(utcOffsetInSeconds/3600);
}






                        
