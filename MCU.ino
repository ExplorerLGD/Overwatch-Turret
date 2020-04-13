
#include <FS.h>   //Include File System Headers
#include <Arduino_JSON.h>
 #define COUNT_LOW 500
 #define COUNT_HIGH 2400

 #define TIMER_WIDTH 16
#include <ESP32Servo.h>

String temp = "";
 Servo myservo;
void setup() {
	Serial.begin(115200);

	myservo.setPeriodHertz(50);
  myservo.attach(18, COUNT_LOW, COUNT_HIGH);
	Serial.println();

}

JSONVar jsonData;
int ret = 0;

char inByte[20000];
//String inByte="";
const byte buffSize = 400;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

void loop() {
	while (Serial.available() > 0)
	{
		char x = Serial.read();
		if (x == endMarker) {
			readInProgress = false;
			newDataFromPC = true;
			inputBuffer[bytesRecvd] = 0;
			parseData();
		}
		if (readInProgress) {
			inputBuffer[bytesRecvd] = x;
			bytesRecvd++;
			if (bytesRecvd == buffSize) {
				bytesRecvd = buffSize - 1;
			}
		}
		if (x == startMarker) {
			bytesRecvd = 0;
			readInProgress = true;
		}
	}
}

int angle =0;
void parseData() {
	Serial.print("input: ");
	Serial.println(inputBuffer);
	jsonData = JSON.parse(inputBuffer);
	Serial.print("json data: ");
	Serial.println(jsonData);
	angle =(int) jsonData[0]["objects"]["pCube1"]["rotateY"];
	//<[{"objects": {"pCube1": {"rotateY": 86}}, "time": 0.0}]>
	Serial.print("angle: ");
	Serial.println(angle);

	doAction(angle);
}
void doAction(int angle) {
	myservo.write(angle);
}
