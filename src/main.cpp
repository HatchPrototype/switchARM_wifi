#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"



#define AIO_SERVER      "192.168.1.10"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "switches/sw1");
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "switches/sw1_out");
String vals[10];
bool dataValid = false;
byte outBuff[] = {0xCC,0xAA,0xCC,0xAA,0x00,0x00,0x00,0x00,0x00,0x00};




void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  //int retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds  Serial.println(); Serial.println();
  //Serial.print("Connecting to ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }


}
}

byte calcCRC()
{
  byte cr =0x00;
  for(int g=0;g<9;g++)
  {
    cr ^= outBuff[g];
  }

  return cr;
}


void sendStuff(String gt)
{
  if (! photocell.publish(gt.c_str())) {
  } else {

  }
}




int splitString(String inpu, char delim)
{

  int len = inpu.length();
  char dm[len];
  byte idxs[20];
  idxs[0] = 0;
  inpu.toCharArray(dm,len);
  int ret = 0;
  for(int i=0;i<len;i++)
  {
    if(dm[i] == delim)
    {
      ret++;
      idxs[ret] = i;
    }
  }
  idxs[ret+1] = len;
  if(ret!=0)
  {

    bool firstOver = false;
    for(int f=0;f<ret+1;f++)
    {
      if(firstOver == false)
      {
        vals[f] = inpu.substring(idxs[f],idxs[f+1]);
        firstOver = true;
      }
      else
      {
        vals[f] = inpu.substring(idxs[f]+1,idxs[f+1]);
      }

    }


  }
return ret;
}

void setup() {

  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);
  if(!wifiManager.autoConnect("Switch 1","elixir1234"))
  {
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  mqtt.subscribe(&onoffbutton);
  // put your setup code here, to run once:
}

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      //Serial.print(F("Got: "));
      String smp = (char *) onoffbutton.lastread;
      int dm = splitString(smp,',');
      if(dm!=0)
      {
          if(vals[0] == "SWITCH")
          {
            if(vals[1] == "STATE")
            {
              int s1,s2,s3,s4;
              s1 = vals[2].toInt();
              s2 = vals[3].toInt();
              s3 = vals[4].toInt();
              s4 = vals[5].toInt();
              outBuff[4] = 0x01;
              if(s1 == 0)
              {
                outBuff[5] = 0x00;
              }
              else if(s1 == 1)
              {
                outBuff[5] = 0x01;
              }
              else
              {
                outBuff[5] = 0x22;
              }
//--------------------------------------------------------------
              if(s2 == 0)
              {
                outBuff[6] = 0x00;
              }
              else if(s2 == 1)
              {
                outBuff[6] = 0x01;
              }
              else
              {
                outBuff[6] = 0x22;
              }

//---------------------------------------------------------------
              if(s3 == 0)
              {
                outBuff[7] = 0x00;
              }
              else if(s3 == 1)
              {
                outBuff[7] = 0x01;
              }
              else
              {
                outBuff[7] = 0x22;
              }
//---------------------------------------------------------------
              if(s4 == 0)
              {
                outBuff[8] = 0x00;
              }
              else if(s4 == 1)
              {
                outBuff[8] = 0x01;
              }
              else
              {
                outBuff[8] = 0x22;
              }

              outBuff[9] = calcCRC();
              Serial.write(outBuff,10);
            }
            else if(vals[1] == "ON")
            {
              int s1,s2,s3;
              s1 = vals[2].toInt();
              s2 = vals[3].toInt();
              s3 = vals[4].toInt();
              if((s1<256)&&(s2<256)&&(s3<256))
              {
                outBuff[4] = 0x02;
                outBuff[5] = (char) s1;
                outBuff[6] = (char) s2;
                outBuff[7] = (char) s3;
                outBuff[8] = 0x00;
                outBuff[9] = calcCRC();
                Serial.write(outBuff,10);
              }

            }

            else if(vals[1] == "OFF")
            {
              int s1,s2,s3;
              s1 = vals[2].toInt();
              s2 = vals[3].toInt();
              s3 = vals[4].toInt();
              if((s1<256)&&(s2<256)&&(s3<256))
              {
                outBuff[4] = 0x03;
                outBuff[5] = (char) s1;
                outBuff[6] = (char) s2;
                outBuff[7] = (char) s3;
                outBuff[8] = 0x00;
                outBuff[9] = calcCRC();
                Serial.write(outBuff,10);
              }

            }

            else if(vals[1] == "GETST")
            {
              outBuff[4] = 0x04;
              outBuff[5] = 0x00;
              outBuff[6] = 0x00;
              outBuff[7] = 0x00;
              outBuff[8] = 0x00;
              outBuff[9] = calcCRC();
              Serial.write(outBuff,10);
            }
            else if(vals[1] == "GETRGB")
            {
              outBuff[4] = 0x05;
              outBuff[5] = 0x00;
              outBuff[6] = 0x00;
              outBuff[7] = 0x00;
              outBuff[8] = 0x00;
              outBuff[9] = calcCRC();
              Serial.write(outBuff,10);
            }
          }
      }
  }
}
byte df[12];
Serial.setTimeout(5);
if(Serial.available())
{
  Serial.readBytes(df,11);
  byte crc = 0x00;
  for(int x=0;x<10;x++)
  {
    crc ^= df[x];
  }
  if(df[11] == crc)
  {
    if((df[0]==0xCC)&&(df[1]==0xAA)&&(df[2]==0xCC)&&(df[3]==0xAA))
    {
      if(df[4] == 0xF0)
      {
        String outst = "SWITCH,ST,";
        if(df[5]==0x00)
        {
          outst = outst + "0,";
        }
        else if(df[5]==0x01)
        {
          outst = outst + "1,";
        }
        else
        {
          outst = outst + "2,";
        }

        if(df[6]==0x00)
        {
          outst = outst + "0,";
        }
        else if(df[6]==0x01)
        {
          outst = outst + "1,";
        }
        else
        {
          outst = outst + "2,";
        }

        if(df[7]==0x00)
        {
          outst = outst + "0,";
        }
        else if(df[7]==0x01)
        {
          outst = outst + "1,";
        }
        else
        {
          outst = outst + "2,";
        }

        if(df[8]==0x00)
        {
          outst = outst + "0";
        }
        else if(df[8]==0x01)
        {
          outst = outst + "1";
        }
        else
        {
          outst = outst + "2";
        }
        sendStuff(outst);

      }
      else if(df[4] == 0xF2)
      {
        String outst = "SWITCH,RGB,";
        outst = outst + String((int) df[5]) + ",";
        outst = outst + String((int) df[6]) + ",";
        outst = outst + String((int) df[7]) + ",";
        outst = outst + String((int) df[8]) + ",";
        outst = outst + String((int) df[9]) + ",";
        outst = outst + String((int) df[10]);
        sendStuff(outst);
      }
    }
  }

}
}
