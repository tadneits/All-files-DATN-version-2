#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

const char* ssid = "P307777";
const char* password = "hongnhungbui";
const char* serverIP = "172.20.10.4"; // IP của ESP8266 trung tâm
const int serverPort = 80;

// Định nghĩa chân và kiểu cảm biến DHT
#define DHTPIN 5      // Cảm biến DHT nối với chân D4
#define DHTTYPE DHT22  // Kiểu cảm biến là DHT22 (AM2302)
float nhiet_do;
float do_am;

// Khởi tạo đối tượng DHT
DHT dht(DHTPIN, DHTTYPE);

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

  // Khởi động cảm biến DHT
  dht.begin();
}


void loop() {
  nhiet_do = dht.readTemperature();
  do_am = dht.readHumidity();

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    if (client.connect(serverIP, serverPort)) {
      http.begin(client, String("http://") + serverIP + "/post-data");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String postData = "nhiet_do=" + String(nhiet_do) + "&do_am=" + String(do_am);
      
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
