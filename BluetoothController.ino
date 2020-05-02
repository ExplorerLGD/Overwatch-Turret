#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "FS.h"
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <ESP32Servo.h>


int HEADOffset = 0;
int LF1Offset = 0;
int LF2Offset = 6;
int RF1Offset =-7;
int RF2Offset = 0;
int LB1Offset = 0;
int LB2Offset = -2;
int RB1Offset =-4;
int RB2Offset = 0;

const char *forwardPath = "/forward.txt";
const char *leftPath = "/left.txt";
const char *rightPath = "/right.txt";
String StringBuffer;
JSONVar forwardData;
JSONVar leftData;
JSONVar rightData;
String action="S";


Servo HEAD;
Servo LF1;
Servo LF2;
Servo RF1;
Servo RF2;
Servo LB1;
Servo LB2;
Servo RB1;
Servo RB2;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
char BLEbuf[32] = { 0 };
uint32_t cnt = 0;

#define COUNT_LOW 500
#define COUNT_HIGH 2400
//#define TIMER_WIDTH 16
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.print("------>Received Value: ");

      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
      }
      Serial.println();
	  if (rxValue.find("S") != -1) {
		  action = "S";
		  Serial.print("stop!");
	  }
	  else if (rxValue.find("0") != -1) {
		  action = "0";
		  Serial.print("stop action!");
	  }
      else if (rxValue.find("F") != -1) {
		  action = "F";
        Serial.print("forward!");
      }
      else if (rxValue.find("L") != -1) {
		  action = "L";
        Serial.print("left!");
      }
	  else if (rxValue.find("R") != -1) {
		  action = "R";
		  Serial.print("right!");
	  }
	  else if (rxValue.find("1") != -1) {
		  action = "1";
		  Serial.print("1!");
	  }
	  else if (rxValue.find("2") != -1) {
		  action = "2";
		  Serial.print("2!");
	  }
      Serial.println();
    }
  }
};

JSONVar readFile(fs::FS &fs, const char * path) {
	Serial.printf("Reading file: %s\r\n", path);

	File file = fs.open(path);
	if (!file || file.isDirectory()) {
		Serial.println("- failed to open file for reading");
		//return;
	}
	else {
		int s = file.size();
		Serial.printf("File Size=%d\r\n", s);
	}

	Serial.println("- read from file:");

	if (file.available()) {
		StringBuffer = file.readString();
		Serial.println(StringBuffer);
	}
	file.close();
	return JSON.parse(StringBuffer);
	//jsonData = JSON.parse(StringBuffer);
	//Serial.print("json data: ");
	//Serial.println(jsonData);
}
void doAction(JSONVar AngleData) {
	Serial.println("doaction");
	int t = int(AngleData["time"]);
	int rate = 1000 / int(AngleData["frameRate"]);
	for (int i = 0; i < t; i++) {
		LF1.write(90 - int(AngleData["LF1"][i]) + LF1Offset);
		//Serial.print("i: ");
		//Serial.println(i);
		//Serial.print("LF1: ");
		//Serial.println(90 - int(AngleData["LF1"][i]) + LF1Offset);
		LF2.write(90 + int(AngleData["LF2"][i]) + LF2Offset);
		RF1.write(90 - int(AngleData["RF1"][i]) + RF1Offset);
		RF2.write(90 + int(AngleData["RF2"][i]) + RF2Offset);
		LB1.write(90 - int(AngleData["LB1"][i]) + LB1Offset);
		LB2.write(90 - int(AngleData["LB2"][i]) + LB2Offset);
		RB1.write(90 - int(AngleData["RB1"][i]) + RB1Offset);
		RB2.write(90 - int(AngleData["RB2"][i]) + RB2Offset);
		//HEAD.write(90 - int(AngleData["HEAD"][i]) + HEADOffset);
		delay(rate);
	}
}
void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("ESP32 BLE Test");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  HEAD.setPeriodHertz(50);
  LF1.setPeriodHertz(50);
  LF2.setPeriodHertz(50);
  RF1.setPeriodHertz(50);
  RF2.setPeriodHertz(50);
  LB1.setPeriodHertz(50);
  LB2.setPeriodHertz(50);
  RB1.setPeriodHertz(50);
  RB2.setPeriodHertz(50);

  HEAD.attach(25, COUNT_LOW, COUNT_HIGH);
  LF1.attach(23, COUNT_LOW, COUNT_HIGH);
  LF2.attach(22, COUNT_LOW, COUNT_HIGH);
  RF1.attach(32, COUNT_LOW, COUNT_HIGH);
  RF2.attach(33, COUNT_LOW, COUNT_HIGH);
  LB1.attach(19, COUNT_LOW, COUNT_HIGH);
  LB2.attach(18, COUNT_LOW, COUNT_HIGH);
  RB1.attach(26, COUNT_LOW, COUNT_HIGH);
  RB2.attach(27, COUNT_LOW, COUNT_HIGH);

  if (!SPIFFS.begin()) {
	  Serial.println("Card Mount Failed");
	  return;
  }
  forwardData=readFile(SPIFFS, forwardPath);
  leftData = readFile(SPIFFS, leftPath);
  rightData = readFile(SPIFFS, rightPath);
}

int headAngle=90;
void loop() {
	if (action == "S") {
		LF1.write(120  + LF1Offset);
		LF2.write(90  + LF2Offset);
		RF1.write(60  + RF1Offset);
		RF2.write(90 + RF2Offset);
		LB1.write(60  + LB1Offset);
		LB2.write(90  + LB2Offset);
		RB1.write(120 + RB1Offset);
		RB2.write(90 + RB2Offset);
		HEAD.write(90  + HEADOffset);
	}
	else if (action == "F") {
		doAction(forwardData);
	}
	else if (action == "L") {
		HEAD.write(45 + HEADOffset);
		doAction(leftData);	
	}
	else if (action == "R") {
		HEAD.write(135 + HEADOffset);
		doAction(rightData);	
	}
	else if (action == "1") {
		headAngle = HEAD.read();
		while (action == "1"&& headAngle <155) {
			headAngle = headAngle + 1;
			HEAD.write(headAngle + HEADOffset);
			Serial.print("headAngle: ");
		Serial.println(headAngle);
			delay(20);
		}
	}
	else if (action == "2") {
		headAngle = HEAD.read();
		while (action == "2"&&headAngle > 25 ) {
			headAngle = headAngle - 1;
			HEAD.write(headAngle + HEADOffset);
			Serial.print("headAngle: ");
			Serial.println(headAngle);
			delay(20);
		}
	}
  //if (deviceConnected) {
  //  memset(BLEbuf, 0, 32);
  //  memcpy(BLEbuf, (char*)"Hello BLE APP!", 32);
  //  pCharacteristic->setValue(BLEbuf);

  //  pCharacteristic->notify(); // Send the value to the app!
  //  Serial.print("*** Sent Value: ");
  //  Serial.print(BLEbuf);
  //  Serial.println(" ***");
  //}
  //delay(1000);
}
