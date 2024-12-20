// Author: Ryan Robinson
// Turret Receiver for turret with ESP-NOW on ESP32

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESP32Servo.h>
#include <HardwareSerial.h>

//receiver MAC address
uint8_t receiver_address[] = {0xcc, 0x7b, 0x5c, 0x35, 0x11, 0x11};
//new MAC address
uint8_t new_mac_address[] = {0xcc, 0x7b, 0x5c, 0x35, 0x22, 0x22};

//controller variables (received data)
//angle control
uint8_t HorizontalAngle = 0;
uint8_t VerticalAngle = 0;

//speed control
uint8_t SpeedPotVal = 0;

//shoot control
bool shoot = 0;

//receiver variables
//range detection (if we do this)
bool WithinRange = 0;
//uint8_t TrigPin = 25;
//uint8_t EchoPin = 26;

//servos: for loading servo, esc, and turret control
Servo servo;
uint8_t servoPin = 32;
Servo esc;
uint8_t escPin = 33;
Servo horizontalServo;
uint8_t horizontalServoPin = 25;
Servo verticalServo;
uint8_t verticalServoPin = 26;

//send on success
bool success;

//struct to receive controller data
typedef struct struct_ControllerData {
  uint8_t HorizontalAngle;
  uint8_t VerticalAngle;
  bool shoot;
  uint8_t speed;
} struct_ControllerData;

//struct to send turret data
typedef struct struct_TurretData {
  bool WithinRange;
} struct_TurretData;

//store controller data
struct_ControllerData ControllerReadings;

//store incoming data
struct_TurretData TurretReadings;

//communication with nano
HardwareSerial SerialPort(1); //UART 1

esp_now_peer_info_t peerInfo;

//callback when data sent
void OnDataSent(const uint8_t *macaddr, esp_now_send_status_t status) {
  esp_now_peer_info_t peerInfo;
  Serial.print("Last packet send status: ");
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
  memcpy(&ControllerReadings, IncomingData, sizeof(ControllerReadings));
  Serial.print("Bytes Received: ");
  Serial.println(len);
  HorizontalAngle = ControllerReadings.HorizontalAngle;
  VerticalAngle = ControllerReadings.VerticalAngle;
  shoot = ControllerReadings.shoot;
  SpeedPotVal = ControllerReadings.speed * 5;
}

void setup(){

  //serial monitor initialization
  Serial.begin(115200);
  
  //communication to nano
  //SerialPort.begin(9600, SERIAL_8N1, 16, 17);

  //initialize servos
  servo.attach(servoPin);
  esc.attach(escPin);
  horizontalServo.attach(horizontalServoPin);
  verticalServo.attach(verticalServoPin);

  servo.write(110); //start at 0 degrees
  esc.write(10); //start at 0 degrees
  horizontalServo.write(45); //center horizontal servo
  verticalServo.write(25); //center vertical servo

  //initialize ultrasonic sensor
  //pinMode(TrigPin, OUTPUT);
  //pinMode(EchoPin, INPUT);
  
  //set device as a wifi station
  WiFi.mode(WIFI_STA);

  //change mac address
  esp_wifi_set_mac(WIFI_IF_STA, &new_mac_address[0]);

  //initialize ESP-NOW
  while (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    delay(1000);
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
    delay(1000);
    //return;
  }

  //register callback when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  Serial.println("Setup end!");
}

void loop() {

  //read sensor data (ultrasonic sensor --> WithinRange)
  //WithinRange = SendUSSPulse();
  WithinRange = 1;

  //set values to send
  TurretReadings.WithinRange = WithinRange;

  //send message (if target is within range)
  esp_err_t result = esp_now_send(receiver_address, (uint8_t *) &TurretReadings, sizeof(TurretReadings));
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
  Serial.println("Received Data:");
  Serial.println(HorizontalAngle);
  Serial.println(VerticalAngle);
  Serial.println(SpeedPotVal);
  Serial.println(shoot);

  //Update servos for turret control
  horizontalServo.write(HorizontalAngle);
  verticalServo.write(VerticalAngle);

  //Update ESC and shoot if shoot == 1
  esc.write(SpeedPotVal);
  if (shoot == 1) {
    //fire
    servo.write(100);
    delay(350);
    servo.write(10);
    delay(300);
  }
  else {
    servo.write(20);
  }

  //Serial.print((char)HorizontalAngle);
  //Serial.print((char)VerticalAngle);
  Serial.print((char)SpeedPotVal);
  Serial.print((char)shoot);

  delay(100); //Can shorten this delay
}

/*
bool SendUSSPulse() {
  //send sound
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  //measure duration of pulse
  float duration_us = pulseIn(EchoPin, HIGH);

  //calculate the distance
  float distance_cm = 0.017 * duration_us;

  //return if in range
  return (distance_cm < 100); //change within range value later
}
*/
