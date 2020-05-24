
#include <FS.h>   //Include File System Headers
#include <Arduino_JSON.h>
 #define COUNT_LOW 500
 #define COUNT_HIGH 2400

 #define TIMER_WIDTH 16
#include <ESP32Servo.h>
#include <WiFi.h>
//#include <Servo.h>

int HEADOffset = 0;
int LF1Offset = 0;
int LF2Offset = 6;
int RF1Offset =-7;
int RF2Offset = 0;
int LB1Offset = 0;
int LB2Offset = -2;
int RB1Offset =-4;
int RB2Offset = 0;

const char* ssid = "FAST_513E";
const char* password = "";
const uint16_t port = 9696;
WiFiServer wifiServer(port);

 Servo HEAD;
 Servo LF1;
 Servo LF2;
 Servo RF1;
 Servo RF2;
 Servo LB1;
 Servo LB2;
 Servo RB1;
 Servo RB2;
 
void setup() {
	Serial.begin(115200);
	delay(1000);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	Serial.println("Connected to the WiFi network");
	Serial.println(WiFi.localIP());

	wifiServer.begin();

	HEAD.setPeriodHertz(50);
	LF1.setPeriodHertz(50);
	LF2.setPeriodHertz(50);
	RF1.setPeriodHertz(50);
	RF2.setPeriodHertz(50);
	LB1.setPeriodHertz(50);
	LB2.setPeriodHertz(50);
	RB1.setPeriodHertz(50);
	RB2.setPeriodHertz(50);

	//node mcu
	//HEAD.attach(16, COUNT_LOW, COUNT_HIGH);
	//LF1.attach(5, COUNT_LOW, COUNT_HIGH);
	//LF2.attach(4, COUNT_LOW, COUNT_HIGH);
	//RF1.attach(0, COUNT_LOW, COUNT_HIGH);
	//RF2.attach(2, COUNT_LOW, COUNT_HIGH);
	//LB1.attach(14, COUNT_LOW, COUNT_HIGH);
	//LB2.attach(12, COUNT_LOW, COUNT_HIGH);
	//RB1.attach(13, COUNT_LOW, COUNT_HIGH);
	//RB2.attach(15, COUNT_LOW, COUNT_HIGH);

	//esp32
  HEAD.attach(25, COUNT_LOW, COUNT_HIGH);
  LF1.attach(23, COUNT_LOW, COUNT_HIGH);
  LF2.attach(22, COUNT_LOW, COUNT_HIGH);
  RF1.attach(32, COUNT_LOW, COUNT_HIGH);
  RF2.attach(33, COUNT_LOW, COUNT_HIGH);
  LB1.attach(19, COUNT_LOW, COUNT_HIGH);
  LB2.attach(18, COUNT_LOW, COUNT_HIGH);
  RB1.attach(26, COUNT_LOW, COUNT_HIGH);
  RB2.attach(27, COUNT_LOW, COUNT_HIGH);

	Serial.println();

}

JSONVar jsonData;
//int ret = 0;


//400
const byte buffSize =2000;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

void loop() {
	WiFiClient client = wifiServer.available();
	if (client) {
		//Serial.println("client ok");
		while (client.connected()) {
			//Serial.println("connected() ok");
			while (client.available() > 0) {
				char x = client.read();
				if (x == startMarker) {
					client.readBytesUntil(endMarker, inputBuffer, buffSize);
					parseData();
					//client.flush();
					//memset(inputBuffer, 0, buffSize);
				}

				//Serial.println("available() ok");
				//char x = client.read();
				//if (x == endMarker) {
				//	readInProgress = false;
				//	newDataFromPC = true;
				//	inputBuffer[bytesRecvd] = 0;
				//	parseData();
				//	//reset
				//	client.flush();
				//	memset(inputBuffer, 0, buffSize);
				//}
				//if (readInProgress) {
				//	inputBuffer[bytesRecvd] = x;
				//	bytesRecvd++;
				//	if (bytesRecvd == buffSize) {
				//		bytesRecvd = buffSize - 1;
				//	}
				//}
				//if (x == startMarker) {
				//	bytesRecvd = 0;
				//	readInProgress = true;
				//}
			}
		}
	}
}
//<[{"objects": {"HEAD": 12}, "time": 0.0}]>
//<[{"objects": {"BODY": 0, "LB1": 0, "RF1": 0, "LB2": 0, "RF2": 0, "RB2": 0, "RB1": 0, "HEAD": 0, "LF1": 0, "LF2": 0}, "time": 0.0}]>

int angle =0;
String servo = "";
void parseData() {
	Serial.print("input: ");
	Serial.println(inputBuffer);
	jsonData = JSON.parse(inputBuffer);
	Serial.print("json data: ");
	Serial.println(jsonData[0]);
	JSONVar k = jsonData[0]["objects"].keys();

	for (int i = 0; i < k.length(); i++) {
		
		String servo = JSON.stringify(k[i]);
		Serial.print("servo: ");
		Serial.println(servo);
		angle=jsonData[0]["objects"][k[i]];
		Serial.print("angle: ");
		Serial.println(angle);
		doAction(servo, angle);
	}

}

void doAction(String servo,int angle) {
	if (servo == "\"LF1\"") {
		angle = 90-angle+LF1Offset;
		LF1.write(angle);
		Serial.println("LF1");
		Serial.println(angle);
	}
	else if (servo == "\"LF2\""){
		angle = 90 + angle+LF2Offset;
		LF2.write(angle);
		Serial.println("LF2");
		Serial.println(angle);
	}
	else if (servo == "\"RF1\"") {
		angle = 90 - angle+RF1Offset;
		RF1.write(angle);
		Serial.println("RF1");
		Serial.println(angle);
	}
	else if (servo == "\"RF2\"") {
		angle = 90 + angle+RF2Offset;
		RF2.write(angle);
	}
	else if (servo == "\"LB1\"") {
		angle = 90 - angle+LB1Offset;
		LB1.write(angle);
	}
	else if (servo == "\"LB2\"") {
		angle = 90 - angle+LB2Offset;
		LB2.write(angle);
	}
	else if (servo == "\"RB1\"") {
		angle = 90 - angle+RB1Offset;
		RB1.write(angle);
	}
	else if (servo == "\"RB2\"") {
		angle = 90 - angle+RB2Offset;
		RB2.write(angle);
	}
	else if (servo == "\"HEAD\"") {
		//angle = 90 - angle+HEADOffset;
		HEAD.write(90 - angle + HEADOffset);
		Serial.println("LF1");
		Serial.println(90 - angle + HEADOffset);
	}
	else {
		Serial.println("not find servo name");
	}


}
