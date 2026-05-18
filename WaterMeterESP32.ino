#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Ada di Gdrive"
#define WIFI_PASSWORD "Ada di Gdrive"
#define API_KEY "Ada di Gdrive"
#define DATABASE_URL "Ada di Gdrive"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const byte sensorPin = 27; 
const int relayPin = 26; 

volatile long pulseCount = 0;
float flowRate = 0.0;
float totalMilliLitres = 0.0; 
unsigned long oldTime = 0;

float targetVolumeML = 0.0; 
bool isValveOpen = false;
const float mL_per_pulsa = 2.834;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void updateOLED() {
  display.clearDisplay();              
  display.setTextSize(1);              
  display.setTextColor(SSD1306_WHITE); 

  display.setCursor(0, 0); 
  if (isValveOpen) {
    display.print("STATUS: MENGISI...");
  } else {
    display.print("STATUS: TERTUTUP");
  }

  display.setCursor(0, 16);
  display.print("Debit : ");
  display.print(flowRate, 2); 
  display.print(" L/min");

  display.setCursor(0, 32);
  display.print("Volume: ");
  display.print(totalMilliLitres / 1000.0, 3); 
  display.print(" L");

  display.setCursor(0, 48);
  display.print("Target: ");
  if (targetVolumeML > 0) {
    display.print(targetVolumeML / 1000.0, 2);
    display.print(" L");
  } else {
    display.print("--- L");
  }
  display.display(); 
}

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  digitalWrite(relayPin, HIGH); 
  
  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Gagal menemukan layar OLED SSD1306"));
    for(;;); 
  } 
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  display.print("SMART WATER METER");
  display.setCursor(45, 40);
  display.print("READY");
  display.display();

 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Terhubung!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Berhasil terhubung ke Firebase Auth!");
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.RTDB.setBool(&fbdo, "/sistem_air/kontrol/perintah_buka", false);
  Firebase.RTDB.setFloat(&fbdo, "/sistem_air/sensor/volume_sekarang", 0.0);
  Firebase.RTDB.setString(&fbdo, "/sistem_air/sensor/status_valve", "TERTUTUP");
  
  Serial.println("Sistem Siap.");
  updateOLED(); 
  oldTime = millis();
}

void loop() {
  if (Serial.available() > 0) {
    float inputLiters = Serial.parseFloat(); 
    while(Serial.available() > 0) Serial.read();

    if (inputLiters > 0) {
      targetVolumeML = inputLiters * 1000.0; 
      totalMilliLitres = 0.0; 
      digitalWrite(relayPin, LOW); 
      isValveOpen = true;
      Serial.printf("\n[LOKAL] Target: %.2f Liter. Valve Terbuka...\n", inputLiters);
      updateOLED(); 
      
      if(Firebase.ready()) {
         Firebase.RTDB.setBool(&fbdo, "/sistem_air/kontrol/perintah_buka", true);
         Firebase.RTDB.setString(&fbdo, "/sistem_air/sensor/status_valve", "TERBUKA");
      }
    }
  }

  if (Firebase.ready()) {
    if (Firebase.RTDB.getBool(&fbdo, "/sistem_air/kontrol/perintah_buka")) {
      bool reqBuka = fbdo.boolData();
      
      if (reqBuka == true && !isValveOpen) {
        if (Firebase.RTDB.getFloat(&fbdo, "/sistem_air/kontrol/target_liter")) {
          targetVolumeML = fbdo.floatData() * 1000.0;
          totalMilliLitres = 0.0;
          digitalWrite(relayPin, LOW);
          isValveOpen = true;
          Serial.printf("\n[WEB] Target: %.2f Liter. Valve Terbuka...\n", targetVolumeML/1000.0);
          updateOLED();
          Firebase.RTDB.setString(&fbdo, "/sistem_air/sensor/status_valve", "TERBUKA");
        }
      } 
      else if (reqBuka == false && isValveOpen) {
        digitalWrite(relayPin, HIGH);
        isValveOpen = false;
        Serial.println("\n[WEB] Valve dihentikan dari Web.");
        updateOLED();
        Firebase.RTDB.setString(&fbdo, "/sistem_air/sensor/status_valve", "TERTUTUP");
      }
    }
  }

  unsigned long currentMillis = millis();
  if ((currentMillis - oldTime) >= 1000) {
    oldTime = currentMillis; 

    Firebase.RTDB.setInt(&fbdo, "/sistem_air/status/heartbeat", millis());

    noInterrupts(); 
    long currentPulses = pulseCount;
    pulseCount = 0;
    interrupts();   

    float penambahanVolumeML = currentPulses * mL_per_pulsa;
    totalMilliLitres += penambahanVolumeML;
    flowRate = (penambahanVolumeML * 60.0) / 1000.0; 

    updateOLED();

    if (Firebase.ready()) {
      Firebase.RTDB.setFloat(&fbdo, "/sistem_air/sensor/debit_air", flowRate);
      if (isValveOpen) {
        Firebase.RTDB.setFloat(&fbdo, "/sistem_air/sensor/volume_sekarang", totalMilliLitres / 1000.0);
      }
    }

    if (isValveOpen) {
      if (totalMilliLitres >= targetVolumeML) {
        digitalWrite(relayPin, HIGH); 
        isValveOpen = false;
        targetVolumeML = 0.0; 
        
        Serial.println("\n[SELESAI] Target tercapai! Valve otomatis tertutup.");
        updateOLED();
        
        if (Firebase.ready()) {
           Firebase.RTDB.setBool(&fbdo, "/sistem_air/kontrol/perintah_buka", false);
           Firebase.RTDB.setString(&fbdo, "/sistem_air/sensor/status_valve", "TERTUTUP");
        }
      }
    } 
  }
}
