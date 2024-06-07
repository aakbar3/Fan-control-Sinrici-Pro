# Fan-control-Sinrici-Pro
README
SinricPro Fan Control dengan ESP8266
Repository ini berisi kode untuk mengontrol kipas menggunakan SinricPro dengan ESP8266. Kode ini juga memungkinkan pengendalian kipas melalui tombol fisik.

Fitur
Menghubungkan ESP8266 ke jaringan WiFi.
Mengontrol kipas melalui platform SinricPro.
Mengontrol kipas menggunakan tombol fisik yang terhubung ke ESP8266.
Mengatur kecepatan kipas (1-3) dan mematikan kipas dengan tombol fisik.
Menyinkronkan status kipas di SinricPro dengan perubahan status yang dilakukan melalui tombol fisik.
Hardware yang Dibutuhkan
ESP8266
Relay module (3 channel)
Tombol fisik (4 buah)
Kipas
Kabel jumper
Library yang Digunakan
ESP8266WiFi.h
SinricPro.h
SinricProFanUS.h
Wiring Diagram
Relay Pins:

Relay 1: D0
Relay 2: D5
Relay 3: D6
Button Pins:

Button 1: D2
Button 2: D1
Button 3: D4
Button 4: D3 (untuk mematikan kipas)
Setup
Clone repository ini.

Install Arduino IDE jika belum terpasang.

Install library yang diperlukan:

ESP8266WiFi
SinricPro
Buka file sketch_jun7a.ino di Arduino IDE.

Ganti informasi WiFi dan SinricPro dengan milik Anda:

cpp
Copy code
#define WIFI_SSID         "NamaWiFiAnda"
#define WIFI_PASS         "PasswordWiFiAnda"
#define APP_KEY           "AppKeySinricProAnda"
#define APP_SECRET        "AppSecretSinricProAnda"
#define FAN_ID            "FanIDSinricProAnda"
Upload kode ke ESP8266.

Hubungkan ESP8266 ke relay module dan tombol fisik sesuai dengan wiring diagram.

Penggunaan
Mengontrol Kipas melalui SinricPro
Setelah ESP8266 tersambung ke WiFi, perangkat Anda akan muncul di aplikasi SinricPro.
Anda dapat mengontrol daya (on/off) dan kecepatan kipas (1-3) melalui aplikasi.
Mengontrol Kipas melalui Tombol Fisik
Tekan tombol 1 untuk mengatur kecepatan kipas ke 1.
Tekan tombol 2 untuk mengatur kecepatan kipas ke 2.
Tekan tombol 3 untuk mengatur kecepatan kipas ke 3.
Tekan tombol 4 untuk mematikan kipas.
Perubahan yang dilakukan melalui tombol fisik akan disinkronkan dengan status di SinricPro.

Troubleshooting
Pastikan informasi WiFi dan SinricPro Anda benar.
Pastikan wiring sesuai dengan diagram.
Pastikan library yang diperlukan sudah terinstall.
Lisensi
Proyek ini dilisensikan di bawah MIT License. Silakan baca file LICENSE untuk informasi lebih lanjut.

Kontribusi
Kontribusi sangat diterima! Silakan fork repository ini dan buat pull request dengan perubahan yang Anda sarankan.

Selamat mencoba dan semoga berhasil! Jika Anda memiliki pertanyaan atau masalah, jangan ragu untuk membuka isu di repository ini.
