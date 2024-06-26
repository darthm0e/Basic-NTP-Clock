#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include <Adafruit_ILI9341.h>

#define TFT_DC D4
#define TFT_CS D2

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


const char *ssid     = "VIDIT-Bingen";
const char *password = "%m1n!5NK=$";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "2.de.pool.ntp.org", 7200);

WiFiClientSecure client;


HTTPClient http;

String payload;
String cityURL = "https://wttr.in/Ockenheim?format=%l:+%t";

//Week Days
//String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
//String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void setup(){
  Serial.begin(115200);
  tft.begin();
  WiFi.begin(ssid, password);

  //Rotate to Portrait - Pins Left
  tft.setRotation(3);

  tft.fillScreen(ILI9341_BLACK);
  // Draw Border
  tft.drawRect(5, 5, 310, 230, ILI9341_BLUE);
  tft.drawRect(6, 6, 308, 228, ILI9341_BLUE);
  tft.drawRect(7, 7, 306, 226, ILI9341_GREENYELLOW);
  tft.drawRect(8, 8, 304, 224, ILI9341_BLUE);
  tft.drawRect(9, 9, 302, 222, ILI9341_BLUE);
  
  //Draw Title
  tft.setCursor(60, 40);
  tft.setTextColor(ILI9341_GREEN);  
  tft.setTextSize(4);
  tft.println("NTP Clock");

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
      client.setInsecure();
    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, cityURL)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = http.getString();
          //Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
}

void loop() {

  //Fetch Time and print it
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  tft.setCursor(40, 120);
  tft.setTextColor(ILI9341_GREENYELLOW, ILI9341_BLACK);  
  tft.setTextSize(5);
  tft.println(timeClient.getFormattedTime());
  
  //Draw Blue seperator Line
  tft.drawLine(40,170,280,170,ILI9341_BLUE);

  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  
  //Print complete date:
  String currentDate = String(monthDay) + "." + String(currentMonth) + "." + String(currentYear);
  tft.setCursor(80, 190);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_PURPLE, ILI9341_BLACK);
  tft.println(currentDate);

  tft.setCursor(65, 90);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.println(payload);

  //Janitor break
  delay(1);

  yield();
  delay(50);
}
