#include "timekeeper.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "lcd_display.h"
#include "wifi_manager.h"
#include "config.h"

static WiFiUDP ntpUDP;
static NTPClient timeClient(ntpUDP, Config::Time::kNtpServer, Config::Time::kUtcOffsetSeconds);
static time_t g_lastSyncedEpoch = 0;
static unsigned long g_lastSyncMillis = 0;
static bool g_timeValid = false;
static unsigned long g_lastNtpAttempt = 0;

void timekeeper_begin() {
  timeClient.begin();
}

static time_t current_epoch_time() {
  if (!g_timeValid) {
    return 0;
  }

  return g_lastSyncedEpoch + ((millis() - g_lastSyncMillis) / 1000);
}

void timekeeper_update() {
  if (!wifi_is_connected()) {
    return;
  }

  unsigned long retryInterval = g_timeValid ? Config::Time::kResyncIntervalMs : Config::Time::kInitialRetryIntervalMs;
  if (g_lastNtpAttempt != 0 && millis() - g_lastNtpAttempt < retryInterval) {
    return;
  }

  g_lastNtpAttempt = millis();
  if (timeClient.update()) {
    g_lastSyncedEpoch = timeClient.getEpochTime();
    g_lastSyncMillis = millis();
    g_timeValid = true;
  } else if (!g_timeValid) {
    unsigned long forcedUpdateInterval = timeClient.getUpdateInterval();
    timeClient.setUpdateInterval(1000);
    if (timeClient.forceUpdate()) {
      g_lastSyncedEpoch = timeClient.getEpochTime();
      g_lastSyncMillis = millis();
      g_timeValid = true;
    }
    timeClient.setUpdateInterval(forcedUpdateInterval);
  }
}

bool timekeeper_is_synced() {
  return g_timeValid;
}

void timekeeper_display() {
  if (!g_timeValid) {
    lcd_write_line(0, String("Date and Time ") + char(2));
    lcd_write_line(1, "Date: --/--/----");
    lcd_write_line(2, "Time: --:--:--");
    lcd_write_line(3, "Day: Not Synced");
    return;
  }

  time_t epochTime = current_epoch_time();
  int yr = year(epochTime);
  int mn = month(epochTime);
  int dy = day(epochTime);
  int hr = hour(epochTime);
  int mi = minute(epochTime);
  int se = second(epochTime);
  const char* daysOfWeek[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
  int wkday = weekday(epochTime);
  const char* dayName = daysOfWeek[wkday-1];

  char buf[32];
  lcd_write_line(0, String("Date and Time ") + char(2));

  snprintf(buf, sizeof(buf), "Date:%02d/%02d/%04d", dy, mn, yr);
  lcd_write_line(1, buf);

  snprintf(buf, sizeof(buf), "Time:%02d:%02d:%02d %s", (hr%12)==0?12:(hr%12), mi, se, hr<12?"AM":"PM");
  lcd_write_line(2, buf);

  snprintf(buf, sizeof(buf), "Day:%s", dayName);
  lcd_write_line(3, buf);
}
