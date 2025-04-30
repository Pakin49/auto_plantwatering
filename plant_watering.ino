#include <WiFi.h>
#include <time.h>
#include <esp_sntp.h>

const int RELAY_ENABLE = 13;

const char *ssid = "true_home2G_979";
const char *password = "n3vH46Dr";

uint64_t deepsleep_time1 = ((7 * 60L) + 55) * 60L * 1000000L; // 7 hrs 55 mins
uint64_t deepsleep_time2 = ((15 * 60L) + 55) * 60L * 1000000L; // 15 hrs 55 mins

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 25200;
const int daylightOffset_sec = 0;

const char *time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
int hour = 0,minute = 0; 

void getLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("No time available (yet)");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;
}

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  //Serial.println("Got time adjustment from NTP!");
  getLocalTime();
}

void setup() {
  
  pinMode(RELAY_ENABLE, OUTPUT);
  digitalWrite(RELAY_ENABLE,LOW);
  //Serial.begin(115200);

  // First step is to configure WiFi STA and connect in order to get the current time and date.
  //Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  /**
   * NTP server address could be acquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 acquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE acquired NTP server address
   */
  esp_sntp_servermode_dhcp(1);  // (optional)

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println(" CONNECTED");

  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}

void loop() {
  getLocalTime();
  if(hour == 16 && minute == 20){
    water_on();
    esp_sleep_enable_timer_wakeup(deepsleep_time2); 
    esp_deep_sleep_start(); // เริ่มเข้าสู่โหมด Deep Sleep
  }
  else if( hour == 8 && minute == 20){
    water_on();
    esp_sleep_enable_timer_wakeup(deepsleep_time1); 
    esp_deep_sleep_start(); // เริ่มเข้าสู่โหมด Deep Sleep
  }
  else{
    //Serial.println("Get time again in 10 seconds");
    delay(10 * 1000);
    return;
  }
 
}

void water_on(){
  //Serial.println("Watering for 25 seconds");
  digitalWrite(RELAY_ENABLE,HIGH);
  delay(25 * 1000);
  digitalWrite(RELAY_ENABLE,LOW);
}
