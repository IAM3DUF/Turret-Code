// Author: Ryan Robinson
// Remote Controller for turret with ESP-NOW on ESP32

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

//receiver MAC address
uint8_t receiver_address[] = {0xcc, 0x7b, 0x5c, 0x35, 0x22, 0x22};
//new MAC address
uint8_t new_mac_address[] = {0xcc, 0x7b, 0x5c, 0x35, 0x11, 0x11};

//controller variables
//angle control:
int16_t HorizontalAngle = 0;
int16_t TempHorizontalAngle = 0;
int16_t VerticalAngle = 0;
int16_t TempVerticalAngle = 0;
uint8_t HorizontalAnglePotPin = 32;
uint8_t VerticalAnglePotPin = 33;

//speed control:
uint16_t SpeedPotVal = 0;
uint8_t SpeedPotPin = 35;
//shoot control
bool shoot = 0;
uint8_t ShootButtonPin = 27;

//uint8_t LEDPin = 14;

//receiver variables (if we do 2-way communication)
bool WithinRange = 0;

//send on success
bool success;

//struct to send data
typedef struct struct_ControllerData {
  uint8_t HorizontalAngle;
  uint8_t VerticalAngle;
  bool shoot;
  uint8_t speed;
} struct_ControllerData;

typedef struct struct_TurretData {
  bool WithinRange;
} struct_TurretData;

//store controller data
struct_ControllerData ControllerReadings;

//store incoming data
struct_TurretData TurretReadings;

esp_now_peer_info_t peerInfo;

//callback when data sent
void OnDataSent(const uint8_t *macaddr, esp_now_send_status_t status) {
  esp_now_peer_info_t peerInfo;
  Serial.print("\r\nLast packet send status: \t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery success!" : "Delivery failure!");
  if (status == 0) {
    success = "Delivery success!";
  }
  else {
    success = "Delivery failure!";
  }
}

//callback when data received
void OnDataRecv(const uint8_t *macaddr, const uint8_t *IncomingData, int len) {
  memcpy(&TurretReadings, IncomingData, sizeof(TurretReadings));
  //Serial.print("Bytes Received: ");
  //Serial.println(len);
  WithinRange = TurretReadings.WithinRange;
  //Serial.println(WithinRange);
}

void setup(){
  //serial monitor initialization
  Serial.begin(115200);

  //initialize shoot button
  pinMode(ShootButtonPin, INPUT);

  //initialize horizontal and vertical angles (assume two potentiometers)
  pinMode(HorizontalAnglePotPin, INPUT);
  pinMode(VerticalAnglePotPin, INPUT);

  //initialize SpeedPotPin
  pinMode(SpeedPotPin, INPUT);

  //initialize LEDPin
  //pinMode(LEDPin, OUTPUT);

  //set device as a wifi station
  WiFi.mode(WIFI_STA);

  //change mac address
  esp_wifi_set_mac(WIFI_IF_STA, &new_mac_address[0]);

  //initialize ESP-NOW
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(100);
    //return;
  }

  //register send callback
  esp_now_register_send_cb(OnDataSent);

  //register peer
  memcpy(peerInfo.peer_addr, receiver_address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //add peer --> maybe make this loop?
  while (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer!");
    delay(100);
    //return;
  }

  //register callback when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  //test: D34 HIGH
  //digitalWrite(LEDPin, HIGH);

  //read sensor data
  HorizontalAngle = analogRead(HorizontalAnglePotPin);
  VerticalAngle = analogRead(VerticalAnglePotPin);
  //map them and set center
  HorizontalAngle = map(HorizontalAngle, 0, 4096, 10, 170); //Middle of horizontal is 80 --> 10 - 170
  VerticalAngle = map(VerticalAngle, 0, 4096, 10, 170) - 60; //Middle of vertical is 20 --> 10 - 170

  if (VerticalAngle < 0)
  {
    VerticalAngle = 0;
  }
  else if (VerticalAngle > 50)
  {
    VerticalAngle = 50;
  }

  shoot = digitalRead(ShootButtonPin); //Maybe debounce? Depends on shooting mechanism
  SpeedPotVal = analogRead(SpeedPotPin);

  //set values to send
  ControllerReadings.HorizontalAngle = HorizontalAngle;
  ControllerReadings.VerticalAngle = VerticalAngle;
  ControllerReadings.shoot = shoot;
  ControllerReadings.speed = map(SpeedPotVal, 0, 4096, 0, 36);

  Serial.println("Measurements:");
  Serial.println(ControllerReadings.HorizontalAngle);
  Serial.println(ControllerReadings.VerticalAngle);
  Serial.println(ControllerReadings.speed);
  Serial.println(shoot);

  //send message
  esp_err_t result = esp_now_send(receiver_address, (uint8_t *) &ControllerReadings, sizeof(ControllerReadings));
  /*
  if (result == ESP_OK)
  {
    Serial.println("Sent with success!");
  }
  else
  {
    Serial.println("Error sending the data!");
  }
  */

  //Update withinrange LED on the controller
  //digitalWrite(LEDPin, WithinRange);

  //delay(50); //Can shorten this delay
}