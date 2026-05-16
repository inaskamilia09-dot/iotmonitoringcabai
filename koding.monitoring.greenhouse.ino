#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// =========================
// WIFI
// =========================
const char* ssid = "Milik Inas";
const char* password = "besokyabos";

// =========================
// DHT11
// =========================
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// =========================
// YL69
// =========================
#define SOIL_PIN 34

int nilaiKering = 3500;
int nilaiBasah  = 1200;

// =========================
// RELAY
// =========================
#define RELAY_PIN 23

bool statusPompa = false;

// =========================
// LED
// =========================
#define LED_HIJAU 18
#define LED_MERAH 19

// =========================
// WEB SERVER
// =========================
WebServer server(80);


// =================================================
// HALAMAN HTML
// =================================================

String HTMLPage(float suhu, int kelembapan) {

  String html = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta charset="UTF-8">

<meta name="viewport" content="width=device-width, initial-scale=1">

<meta http-equiv='refresh' content='3'>

<title>Smart Greenhouse</title>

<style>

*{
  margin:0;
  padding:0;
  box-sizing:border-box;

  font-family:
  Arial,
  "Segoe UI Emoji",
  "Noto Color Emoji",
  sans-serif;
}

body{

  background:linear-gradient(to right,#56ab2f,#a8e063);

  min-height:100vh;

  display:flex;

  justify-content:center;

  align-items:center;
}

/* CONTAINER */
.container{

  width:350px;
}

/* CARD */
.card{

  background:rgba(255,255,255,0.2);

  backdrop-filter:blur(10px);

  border-radius:20px;

  padding:25px;

  color:white;

  box-shadow:0 8px 20px rgba(0,0,0,0.2);
}

/* TITLE */
.title{

  text-align:center;

  margin-bottom:20px;

  font-size:28px;

  font-weight:bold;
}

/* DATA BOX */
.data-box{

  background:rgba(255,255,255,0.15);

  padding:15px;

  border-radius:15px;

  margin-top:15px;
}

/* LABEL */
.label{

  font-size:18px;
}

/* VALUE */
.value{

  font-size:30px;

  font-weight:bold;

  margin-top:5px;
}

/* STATUS */
.status-on{

  color:#00ff88;

  font-weight:bold;
}

.status-off{

  color:#ff4d4d;

  font-weight:bold;
}

/* PROGRESS BAR */
.progress{

  width:100%;

  height:20px;

  background:#ddd;

  border-radius:20px;

  overflow:hidden;

  margin-top:10px;
}

.progress-bar{

  height:100%;

  background:#00ff88;
}

/* FOOTER */
.footer{

  text-align:center;

  margin-top:20px;

  font-size:14px;

  line-height:22px;
}

</style>

</head>

<body>

<div class="container">

<div class="card">

<div class="title">
MONITORING<br>
MINI GREENHOUSE
</div>

<!-- SUHU -->
<div class="data-box">

<div class="label">
🌡 Suhu
</div>

<div class="value">
)rawliteral";

  html += suhu;

  html += R"rawliteral(
&deg;C
</div>

</div>

<!-- KELEMBAPAN -->
<div class="data-box">

<div class="label">
💧 Kelembapan Tanah
</div>

<div class="value">
)rawliteral";

  html += kelembapan;

  html += R"rawliteral(
%
</div>

<div class="progress">

<div class="progress-bar" style="width:)rawliteral";

  html += kelembapan;

  html += R"rawliteral(%;"></div>

</div>

</div>

<!-- STATUS POMPA -->
<div class="data-box">

<div class="label">
🚰 Status Pompa
</div>

<div class="value">
)rawliteral";

  if(statusPompa){

    html += "<span class='status-on'>ON</span>";
  }
  else{

    html += "<span class='status-off'>OFF</span>";
  }

  html += R"rawliteral(
</div>

</div>

<!-- FOOTER -->
<div class="footer">

Mini Greenhouse IoT ESP32<br>

Teknologi Industri Pertanian<br>

Universitas Brawijaya

</div>

</div>

</div>

</body>
</html>

)rawliteral";

  return html;
}

// =================================================
// HANDLE ROOT
// =================================================

void handleRoot() {

  // Suhu
  float suhu = dht.readTemperature() - 2;

  // Soil moisture
  int nilaiSensor = analogRead(SOIL_PIN);

  int kelembapan = map(
                        nilaiSensor,
                        nilaiKering,
                        nilaiBasah,
                        0,
                        100
                      );

  kelembapan = constrain(kelembapan, 0, 100);

  // Kontrol pompa
 if (kelembapan < 56 && !statusPompa) {

  digitalWrite(RELAY_PIN, LOW);

  digitalWrite(LED_HIJAU, HIGH);
  digitalWrite(LED_MERAH, LOW);

  statusPompa = true;
}

 else if (kelembapan > 80 && statusPompa) {

  digitalWrite(RELAY_PIN, HIGH);

  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, HIGH);

  statusPompa = false;
}

  // Kirim HTML
  server.send(200, "text/html", HTMLPage(suhu, kelembapan));
}


// =================================================
// SETUP
// =================================================

void setup() {

  Serial.begin(115200);

  dht.begin();

  analogReadResolution(12);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, HIGH);

  // WIFI
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Terhubung");

  // Tampilkan IP
  Serial.println(WiFi.localIP());

  // Web
  server.on("/", handleRoot);

  server.begin();

  Serial.println("Web Server Aktif");
}


// =================================================
// LOOP
// =================================================

void loop() {

  server.handleClient();
}