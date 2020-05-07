#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "FS.h"
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


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

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
uint8_t HEAD=4;
//uint8_t HEAD = 8;
uint8_t LF1=15;
uint8_t LF2=14;
uint8_t RF1=3;
uint8_t RF2=2;
uint8_t LB1=13;
uint8_t LB2=12;
uint8_t RB1=1;
uint8_t RB2=0;
double pulselength;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
char BLEbuf[32] = { 0 };
uint32_t cnt = 0;

#define SERVOMIN  102 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  512 // This is the 'maximum' pulse length count (out of 4096)
//#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
//#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

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
		pulselength = map(90 - int(AngleData["LF1"][i]) + LF1Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(LF1, 0, pulselength);
		pulselength = map(90 + int(AngleData["LF2"][i]) + LF2Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(LF2, 0, pulselength);
		pulselength = map(90 - int(AngleData["RF1"][i]) + RF1Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(RF1, 0, pulselength);
		pulselength = map(90 + int(AngleData["RF2"][i]) + RF2Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(RF2, 0, pulselength);
		pulselength = map(90 - int(AngleData["LB1"][i]) + LB1Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(LB1, 0, pulselength);
		pulselength = map(90 - int(AngleData["LB2"][i]) + LB2Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(LB2, 0, pulselength);
		pulselength = map(90 - int(AngleData["RB1"][i]) + RB1Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(RB1, 0, pulselength);
		pulselength = map(90 - int(AngleData["RB2"][i]) + RB2Offset, 0, 180, SERVOMIN, SERVOMAX);
		pwm.setPWM(RB2, 0, pulselength);

		delay(rate);
	}
}

void servoWrite(uint8_t servo, int angle) {
	pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
	pwm.setPWM(servo, 0, pulselength);
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

  if (!SPIFFS.begin()) {
	  Serial.println("Card Mount Failed");
	  return;
  }
  forwardData=readFile(SPIFFS, forwardPath);
  leftData = readFile(SPIFFS, leftPath);
  rightData = readFile(SPIFFS, rightPath);

  pwm.begin();
 pwm.setPWMFreq(50);
}

int headAngle=90;
void loop() {
	if (action == "S") {
		servoWrite(LF1,120  + LF1Offset);
		servoWrite(LF2,90  + LF2Offset);
		servoWrite(RF1,60  + RF1Offset);
		servoWrite(RF2,90 + RF2Offset);
		servoWrite(LB1,60  + LB1Offset);
		servoWrite(LB2,90  + LB2Offset);
		servoWrite(RB1,120 + RB1Offset);
		servoWrite(RB2,90 + RB2Offset);
		servoWrite(HEAD,90  + HEADOffset);
	}
	else if (action == "F") {
		doAction(forwardData);
	}
	else if (action == "L") {
		servoWrite(HEAD,45 + HEADOffset);
		doAction(leftData);	
	}
	else if (action == "R") {
		servoWrite(HEAD,135 + HEADOffset);
		doAction(rightData);	
	}
	else if (action == "1") {
		//headAngle = map(pwm.getPWM(HEAD), SERVOMIN, SERVOMAX, 0, 180);
		while (action == "1"&& headAngle <155) {
			headAngle = headAngle + 1;
			servoWrite(HEAD,headAngle + HEADOffset);
			Serial.print("headAngle: ");
		Serial.println(headAngle);
			delay(20);
		}
	}
	else if (action == "2") {
		//headAngle = map(pwm.getPWM(HEAD), SERVOMIN, SERVOMAX, 0, 180);
		while (action == "2"&&headAngle > 25 ) {
			headAngle = headAngle - 1;
			servoWrite(HEAD,headAngle + HEADOffset);
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
