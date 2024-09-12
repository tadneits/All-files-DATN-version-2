#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <BH1750.h>

// Khởi tạo đối tượng BH1750
BH1750 lightMeter;
float lux;

const char* ssid = "P307777";
const char* password = "hongnhungbui";
const char* serverIP = "172.20.10.4"; // IP của ESP8266 trung tâm
const int serverPort = 80;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Khởi động giao tiếp I2C
  Wire.begin();

  // Khởi động cảm biến BH1750
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("Lỗi!"));
    while (1);
  }
}

void loop() {
  lux = lightMeter.readLightLevel();

  // In kết quả lên Terminal
  Serial.print(F("Cường độ ánh sáng: "));
  Serial.print(lux);
  Serial.println(F(" lx"));

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    if (client.connect(serverIP, serverPort)) {
      http.begin(client, String("http://") + serverIP + "/post-data");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      float cuong_do_sang = lux;
      String postData = "cuong_do_sang=" + String(cuong_do_sang);
      
      int httpResponseCode = http.POST(postData);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(response);
      }
      http.end();
    }
  }
  delay(1000); // Gửi dữ liệu mỗi 1 giây
}