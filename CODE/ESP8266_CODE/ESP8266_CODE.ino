// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "TMPL6F8IV_NSv"
#define BLYNK_TEMPLATE_NAME "DATN v2"
#define BLYNK_AUTH_TOKEN "Lv8T4HarRYJ0oEQDlNGvq2xJU0ewgKTi"
#define BLYNK_PRINT Serial

#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Cấu hình UART
#define RX 0
#define TX 2
SoftwareSerial uart(RX, TX);

// Cấu hình WiFi
char SSID[] = "P307777";
char PASS[] = "hongnhungbui";

ESP8266WebServer server(80);

// Cấu hình địa chỉ IP tĩnh
IPAddress local_IP(172, 20, 10, 4);  // IP tĩnh cho ESP trung tâm
IPAddress gateway(172, 20, 10, 1);
IPAddress subnet(255, 255, 255, 240);
IPAddress primaryDNS(8, 8, 8, 8);      // DNS chính
IPAddress secondaryDNS(8, 8, 4, 4);    // DNS phụ (tùy chọn)

// Biến
float nhiet_do, do_am, do_am_dat, cuong_do_sang, trang_thai_thiet_bi;

void handlePostData() {
  if (server.hasArg("nhiet_do") && server.hasArg("do_am")) {
    nhiet_do = server.arg("nhiet_do").toFloat();
    do_am = server.arg("do_am").toFloat();
  } else if (server.hasArg("do_am_dat")) {
    do_am_dat = server.arg("do_am_dat").toFloat();
  } else if (server.hasArg("cuong_do_sang")) {
    cuong_do_sang = server.arg("cuong_do_sang").toFloat();
  }

  Blynk.virtualWrite(V1, nhiet_do);
  Blynk.virtualWrite(V2, do_am);
  Blynk.virtualWrite(V3, do_am_dat);
  Blynk.virtualWrite(V4, cuong_do_sang);

  server.send(200, "text/plain", "Data received");
}

// Blynk Timer
BlynkTimer timer;

void myTimerEvent() {
  // Gửi mã bắt tay '@'
  uart.write('@');
  
  // Gửi dữ liệu
  uart.write((uint8_t*)&nhiet_do, sizeof(nhiet_do));
  uart.write((uint8_t*)&do_am, sizeof(do_am));
  uart.write((uint8_t*)&do_am_dat, sizeof(do_am_dat));
  uart.write((uint8_t*)&cuong_do_sang, sizeof(cuong_do_sang));
}

void Truyen_UART() {
  uart.printf("%f %f %f %f\n", nhiet_do, do_am, do_am_dat, cuong_do_sang);
}

void setup() {
  // Khởi tạo Serial và UART
  Serial.begin(9600);
  uart.begin(115200);

  // Cấu hình IP tĩnh
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Blynk.config(BLYNK_AUTH_TOKEN);
  server.on("/post-data", HTTP_POST, handlePostData);
  server.begin();

  // Khởi tạo Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, SSID, PASS);

  // Thiết lập Timer cho Blynk
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  // Chạy Blynk và Timer
  Blynk.run();
  timer.run();

  server.handleClient();

  Truyen_UART();
}