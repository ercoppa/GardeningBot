// credits:
//  https://www.bakke.online/index.php/2017/05/21/reducing-wifi-power-consumption-on-esp8266-part-2/
//  https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

#include <UniversalTelegramBot.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "conf.h"

#define DEBUG 0

#define SENSOR_INPUT          A0
#define SENSOR_SWITCH         D5
#define PUMP                  D6
#define N_READINGS            2
#define SLEEP_TIME            (60e6 * 60) // 1 hour (max allowed is 71 mins)
#define TELEGRAM_MAX_ATTEMPTS 4
#define WATERING_TIME         40 // secs
#define MOISTURE_THRESHOLD    30

// SSL client needed for both libraries
WiFiClientSecure client;

// telegram bot
UniversalTelegramBot bot(BOT_TOKEN, client);

// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct {
  uint32_t crc32;   // 4 bytes
  uint8_t channel;  // 1 byte,   5 in total
  uint8_t bssid[6]; // 6 bytes, 11 in total
  uint8_t padding;  // 1 byte,  12 in total
} rtcData;

void setup() {

  // turn off wifi
  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  delay( 1 );

#if DEBUG
  Serial.begin(115200);
#endif

  // PIN setup
  pinMode(SENSOR_SWITCH, OUTPUT);
  digitalWrite(SENSOR_SWITCH, LOW);
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);

  // sensor reading
  int valueBefore = getMoisture();
  if (valueBefore < MOISTURE_THRESHOLD)
    activatePump();
  int valueAfter = getMoisture();

  // Try to read WiFi settings from RTC memory
  bool rtcValid = false;
  if( ESP.rtcUserMemoryRead( 0, (uint32_t*)&rtcData, sizeof( rtcData ) ) ) {
    // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
    uint32_t crc = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
    if( crc == rtcData.crc32 ) {
      rtcValid = true;
    }
  }
  
  // Attempt to connect to Wifi network:
#if DEBUG
  Serial.print("Connecting Wifi: ");
  Serial.println(WLAN_SSID);
#endif
  
  WiFi.forceSleepWake();
  delay( 1 );
  WiFi.persistent( false );
  WiFi.mode( WIFI_STA );

  /*
  IPAddress ip( 192, 168, 1, 109 );
  IPAddress gateway( 192, 168, 1, 1 );
  IPAddress subnet( 255, 255, 255, 0 );
  WiFi.config( ip, gateway, subnet );
  */

  if( rtcValid ) {
    // The RTC data was good, make a quick connection
    WiFi.begin( WLAN_SSID, WLAN_PASSWD, rtcData.channel, rtcData.bssid, true );
  } else {
    // The RTC data was not valid, so make a regular connection
    WiFi.begin( WLAN_SSID, WLAN_PASSWD );
  }

  int retries = 0;
  int wifiStatus = WiFi.status();
  while( wifiStatus != WL_CONNECTED ) {
    retries++;

#if DEBUG
    Serial.print(".");
#endif

    if( retries == 100 ) {
      // Quick connect is not working, reset WiFi and try regular connection
      WiFi.disconnect();
      delay( 10 );
      WiFi.forceSleepBegin();
      delay( 10 );
      WiFi.forceSleepWake();
      delay( 10 );
      WiFi.begin( WLAN_SSID, WLAN_PASSWD );
    }
    if( retries == 600 ) {
      // Giving up after 30 seconds and going back to sleep
      WiFi.disconnect( true );
      delay( 1 );
      WiFi.mode( WIFI_OFF );
      ESP.deepSleep( SLEEP_TIME, WAKE_RF_DISABLED );
      return; // Not expecting this to be called, the previous call will never return.
    }
    delay( 50 );
    wifiStatus = WiFi.status();
  }

#if DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  IPAddress ip_got = WiFi.localIP();
  Serial.println(ip_got);
#endif

  // Write current connection info back to RTC
  rtcData.channel = WiFi.channel();
  memcpy( rtcData.bssid, WiFi.BSSID(), 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
  rtcData.crc32 = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
  ESP.rtcUserMemoryWrite( 0, (uint32_t*)&rtcData, sizeof( rtcData ) );

  sendTelegramMessage(valueBefore, valueAfter);

  WiFi.disconnect( true );
  delay(1);

  // WAKE_RF_DISABLED to keep the WiFi radio disabled when we wake up
  ESP.deepSleep(SLEEP_TIME, WAKE_RF_DISABLED);
}

void sendTelegramMessage(int moistureValueBefore, int moistureValueAfter) {

  String message = "Moisture value: ";
  message.concat(moistureValueBefore);
  message.concat("% => ");
  message.concat(moistureValueAfter);
  message.concat("%\n");

  int attempt = 0;
  while (attempt++ < TELEGRAM_MAX_ATTEMPTS)
    if(bot.sendMessage(CHAT_ID, message, "Markdown")) {
#if DEBUG
      Serial.println("TELEGRAM sent");
#endif
      break;
    } else {
#if DEBUG
      Serial.println("TELEGRAM failed"); 
#endif
      delay(500);
    }
}

void activatePump() {
#if DEBUG
  Serial.print("PUMP ON\n");
#endif
  digitalWrite(PUMP, HIGH);
  delay(1000 * WATERING_TIME);
  digitalWrite(PUMP, LOW);
#if DEBUG
  Serial.print("PUMP OFF\n");
#endif 
}

int getMoisture() {

#if DEBUG
  Serial.println("Starting reading...");
#endif

  int output_value = 0;
  for (int i = 0; i < N_READINGS; i++) {

    if (i > 0)
      delay(2 * 1000);

    digitalWrite(SENSOR_SWITCH, HIGH);
    delay(1000);
    int raw_value = analogRead(SENSOR_INPUT);
    digitalWrite(SENSOR_SWITCH, LOW);

#if DEBUG
    Serial.print("Moisture: ");
    Serial.print(raw_value);
    Serial.print("\n");
#endif

    output_value += raw_value;
  }

  output_value = output_value / N_READINGS;
  output_value = map(output_value, 1024, 350, 0, 100);

#if DEBUG
  Serial.print("Moisture: ");
  Serial.print(output_value);
  Serial.print("%\n");
#endif

  return output_value;
}

uint32_t calculateCRC32( const uint8_t *data, size_t length ) {
  uint32_t crc = 0xffffffff;
  while( length-- ) {
    uint8_t c = *data++;
    for( uint32_t i = 0x80; i > 0; i >>= 1 ) {
      bool bit = crc & 0x80000000;
      if( c & i ) {
        bit = !bit;
      }

      crc <<= 1;
      if( bit ) {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}

void loop() {}
