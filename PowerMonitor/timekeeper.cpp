#include "timekeeper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "lcd_display.h"

const long utcOffsetInSeconds = 19800;
static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void timekeeper_begin() {
  timeClient.begin();
}

void timekeeper_update() {
  timeClient.update();
}

void timekeeper_display() {
  time_t epochTime = timeClient.getEpochTime();
  int yr = year(epochTime);
  int mn = month(epochTime);
  int dy = day(epochTime);
  int hr = hour(epochTime);
  int mi = minute(epochTime);
  int se = second(epochTime);
  const char* daysOfWeek[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
  int wkday = weekday(epochTime);
  const char* dayName = daysOfWeek[wkday-1];

  lcd_set_cursor(3,0);
  lcd_print("Date and Time");
  lcd_write_custom(2);
  lcd_set_cursor(2,1);
  char buf[32];
  snprintf(buf, sizeof(buf), "Date: %02d/%02d/%04d", dy, mn, yr);
  lcd_print(buf);
  lcd_set_cursor(2,2);
  snprintf(buf, sizeof(buf), "Time: %02d:%02d:%02d %s", (hr%12)==0?12:(hr%12), mi, se, hr<12?"AM":"PM");
  lcd_print(buf);
  lcd_set_cursor(3,3);
  lcd_printf("Day: %s", dayName);
  delay(1000);
}
