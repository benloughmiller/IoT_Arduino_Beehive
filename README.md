# IoT_Arduino_Beehive

### Introduction
- This project is designed for the Arduino MKR Wifi 1010 and AWS, but the code and procedure can be modified to fit your hardware and cloud requirements. It connects multiple temperature and humidity sensors, a real-time clock, and a vibration sensor, collects data from them, and uploads it to the IoT cloud on a repeating timer. Sensors can be added to each box in a hive, allowing for efficient data collection for your beehive.

### Arduino Library Requirements
- To program the Arduino, download the Arduino IDE software from the Arduino website. Open the software and install these libraries: "hx711," "DHT Sensor Library," "RTClib," "ArduinoBearSSL," "ArduinoECCX08," "ArduinoMqttClient," "WiFiNINA," and "ArduinoJson." If prompted to install extra libraries, install those too.

### Generate and Upload the Arduino CSR Certificate
- Before programming the Arduino, generate the CSR certificate. Follow these instructions until you successfully have the key: [Arduino CSR Guide](https://docs.arduino.cc/tutorials/mkr-wifi-1010/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core). Generate the key in slot 0 and copy it (including headers) to a text file. Save the file as a .csr.

- Go to AWS IoT Core, click "Devices," then "Add Device." Configure the device name, and on the device certificate screen, click "Upload CSR" and select your .csr file. Complete the device setup using the instructions from the guide and attach the policy that has already been created.

### Fill out the Connection_Data.h File
- Add your User ID and Device Name
- Add your WiFi SSID and Password
- Add your AWS Broker address
- Add your device certificate generated in the last step

### Device Build
#### Temperature and Humidity Sensors
- Use a wire with three connectors. Solder the black ground wire to the negative pin, the white (or green) data wire to the "out" pin, and the red positive wire to the positive pin. Isolate each soldered connection with Kapton tape, then seal it with heat shrink tubing.
  ![image](https://github.com/user-attachments/assets/28725f2e-6057-4d09-a28f-b99ddca827e9)

#### Vibration Sensor
- Solder a power wire to the VCC connection and a ground wire to ground. Connect a female-to-male wire to the Data Out (DO) connection.
  ![image](https://github.com/user-attachments/assets/75c7d9fa-974a-448f-b953-fef54244dbd4)

#### Connecting to Arduino
- Connect power wires to VCC and ground on the Arduino. Connect data wires to the corresponding pins on the Arduino based on the configuration set in the INO file.

### Housing the device
- For my prototype, I used a waterproof junction box to hold the arduino and vibration sensors, with wires exiting to USB and the Temp/Humidity sensors, but you can use any waterproof container to hold the components.
  
### Testing the Device
#### Arduino IDE
- To check if the Arduino is working, click the "Serial Monitor" icon in the top left corner of the IDE. With the Arduino plugged into the computer, it will display output from the device.

#### AWS
- In AWS IoT Core, go to the "Test" tab and open the "MQTT Test Client." Subscribe to the '[userID]/[DeviceName]/sensors' topic to view data being sent from the device.

