
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  // http web access library
#include <ArduinoJson.h>        // JSON decoding library

#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include "config.h"
#include "logos.h"
#include "image.h"

#define TFT_CS    D2     // TFT CS  pin is connected to NodeMCU pin D2
#define TFT_RST   D3     // TFT RST pin is connected to NodeMCU pin D3
#define TFT_DC    D4     // TFT DC  pin is connected to NodeMCU pin D4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);


int currentXBT = 0;
float currentXRP = 0.000;
int currentETH = 0;
float currentXDG = 0.000;

int  XBTpoint = 0;
int  XRPpoint = 0;
int  ETHpoint = 0;
int  XDGpoint = 0;

WiFiClient client;

int paddingScreen = 20;
int paddingLines = 50;
int paddingLeft = 5;

void setup() {

  Serial.begin(9600);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(1);
  tft.setCursor(20, 40);
  tft.print("Connecting to WIFI");
  tft.setCursor(20, 60);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    tft.print(".");
    delay(500);
  }

  tft.setCursor(20, 100);
  tft.setTextColor(ILI9341_GREEN);
  tft.println("WIFI connected!");

  delay(1000);

  tft.fillScreen(ILI9341_BLACK);

  tft.drawBitmap(paddingScreen + paddingLeft, paddingScreen, xbt_logo, 30, 30, ILI9341_BLACK, ILI9341_ORANGE);
  tft.drawBitmap(paddingScreen + paddingLeft, paddingScreen + (paddingLines * 1), xrp_logo, 30, 30, ILI9341_BLACK, ILI9341_CYAN);
  tft.drawBitmap(paddingScreen + paddingLeft, paddingScreen + (paddingLines * 2), eth_logo, 30, 30, ILI9341_BLACK, ILI9341_MAGENTA);
  tft.drawBitmap(paddingScreen + paddingLeft, paddingScreen + (paddingLines * 3), xdg_logo, 30, 30, ILI9341_BLACK, 0xffbb00);

  tft.drawRGBBitmap(0, 200, imageBitmap, IMG_WIDTH, IMG_HEIGHT);
}


unsigned long draw24(int value) {

  tft.setFont(&FreeSans9pt7b);
  tft.setTextSize(1);
  tft.print("  ");
  int i = 0;
  while (i <= 10) {

    if (i == value || (i == 10 && value == 11)) {
      tft.setTextColor(ILI9341_GREEN);
      tft.print("I");

    }
    else {
      tft.setTextColor(0x3266);
      tft.print("I");
    }

    i++;
  }


}

unsigned long drawText(int x, int y, String text, String previous) {

  tft.setFont(&FreeSans12pt7b);
  tft.setTextSize(1);
  tft.setCursor(x + paddingLeft + 43 , y + 23);
  tft.setTextColor(ILI9341_BLACK);
  tft.print(previous);
  tft.setCursor(x + paddingLeft + 43 , y + 23);
  tft.setTextColor(ILI9341_WHITE);
  //tft.fillRect(x + 43, y + 5, 210, 30, ILI9341_BLACK);
  tft.print(text);
}


unsigned long drawFloat(int x, int y, float text, float previous) {

  tft.setFont(&FreeSans12pt7b);
  tft.setTextSize(1);
  tft.setCursor(x + paddingLeft + 43 , y + 23);
  tft.setTextColor(ILI9341_BLACK);
  tft.print(previous, 3);
  tft.setCursor(x + paddingLeft + 43 , y + 23);
  tft.setTextColor(ILI9341_WHITE);
  //tft.fillRect(x + 43, y + 5, 210, 30, ILI9341_BLACK);
  tft.print(text, 3);
}



unsigned long drawInt(int x, int y, int text, int previous) {

  tft.setFont(&FreeSans12pt7b);
  tft.setTextSize(1);
  tft.setCursor(x + paddingLeft + 42 , y + 23);
  tft.setTextColor(ILI9341_BLACK);
  tft.print(previous);
  tft.setCursor(x + paddingLeft + 42 , y + 23);
  tft.setTextColor(ILI9341_WHITE);
  //tft.fillRect(x + 43, y + 5, 210, 30, ILI9341_BLACK);
  tft.print(text);
}


void loop(void) {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(api);
    int httpCode = http.GET();

    Serial.println("Request HTTP");
    Serial.println(httpCode);
    Serial.println(http.getString());
    if (httpCode > 0)
    {

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, http.getString());


      int newXBT = doc["xbt"].as<int>();
      float newXRP = doc["xrp"].as<float>();
      int newETH = doc["eth"].as<int>();
      float newXDG = doc["xdg"].as<float>();



      XBTpoint = doc["xbt_point"].as<int>();
      XRPpoint = doc["xrp_point"].as<int>();
      ETHpoint = doc["eth_point"].as<int>();
      XDGpoint = doc["xdg_point"].as<int>();

      if (currentXBT == 0) {
        currentXBT = newXBT;
        drawInt(paddingScreen, paddingScreen + (paddingLines * 0), currentXBT, currentXBT);
        draw24(XBTpoint);
      }

      if (currentXRP == 0) {
        currentXRP = newXRP;
        drawFloat(paddingScreen, paddingScreen + (paddingLines * 1), currentXRP, currentXRP);
        draw24(XRPpoint);
      }

      if (currentETH == 0) {
        currentETH = newETH;
        drawInt(paddingScreen, paddingScreen + (paddingLines * 2),  currentETH, currentETH);
        draw24(ETHpoint);
      }

      if (currentXDG == 0) {
        currentXDG = newXDG;
        drawFloat(paddingScreen, paddingScreen + (paddingLines * 3),  currentXDG, currentXDG);
        draw24(XDGpoint);
      }

      Serial.print("Starting while...");

      while (currentXBT != newXBT || currentXRP != newXRP || currentETH != newETH || currentXDG != newXDG) {

        int previousXBT = currentXBT;
        float previousXRP = currentXRP;
        int previousETH = currentETH;
        float previousXDG = currentXDG;

        if (currentXBT < newXBT) {
          currentXBT += 1;
          drawInt(paddingScreen, paddingScreen + (paddingLines * 0), currentXBT, previousXBT);
          draw24(XBTpoint);
        }
        else if (currentXBT > newXBT) {
          currentXBT -= 1;
          drawInt(paddingScreen, paddingScreen + (paddingLines * 0), currentXBT, previousXBT);
          draw24(XBTpoint);
        }



        if (currentXRP < newXRP) {

          if ((newXRP - currentXRP) > 0.001) {

            currentXRP += 0.001;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 1), currentXRP, previousXRP);
            draw24(XRPpoint);
          }
          else {
            currentXRP = newXRP;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 1), currentXRP, previousXRP);
            draw24(XRPpoint);
          }
        }
        else if (currentXRP > newXRP) {

          if ((currentXRP - newXRP) > 0.001) {
            currentXRP -= 0.001;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 1), currentXRP, previousXRP);
            draw24(XRPpoint);
          }
          else {
            currentXRP = newXRP;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 1), currentXRP, previousXRP);
            draw24(XRPpoint);
          }
        }

        if (currentETH < newETH) {
          currentETH += 1;
          drawInt(paddingScreen, paddingScreen + (paddingLines * 2), currentETH, previousETH);
          draw24(ETHpoint);
        }
        else if (currentETH > newETH) {
          currentETH -= 1;
          drawInt(paddingScreen, paddingScreen + (paddingLines * 2), currentETH, previousETH);
          draw24(ETHpoint);
        }

        if (currentXDG < newXDG) {
          if ((newXDG - currentXDG) > 0.001) {
            currentXDG += 0.001;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 3), currentXDG, previousXDG);
            draw24(XDGpoint);
          }
          else {
            currentXDG = newXDG;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 3), currentXDG, previousXDG);
            draw24(XDGpoint);
          }
        }
        else if (currentXDG > newXDG) {

          if ((currentXDG - newXDG) > 0.001) {

            currentXDG -= 0.001;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 3), currentXDG, previousXDG);
            draw24(XDGpoint);
          }
          else {
            currentXDG = newXDG;
            drawFloat(paddingScreen, paddingScreen + (paddingLines * 3), currentXDG, previousXDG);
            draw24(XDGpoint);
          }
        }

        delay(100);
      }
    }
  }
  delay(5000);
}
