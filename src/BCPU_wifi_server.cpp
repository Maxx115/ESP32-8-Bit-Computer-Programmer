/* --------------- INCLUDE SECTION ---------------- */
#include "BCPU_wifi_server.hpp"

#include "self_arduino.hpp"

#include "WiFi.h"

#include <AsyncElegantOTA.h>

#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <WebSerial.h>

#include "BCPU_ramPrograms.hpp"
#include "BCPU_functions.hpp"

/* ---------------- DEFINES / CONSTANTs ---------------- */
TaskHandle_t selProgTask = NULL;

/* ---------------- LOCAL VARIABLE SECTION ---------------- */
char* ssid = "xxx";
char* password = "xxx";

IPAddress local_IP(192, 168, 0, 101);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
String hostname = "ESP32_BCPU_Programmer";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String haltState;

int freqDelayValue = -1;

/* ---------------- HELP FUNCTION SECTION ---------------- */
void initSpiffs(void)
{
  if(!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}

String siteProcess(const String& var)
{
  return String();
}

int freq2time(int freq)
{
  if(freq == 0)
  {
    return -1;
  }
  else
  {
    return (1000/freq);
  }
}

/* ---------------- WIFI FUNCTION SECTION ---------------- */
void initWifi(TaskHandle_t task_selProg)
{
  selProgTask = task_selProg;

  if(!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA failed to configure");
  }
  else
  {
    WiFi.setHostname(hostname.c_str());
  }

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  Serial.println("Wifi-Setup done.");
}

/* ---------------- WEBSOCKET FUNCTION SECTION ---------------- */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      // handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket(void)
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  Serial.println("Websocket-Setup done.");
}

/* ---------------- SERVER FUNCTION SECTION ---------------- */
void initServer(void)
{
  initSpiffs();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
      request->send(SPIFFS, "/index.html", String(), false, siteProcess);
    });
  
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
    {
      request->send(SPIFFS, "/style.css", "text/css");
    });

  /* --> used by OverTheAir (OTA) update
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
      request->send(200, "text/plain", "OK");
    });
  */

  server.on("/gpio", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
      String inputMessage1;
      String inputMessage2;

      if(request->hasParam("output") && request->hasParam("state")) 
      {
        inputMessage1 = request->getParam("output")->value();
        inputMessage2 = request->getParam("state")->value();

        if(inputMessage1 == "hlt_gpio" && inputMessage2 == "0")
        {
          setHLT(LOW);
        }
        else if(inputMessage1 == "hlt_gpio" && inputMessage2 == "1")
        {
          setHLT(HIGH);
        }
        else if(inputMessage1 == "sclk_gpio" && inputMessage2 == "0")
        {
          setMCLK(LOW);
        }
        else if(inputMessage1 == "sclk_gpio" && inputMessage2 == "1")
        {
          setMCLK(HIGH);
        }
      }

      if(request->hasParam("slider") && request->hasParam("value")) 
      {
        inputMessage1 = request->getParam("slider")->value();
        inputMessage2 = request->getParam("value")->value();

        if(inputMessage1 == "freqSlider")
        {
          freqDelayValue = freq2time(inputMessage2.toInt());
        }
      }
      
      if(request->hasParam("button"))
      {
        inputMessage1 = request->getParam("button")->value();

        if(inputMessage1 == "pulse_button" && freqDelayValue == -1)
        {
          pulseMCLK();
        }
        else if(inputMessage1 == "reset_button")
        {
          resetBCPU();
        }
      }

      request->send(200, "text/plain", "OK");
    });

  server.on("/readCLK", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if(getClkState() == true)
    {
      request->send(200, "text/plane", "ON");
    }
    else
    {
      request->send(200, "text/plane", "OFF");
    }
  });

  server.on("/progSel", HTTP_GET, [] (AsyncWebServerRequest *request) 
    {
      String inputMessage1;
      String inputMessage2;

      if(request->hasParam("name") && request->hasParam("value")) 
      {
        inputMessage1 = request->getParam("name")->value();
        inputMessage2 = request->getParam("value")->value();

        if(inputMessage2 == "fibonacci")
        {
          newProg = prog_FibonacciSeries;
        }
        else if(inputMessage2 == "addUntilOverflow")
        {
          newProg = prog_AddUntilOverflow;
        }
        else if(inputMessage2 == "7x12")
        {
          newProg = prog_Multiply7By12;
        }
        else if(inputMessage2 == "NOP")
        {
          newProg = prog_NOP;
        }

        vTaskResume(selProgTask);
      }

      request->send(200, "text/plain", "OK");
    });

  initWebSocket();

  AsyncElegantOTA.begin(&server);
  Serial.println("OTA Enabled.");

  WebSerial.begin(&server);
  Serial.println("WebSerial Enabled.");

  server.begin();
  Serial.println("Server-Setup done.");
}

int freqDelay(void)
{
  return freqDelayValue;
}