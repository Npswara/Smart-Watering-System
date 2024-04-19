#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <virtuabotixRTC.h>

const char* ssid = "ESP"; // Nama WiFi
const char* password = "12345678"; // Kata sandi WiFi

const int buzzerPin = D1; // Pin buzzer
const int relayPin = D2; // Pin relay
int targetHour = 0; // Jam target
int targetMin = 0; // Menit target
int delayDuration = 0; // Durasi penundaan sebelum bunyi buzzer (dalam detik)
int targetDate = 0;

ESP8266WebServer server(80);
virtuabotixRTC myRTC(D8, D7, D6); // CLK, DAT, RST

void setup() {
  Serial.begin(9600);
  myRTC.setDS1302Time(00, 59, 20, 6, 20, 10, 2024);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Inisialisasi relay
  pinMode(relayPin, OUTPUT); // Mengatur pin relay sebagai OUTPUT
  digitalWrite(relayPin, HIGH); // Mematikan relay awalnya

  // Koneksi ke WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Bunyikan buzzer selama 1 detik setelah perangkat terhubung ke WiFi
  digitalWrite(buzzerPin, HIGH);
  delay(1000); // Bunyikan buzzer selama 1 detik
  digitalWrite(buzzerPin, LOW);

  // Mulai server
  server.on("/", handleRoot);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Perbarui waktu RTC
  myRTC.updateTime();
  server.handleClient();
  Serial.println("----------------- Jam -----------------");
  // Tampilkan tanggal / waktu di Serial Monitor
  Serial.print("Tanggal / Waktu: ");
  Serial.print(myRTC.dayofmonth); // menampilkan tanggal
  Serial.print("/");
  Serial.print(myRTC.month);      // menampilkan bulan
  Serial.print("/");
  Serial.print(myRTC.year);       // menampilkan tahun
  Serial.print(" ");
  Serial.print(myRTC.hours);      // menampilkan jam
  Serial.print(":");
  Serial.print(myRTC.minutes);    // menampilkan menit
  Serial.print(":");
  Serial.println(myRTC.seconds);   // menampilkan detik
  Serial.println("----------------- Variabel -----------------");
  Serial.print("Jam : ");
  Serial.print(targetHour);
  Serial.println();
  Serial.print("Menit : ");
  Serial.print(targetMin);
  Serial.println();
  Serial.print("Delay : ");
  Serial.print(delayDuration);
  Serial.println();
  Serial.print("Target Date : ");
  Serial.print(targetDate);
  Serial.println();

  // Jika waktu saat ini sama dengan targetHour dan targetMin, bunyikan buzzer dan aktifkan relay
  if (myRTC.hours == targetHour && myRTC.minutes == targetMin && myRTC.dayofmonth == targetDate) {
    Serial.println("Alarm triggered!");
    digitalWrite(buzzerPin, HIGH);
    delay(2000);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, LOW); // Mengaktifkan relay
    delay(delayDuration*60000); // Menggunakan delay duration yang ditentukan sebelum matikan relay
    digitalWrite(relayPin, HIGH); // Mematikan relay setelah penundaan
    if (myRTC.dayofmonth == 28) {
      targetDate = 1;
    } else if (myRTC.dayofmonth == 30) {
      targetDate = 1;
    } else if (myRTC.dayofmonth == 31) {
      targetDate = 1;
    } else {
      targetDate = myRTC.dayofmonth + 1;
    }
  }
}

void handleRoot() {
  server.send(200, "text/html", "<!DOCTYPE html><html><head><title>Setting Alarm</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><style>body{font-family:Arial,sans-serif;background-color:#3498db;margin:0;padding:0;}.container{display:flex;justify-content:center;align-items:center;height:100vh;}.form-container{background-color:#fff;padding:20px;border-radius:10px;text-align:center;width:80%;max-width:400px;}.form-input{margin-bottom:20px;}.input-label{font-weight:bold;}.input-field{width:100px;padding:10px;border-radius:5px;border:1px solid black;margin-left:10px;text-align:center;}.button-container{text-align:center;}.button{width:150px;padding:15px;border-radius:30px;background-color:#4CAF50;color:#fff;border:none;font-size:16px;cursor:pointer;}.button:hover{background-color:#45a049;}</style><script>function showNotification() { alert('Data Terkirim'); }</script></head><body><div class=\"container\"><div class=\"form-container\"><h1>Setting Alarm</h1><form action=\"/set\" method=\"post\"><div class=\"form-input\"><label class=\"input-label\">Jam</label><input class=\"input-field\" type=\"number\" name=\"hour\" min=\"0\" max=\"23\"></div><div class=\"form-input\"><label class=\"input-label\">Menit</label><input class=\"input-field\" type=\"number\" name=\"minute\" min=\"0\" max=\"59\"></div><div class=\"form-input\"><label class=\"input-label\">Berapa Menit Air Akan Hidup</label><input class=\"input-field\" type=\"number\" name=\"duration\" min=\"1\"></div><div class=\"button-container\"><button class=\"button\" type=\"submit\" onclick=\"showNotification();\">Set Alarm</button></div></form></div></div></body></html>");
}

void handleSet() {
  if (server.hasArg("hour")) {
    targetHour = server.arg("hour").toInt();
  }
  if (server.hasArg("minute")) {
    targetMin = server.arg("minute").toInt();
  }
  if (server.hasArg("duration")) {
    delayDuration = server.arg("duration").toInt();
  }
  if (targetHour != 0 && targetMin != 0 && delayDuration != 0) {
    digitalWrite(buzzerPin, HIGH);
    delay(2000);
    digitalWrite(buzzerPin, LOW);
    targetDate = myRTC.dayofmonth;
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
