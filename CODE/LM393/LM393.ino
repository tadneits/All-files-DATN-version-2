#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "P307777";
const char* password = "hongnhungbui";
const char* serverIP = "172.20.10.4"; // IP của ESP8266 trung tâm
const int serverPort = 80;

const int dryValue = 1023; // Giá trị analog khi đất khô (thay đổi tùy theo cảm biến của bạn)
const int wetValue = 0;    // Giá trị analog khi đất ướt (thay đổi tùy theo cảm biến của bạn)

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}

void loop() {
  int sensorValue = analogRead(A0);  // Đọc giá trị điện áp từ chân A0
  
  // Chuyển đổi giá trị analog thành % độ ẩm đất
  int moisturePercentage = map(sensorValue, dryValue, wetValue, 0, 100);
  
  // Đảm bảo giá trị không vượt quá 0-100%
  moisturePercentage = constrain(moisturePercentage, 0, 100);

  Serial.print("Độ ẩm đất: ");
  Serial.print(moisturePercentage);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    if (client.connect(serverIP, serverPort)) {
      http.begin(client, String("http://") + serverIP + "/post-data");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      float do_am_dat = moisturePercentage;
      String postData = "do_am_dat=" + String(do_am_dat);
      
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