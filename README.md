# NTP time for ESP8266 and ESP32
 NTP time example for ESP8266 and ESP32 based on standard functionality
 
 See video: https://youtu.be/r2UAmBLBBRM
 
 Correction: The time() function only calls the NTP server every hour. So there is no need for the function
 getTimeReducedTraffic(). The function getNTPtime() does more or less the same...
 Thank you, Jose Baars for your testing!
