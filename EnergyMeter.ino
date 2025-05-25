#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> 
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h> 
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <NTPClient.h>
#include <TimeLib.h> 
#include <ArduinoJson.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include <ElegantOTA.h> // Set for #define ELEGANTOTA_USE_ASYNC_WEBSERVER 0
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11

// Global Variable
int sec = 0, myMinute = 0;
int prevDisplay = -1;
float temp;
int humidity;
const char* weatherDesc = "Updating...";
volatile bool screenToggle = false;
volatile bool LCD_LED = false;
volatile unsigned long lastInterruptTime = 0;
int currentScreen = 0;
const unsigned long debounceDelay = 200;  // 200 ms debounce
uint8_t DHTPin = D3; 
float Temperature;
float Humidity;

// set the LCD address for a 16 chars and 4 line display
LiquidCrystal_I2C lcd(0x27,20,4);  

// set user and password
const char *ssid     = "*********"; // SSID
const char *password = "*********"; //PASSWORD

ESP8266WebServer server(80);

const char* serverUrl = "https://api.openweathermap.org/data/2.5/forecast?q=CITY,COUNRTY_CODE&appid=API_KEY&mode=json&units=metric&cnt=2"; //Set API

const long utcOffsetInSeconds = 19800;// set time +5:30 INDIA

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//set input for pzem sensor to UART
SoftwareSerial pzemSerial(D6, D7); // RX, TX
PZEM004Tv30 pzem(pzemSerial);
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

byte rightArrow[8] = {
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b00000
};

byte clockSymbol[8] = {
  0b00000,
  0b01110,
  0b10101,
  0b10111,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

byte smilesymbol[8] = {
  0b00000,
  0b01010,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte lightningSymbol[8] = {
  0b00100,
  0b01100,
  0b11110,
  0b00110,
  0b01100,
  0b11100,
  0b01000,
  0b00000
};

//Interrupt for Screen toggle
void IRAM_ATTR handleInterrupt1() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    screenToggle = true;
    lastInterruptTime = currentTime;
  }
}

//Interrupt for LED 
void IRAM_ATTR handleInterrupt2() { 
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    LCD_LED = !LCD_LED;
    lastInterruptTime = currentTime;
  }
}

void setup() {
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on

  Serial.begin(115200);
  pzemSerial.begin(9600);

  lcd.createChar(1, rightArrow);
  lcd.createChar(2, clockSymbol);
  lcd.createChar(3, smilesymbol);
  lcd.createChar(4, lightningSymbol);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
    lcd.setCursor(5,0);
    lcd.print("Hello ");
    lcd.write(byte(3));
    lcd.setCursor(4,1);
    lcd.print("Vignesh!");
    lcd.setCursor(4,2);
    lcd.print("Booting up");
    lcd.setCursor(0,3);
    lcd.print("connecting Wifi ...");
  }

  lcd.clear();

  setupOTA();

  pinMode(14, INPUT_PULLUP);  // Button to change screen
  pinMode(2, INPUT_PULLUP);  // Button to turn on or off LCD Led
  attachInterrupt(digitalPinToInterrupt(14), handleInterrupt1, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt2, FALLING);

  showScreen(currentScreen);  // Show initial screen

  timeClient.begin();

  pinMode(DHTPin, INPUT);
  dht.begin();  
}

void loop() {
  server.handleClient();
  ElegantOTA.loop();

  sec = timer();

  // Enable this for automatic screen change for every 10sec
  // int secBlock = sec / 10;
  // int display = 0;

  // if (secBlock == 0 || secBlock == 3)
  //   display = 1;
  // else if (secBlock == 1 || secBlock == 4)
  //   display = 2;
  // else if (secBlock == 2 || secBlock == 5)
  //   display = 3;

  // // Only clear and update when screen changes
  // if (display != prevDisplay) {
  //   lcd.clear();
  //   prevDisplay = display;
  // }

  // switch (display) {
  //   case 1: Sensor_temp(); break;
  //   case 2: Time(); break;
  //   case 3: energy(); break;
  // }

  // button input for change screen
  if (screenToggle) {
      screenToggle = false;
      currentScreen = (currentScreen + 1) % 3;
      lcd.clear(); 
    }

    // Update screen content continuously
    showScreen(currentScreen);
  }

  void showScreen(int screen) {
    switch (screen) {
    case 0:
      Time();
      break;
    case 1:
      Energy();
      break;
    case 2:
      Sensor_temp();
      break;  
  }

  if (LCD_LED) {
    lcd.backlight();
  }else{
    lcd.noBacklight();
  }

  // Call weather function every 10 minutes
  if (sec >= 59) {
    myMinute++;
    if (myMinute == 10) {
      myMinute = 0;
      weather();
    }
  }

}

//OTA updater don't remove for feature update's
void setupOTA() {
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is ElegantOTA Demo.");
  });
  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");
}

int timer(){
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  sec = second(epochTime);
  return sec;
}

void Time(){
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();

  const char* daysOfWeek[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
  };

  int yr = year(epochTime);
  int mn = month(epochTime);
  int dy = day(epochTime);

  int hr = hour(epochTime);
  int mi = minute(epochTime);
  int se = second(epochTime);

  int wkday = weekday(epochTime); // 1 = Sunday, 7 = Saturday
  const char* dayName = daysOfWeek[wkday - 1];

  Serial.printf("Date: %s %d-%02d-%02d\n", dayName, yr, mi, dy);
  Serial.printf("Time: %02d:%02d:%02d\n", hr, mn, se);

  lcd.setCursor(3,0);
  lcd.print("Data and Time");
  lcd.write(byte(2));
  
  // for date in LCD
  lcd.setCursor(2,1);
  lcd.printf("Date: %02d/%02d/%04d", dy, mn, yr);
  
  //for time in LCD
  lcd.setCursor(2,2);
  lcd.printf("Time: %02d:%02d:%02d %s", 
  (hr % 12 == 0) ? 12 : (hr % 12), mi, se, (hr < 12) ? "AM" : "PM");

  // Day Name
  lcd.setCursor(3,3);
  lcd.printf("Day: %s", dayName);

  delay(1000);
}


void Sensor_temp(){
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 

  // Title
  lcd.setCursor(0,0);
  lcd.print("Weather");
  lcd.setCursor(15,0);
  lcd.print("Ch,IN");

  //Local weather from DHT11
  lcd.setCursor(0,1);
  lcd.print("L"); // local data
  lcd.write(byte(1));
  lcd.printf("Temp:%d%cC Hum:%d%%", (int)Temperature, 223, (int)Humidity);

  // Internet weather
  lcd.setCursor(0,2);
  lcd.print("S"); // Internet data
  lcd.write(byte(1));
  lcd.print("Temp:");
  lcd.print((int)temp);
  lcd.write(223);  // Degree symbol
  lcd.print("C Hum:");
  lcd.print((int)humidity);
  lcd.print("%");

  //weather description
  lcd.setCursor(0,3);
  lcd.write(byte(1));
  lcd.print(weatherDesc);

}

void weather(){
  WiFiClientSecure client;
  client.setInsecure();  //Skip certificate verification

  HTTPClient http;
  http.begin(client, serverUrl);  //Use HTTPS and secure client

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Received JSON:");
    Serial.println(payload);

  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (!error) {
    // Access first item in forecast list
    JsonObject forecast = doc["list"][0];

    temp = forecast["main"]["temp"];
    humidity = forecast["main"]["humidity"];
    const char* weatherMain = forecast["weather"][0]["main"];
    weatherDesc = forecast["weather"][0]["description"];
    const char* time = forecast["dt_txt"];

    Serial.print("Forecast Time: ");
    Serial.println(time);
    
    Serial.print("Temperature: ");
    Serial.println(temp);

    Serial.print("Humidity: ");
    Serial.println(humidity);

    Serial.print("Weather: ");
    Serial.println(weatherMain);

    Serial.print("Description: ");
    Serial.println(weatherDesc);
  } else {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
  }

  } else {
    Serial.print("HTTP GET failed: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();
}

void Energy(){

    // Read data from the PZEM sensor
    float voltage   = pzem.voltage();
    float current   = pzem.current();
    float power     = pzem.power();
    float energy    = pzem.energy();
    float frequency = pzem.frequency();
    float pf        = pzem.pf();
 
    if(isnan(voltage)){
        Serial.println("Error reading voltage");
    } else if (isnan(current)) {
        Serial.println("Error reading current");
    } else if (isnan(power)) {
        Serial.println("Error reading power");
    } else if (isnan(energy)) {
        Serial.println("Error reading energy");
    } else if (isnan(frequency)) {
        Serial.println("Error reading frequency");
    } else if (isnan(pf)) {
        Serial.println("Error reading power factor");
    } else {
        //Print values
        Serial.printf("Voltage: %g V", voltage); 
        Serial.printf("Current: %g A", current); 
        Serial.printf("Power: %g W", power); 
        Serial.printf("Energy: %g Kwh", energy); 
        Serial.printf("Frequency: %g Hz", frequency); 
        Serial.printf("Power Factor: %g", pf);
    }

    lcd.setCursor(4,0);
    lcd.print("Energy Meter");
    lcd.write(byte(4));
    lcd.setCursor(0,1);
    lcd.printf("V:%.1fV", voltage); 
    lcd.setCursor(16, 1);
    lcd.print("   ");// Avoid ghost character of A
    lcd.setCursor(10,1);
    lcd.printf("C:%.2fA", current);
    lcd.setCursor(5, 2);
    lcd.print("    "); // Avoid ghost character of W
    lcd.setCursor(0,2);
    lcd.printf("P:%.1fW", power);
    lcd.setCursor(10,2); 
    lcd.printf("E:%.1fkW", energy);
    lcd.setCursor(0,3); 
    lcd.printf("F:%.1fHz", frequency); 
    lcd.setCursor(10,3); 
    lcd.printf("PF:%.2f", pf);

    delay(1000);
 
    Serial.println();
}
