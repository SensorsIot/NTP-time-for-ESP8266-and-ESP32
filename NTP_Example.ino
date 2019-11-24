/*
  This is an example file for using the time function in ESP8266 or ESP32 tu get NTP time
  It offers two functions:

  - getNTPtime(struct tm * info, uint32_t ms) where info is a structure which contains time
  information and ms is the time the service waits till it gets a response from NTP.
  Each time you cann this function it calls NTP over the net.

  If you do not want to call an NTP service every second, you can use
  - getTimeReducedTraffic(int ms) where ms is the the time between two physical NTP server calls. Betwwn these calls,
  the time structure is updated with the (inaccurate) timer. If you call NTP every few minutes you should be ok

  The time structure is called tm and has teh following values:

  Definition of struct tm:
  Member  Type  Meaning Range
  tm_sec  int seconds after the minute  0-61*
  tm_min  int minutes after the hour  0-59
  tm_hour int hours since midnight  0-23
  tm_mday int day of the month  1-31
  tm_mon  int months since January  0-11
  tm_year int years since 1900
  tm_wday int days since Sunday 0-6
  tm_yday int days since January 1  0-365
  tm_isdst  int Daylight Saving Time flag

  because the values are somhow akwardly defined, I introduce a function makeHumanreadable() where all values are adjusted according normal numbering.
  e.g. January is month 1 and not 0 And Sunday or monday is weekday 1 not 0 (according definition of MONDAYFIRST)

  Showtime is an example on how you can use the time in your sketch

  The functions are inspired by work of G6EJD ( https://www.youtube.com/channel/UCgtlqH_lkMdIa4jZLItcsTg )

*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <Time.h>
#include <credentials.h>

/*
  The credentials.h file at least has to contain:
  char mySSID[]="your SSID";
  char myPASSWORD[]="your Password";

  It has to be placed in the libraries folder

  If you do not want a credentials file. delete the line: #include <credentials.h>
*/

#ifdef CREDENTIALS
const char* ssid = mySSID;
const char* password = myPASSWORD;
#else
const char* ssid = "yourSSID";
const char* password = "YourPassword";
#endif

const char* NTP_SERVER = "ch.pool.ntp.org";
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";  // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)

tm timeinfo;
time_t now;
long unsigned lastNTPtime;
unsigned long lastEntryTime;


void setup() {
  Serial.begin(115200);
  Serial.println("\n\nNTP Time Test\n");
  WiFi.begin(ssid, password);

  int counter;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if (counter++ > 10) ESP.restart();
    Serial.print ( "." );
  }
  Serial.println("\n\nWiFi connected\n\n");

  configTime(0, 0, NTP_SERVER);
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  setenv("TZ", TZ_INFO, 1);

  if (getNTPtime(10)) {  // wait up to 10sec to sync
  } else {
    Serial.println("Time not set");
    ESP.restart();
  }
  showTime(&timeinfo);
  lastNTPtime = time(&now);
  lastEntryTime = millis();
}


void loop() {
  // getTimeReducedTraffic(3600);
  getNTPtime(10);
  showTime(&timeinfo);
  delay(1000);
}

bool getNTPtime(int sec) {

  {
    uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &timeinfo);
      Serial.print(".");
      delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    if (timeinfo.tm_year <= (2016 - 1900)) return false;  // the NTP call was not successful
    Serial.print("now ");  Serial.println(now);
      char time_output[30];
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now));
    Serial.println(time_output);
    Serial.println();
  }
  return true;
}

void getTimeReducedTraffic(int sec) {
  tm *ptm;
  if ((millis() - lastEntryTime) < (1000 * sec)) {
    now = lastNTPtime + (int)(millis() - lastEntryTime) / 1000;
  } else {
    lastEntryTime = millis();
    lastNTPtime = time(&now);
    now = lastNTPtime;
    Serial.println("Get NTP time");
  }
  ptm = localtime(&now);
  timeinfo = *ptm;
}

void showTime(tm *localTime) {
  Serial.print(localTime->tm_mday);
  Serial.print('/');
  Serial.print(localTime->tm_mon + 1);
  Serial.print('/');
  Serial.print(localTime->tm_year - 100);
  Serial.print('-');
  Serial.print(localTime->tm_hour);
  Serial.print(':');
  Serial.print(localTime->tm_min);
  Serial.print(':');
  Serial.print(localTime->tm_sec);
  Serial.print(" Day of Week ");
  if (localTime->tm_mday = 0) localTime->tm_mday = 7;
  Serial.println(localTime->tm_wday);
}
