# 🚰 Smart Water Meter Portable (ESP32 & Firebase)

Proyek *Internet of Things* (IoT) untuk mengukur debit dan volume aliran air secara presisi, dilengkapi dengan fitur *auto-cutoff* menggunakan katup solenoid (*solenoid valve*). Perangkat ini dapat dipantau secara lokal via layar OLED maupun dari jarak jauh melalui *Web Dashboard* yang terhubung dengan Firebase.

## Fitur Utama
- **Monitoring Real-Time**: Memantau debit air (L/min) dan total volume yang mengalir (Liter).
- **Mode Otomatis (Auto-Cutoff)**: Mengalirkan air sesuai dengan target liter yang diinputkan dan otomatis menutup katup jika target tercapai.
- **Mode Manual**: Membuka katup secara manual atau menghentikan aliran air secara paksa kapan saja.
- **Web Dashboard Responsif**: Antarmuka kontrol berbasis web.
- **Indikator Koneksi**: Mendeteksi apakah perangkat ESP32 sedang *Online* atau *Offline* langsung dari web.
- **Dual-Control**: Dapat dikendalikan lewat Web maupun lokal melalui Serial Monitor, dan keduanya tersinkronisasi.

## Hardware
Proyek ini dikembangkan menggunakan mikrokontroler **ESP32**. Komponen yang dibutuhkan:
1. ESP32 Development Board
2. Water Flow Sensor (terhubung ke pin **27**)
3. Modul Relay & Solenoid Valve (terhubung ke pin **26**)
4. Layar OLED SSD1306 128x64 I2C (SDA = **21**, SCL = **22**)
5. Kabel Jumper & Catu Daya yang sesuai.

## Software and Libraries
Pastikan Anda telah menginstal *library* berikut di Arduino IDE:
- `WiFi.h` 
- `Adafruit GFX Library` & `Adafruit SSD1306` 
- `Firebase Arduino Client Library for ESP8266 and ESP32` 


**Pada File Arduino (`.ino`)**
Temukan baris berikut di awal kode dan ganti yang ada pada file "Meter Air Portable dengan ESP32.docs":
```cpp
#define WIFI_SSID "NAMA_WIFI"
#define WIFI_PASSWORD "PASSWORD_WIFI"
#define API_KEY "API_KEY_FIREBASE"
#define DATABASE_URL "URL_DATABASE_FIREBASE"

Lokasi File "Meter Air Portable dengan ESP32.docs":Pelatihan Non Aparatur Perdagangan 2026 > 0.Sistem Informasi Pelatihan 2026 > sertifikat_khairul > Meter_Air_Portable_ESP32 > Meter Air dengan ESP32.doc
