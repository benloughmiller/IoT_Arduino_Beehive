//Initial Configuration
  //Libraries
    //MQTT Libraries
      #include <ArduinoBearSSL.h>
      #include <ArduinoECCX08.h>
      #include <ArduinoMqttClient.h>
      #include <WiFiNINA.h>
      #include <ArduinoJson.h>

    //Link Internet Connection Data File
      #include "Connection_Data.h"
  
    //Sensor Libraries
      #include <DHT.h>
      #include <RTClib.h>
      #include <Wire.h>

  //Pulls from Connection_Data.h
    const char ssid[] = SSID;
    const char pass[] = PASSWORD;
    const char broker[] = BROKER;
    const char* certificate = CERTIFICATE;
    const char name[] = DEVICENAME;
    const char id[] = USERID;

  //Sensor Configuration
    #define DHTPIN_1 4
    #define DHTPIN_2 5
    #define DHTPIN_3 6
    #define DHTPIN_4 7
    #define DHTTYPE DHT22
    const int vibr_Pin =3;
    DHT dht1(DHTPIN_1, DHTTYPE);
    DHT dht2(DHTPIN_2, DHTTYPE);
    DHT dht3(DHTPIN_3, DHTTYPE);
    DHT dht4(DHTPIN_4, DHTTYPE);
    char timestamp[32];
    RTC_DS3231 rtc;
    float hum;
    float temp;
    float tempF;

  //Internet Configuration
    WiFiClient wifiClient;
    BearSSLClient sslClient(wifiClient); 
    MqttClient mqttClient(sslClient);
    unsigned long lastMillis = 0;
    DynamicJsonDocument jsonDocument(1024);

void setup() {
  Serial.begin(9600);

  //Initiates Certificate
    if (!ECCX08.begin()) {
      Serial.println("No ECCX08 present!");
      while (1);
    }
    ArduinoBearSSL.onGetTime(getTime);
    sslClient.setEccSlot(0, certificate);
    mqttClient.onMessage(onMessageReceived);

  // Humidity/Temp Sensors
    dht1.begin();
    dht2.begin();
    dht3.begin();
    dht4.begin();
  
  //Vibration Sensor
    pinMode(vibr_Pin, INPUT);

  //Real Time Clock
    Wire.begin();
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__),F(__TIME__))); 
}

void loop() {
  //Establishes connection to AWS
    if (WiFi.status() != WL_CONNECTED) {
      connectWiFi();
    }
    if (!mqttClient.connected()) {
      connectMQTT();
    }
    mqttClient.poll();

  //Runs code at 30 minute intervals
    if ((rtc.now().minute() == 0 || rtc.now().minute() == 30)) {

      //Real Time Clock
        DateTime now = rtc.now();
        sprintf(timestamp, "%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.day(), now.month(), now.year());  
        Serial.print(F("Date/Time: "));
        Serial.println(timestamp);

      // Reset JSON Document
        jsonDocument.clear();
        jsonDocument["tmestmp"] = timestamp;  

      //Adds the devicename to the top of the payload  
        jsonDocument["uid"] = id;
        jsonDocument["dname"] = name;

      //Humidity/Temp Sensors
        //External Sensor
          hum = dht1.readHumidity();
          temp = dht1.readTemperature();
          tempF = (temp * 9.0 / 5.0) + 32.0;
          Serial.print("External Sensor: Humidity: ");
          Serial.print(hum, 1);
          Serial.print("%, Temp: ");
          Serial.print(tempF, 1);
          Serial.println(" Fahrenheit");

          //Send to JSON
            JsonObject extsensor = jsonDocument.createNestedObject("extsensor");
            extsensor["hum"] = round(hum * 10.0) / 10.0;
            extsensor["temp"] = round(tempF * 10.0) / 10.0;
          
        //Bottom Sensor
          hum = dht2.readHumidity();
          temp = dht2.readTemperature();
          tempF = (temp * 9.0 / 5.0) + 32.0;
          Serial.print("Bottom Sensor: Humidity: ");
          Serial.print(hum, 1);
          Serial.print("%, Temp: ");
          Serial.print(tempF, 1);
          Serial.println(" Fahrenheit");

          //Send to JSON
            JsonObject botsensor = jsonDocument.createNestedObject("botsensor");
            botsensor["hum"] = round(hum * 10.0) / 10.0;
            botsensor["temp"] = round(tempF * 10.0) / 10.0;

        //Middle Sensor
          hum = dht3.readHumidity();
          temp = dht3.readTemperature();
          tempF = (temp * 9.0 / 5.0) + 32.0;
          Serial.print("Middle Sensor: Humidity: ");
          Serial.print(hum, 1);
          Serial.print("%, Temp: ");
          Serial.print(tempF, 1);
          Serial.println(" Fahrenheit");

          //Send to JSON
            JsonObject midsensor = jsonDocument.createNestedObject("midsensor");
            midsensor["hum"] = round(hum * 10.0) / 10.0;
            midsensor["temp"] = round(tempF * 10.0) / 10.0;


        //Top Sensor
          hum = dht4.readHumidity();
          temp = dht4.readTemperature();
          tempF = (temp * 9.0 / 5.0) + 32.0;
          Serial.print("Top Sensor: Humidity: ");
          Serial.print(hum, 1);
          Serial.print("%, Temp: ");
          Serial.print(tempF, 1);
          Serial.println(" Fahrenheit");

          //Send to JSON
            JsonObject topsensor = jsonDocument.createNestedObject("topsensor");
            topsensor["hum"] = round(hum * 10.0) / 10.0;
            topsensor["temp"] = round(tempF * 10.0) / 10.0;

      //Vibration Sensor
        long measurement = pulseIn(vibr_Pin, HIGH);
        Serial.print("Vibration: ");
        Serial.println(measurement, 2);

        // Publish Vibration Data
          jsonDocument["vibr"] = static_cast<float>(measurement);

      //JSON Upload Process
        // Convert JSON object to string
          String jsonString;
          serializeJson(jsonDocument, jsonString);  

        // Publish the combined message
          char combinedTopic[256];
          snprintf(combinedTopic, sizeof(combinedTopic), "%s/sensors", name);
          publishMessage(combinedTopic, jsonString.c_str());
          delay(60000);
    }
  
}

unsigned long getTime() {
  //Gets Time from WiFi Module  
    return WiFi.getTime(); 
}

void connectWiFi() {
  //Initiates Connection
    Serial.print("Attempting to connect to the network: ");
    Serial.print(ssid);
    Serial.print(" ");

  //Failed Connection
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
      Serial.print(".");
      delay(5000);
    }
  //Successful Connection
    Serial.println();
    Serial.println("You're connected to the network");
    Serial.println();
}

void connectMQTT() {
  //Initiates Connection
    Serial.print("Attempting to connect to AWS: ");
    Serial.print(broker);
    Serial.println(" ");

  //Failed Connection
    while (!mqttClient.connect(broker, 8883)) {
      Serial.print(".");
      delay(5000);
    }
  //Successful Connection
    Serial.println();
    Serial.println("You're connected to AWS");
    Serial.println();

  //Subscribes to Broadcast Channel
    mqttClient.subscribe("arduino/incoming");
}

void publishMessage(const char *topic, const char *message) {
  char maintopic[128];
  snprintf(maintopic, sizeof(maintopic), "%s/%s", id, topic);
  //Publishes Message
    Serial.print("Publishing message to topic: ");
    Serial.println(maintopic);
    mqttClient.beginMessage(maintopic);
    mqttClient.print(message);
    mqttClient.endMessage();
}

void onMessageReceived(int messageSize) {
  //Recieved Incoming Message
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

  //Prints the Contents
    while (mqttClient.available()) {
      Serial.print((char)mqttClient.read());
    }
    Serial.println();
    Serial.println();
}

