#include "FS.h"
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#define COUNT_LOW 500
#define COUNT_HIGH 2400

#define TIMER_WIDTH 16
#include <ESP32Servo.h>


int HEADOffset = 0;
int LF1Offset = -8;
int LF2Offset = 0;
int RF1Offset =6;
int RF2Offset = 0;
int LB1Offset = 7;
int LB2Offset = -12;
int RB1Offset =-2;
int RB2Offset = 0;

Servo HEAD;
Servo LF1;
Servo LF2;
Servo RF1;
Servo RF2;
Servo LB1;
Servo LB2;
Servo RB1;
Servo RB2;

const char *path = "/test.txt";
const byte buffSize = 80000;
char inputBuffer[buffSize];
String StringBuffer;
byte bytesRecvd = 0;
JSONVar jsonData;
int angle = 0;

void parseData() {
	Serial.print("input: ");
	Serial.println(StringBuffer);
	jsonData = JSON.parse(StringBuffer);
	Serial.print("json data: ");
	Serial.println(jsonData);

	JSONVar k = jsonData[0]["objects"].keys();
  Serial.println(jsonData.length());
  Serial.println(jsonData[0]["objects"]["LB1"]);
	for (int t = 0; t < jsonData.length(); t++) {
		for (int i = 0; i < k.length(); i++) {

			String servo = JSON.stringify(k[i]);
			Serial.print("servo: ");
			Serial.println(servo);
			angle = jsonData[t]["objects"][k[i]];
			Serial.print("angle: ");
			Serial.println(angle);
			doAction(servo, angle);
		}
		//delay for movement
		delay(620);
    if(t==jsonData.length()-1){
      t=-1;
    }
	}
}

void doAction(String servo, int angle) {
	if (servo == "\"LF1\"") {
		angle = 90 - angle + LF1Offset;
		LF1.write(angle);
		Serial.println("LF1");
		Serial.println(angle);
	}
	else if (servo == "\"LF2\"") {
		angle = 90 + angle + LF2Offset;
		LF2.write(angle);
		Serial.println("LF2");
		Serial.println(angle);
	}
	else if (servo == "\"RF1\"") {
		angle = 90 - angle + RF1Offset;
		RF1.write(angle);
		Serial.println("RF1");
		Serial.println(angle);
	}
	else if (servo == "\"RF2\"") {
		angle = 90 + angle + RF2Offset;
		RF2.write(angle);
	}
	else if (servo == "\"LB1\"") {
		angle = 90 - angle + LB1Offset;
		LB1.write(angle);
	}
	else if (servo == "\"LB2\"") {
		angle = 90 - angle + LB2Offset;
		LB2.write(angle);
	}
	else if (servo == "\"RB1\"") {
		angle = 90 - angle + RB1Offset;
		RB1.write(angle);
	}
	else if (servo == "\"RB2\"") {
		angle = 90 - angle + RB2Offset;
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

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }else{
      int s = file.size();
        Serial.printf("File Size=%d\r\n", s);
    }

    Serial.println("- read from file:");
	//file.readBytes(inputBuffer, buffSize);
	if (file.available()) {
		StringBuffer = file.readString();
		Serial.println(StringBuffer);
		//JSONVar b = JSON.parse(a);
		//Serial.println(b);
	}
 //   while(file.available()){
 //       Serial.write(file.read());
	//	
	//	inputBuffer[bytesRecvd] = file.read();
	//	file.readBytes(inputBuffer, buffSize);
	//	if (bytesRecvd < buffSize) bytesRecvd++;
	//	
 //   }
	//bytesRecvd = 0;
	parseData();
	file.close();
}



void setup() {
  Serial.begin(115200);
  HEAD.setPeriodHertz(50);
  LF1.setPeriodHertz(50);
  LF2.setPeriodHertz(50);
  RF1.setPeriodHertz(50);
  RF2.setPeriodHertz(50);
  LB1.setPeriodHertz(50);
  LB2.setPeriodHertz(50);
  RB1.setPeriodHertz(50);
  RB2.setPeriodHertz(50);

  HEAD.attach(15, COUNT_LOW, COUNT_HIGH);
  LF1.attach(2, COUNT_LOW, COUNT_HIGH);
  LF2.attach(4, COUNT_LOW, COUNT_HIGH);
  RF1.attach(5, COUNT_LOW, COUNT_HIGH);
  RF2.attach(18, COUNT_LOW, COUNT_HIGH);
  LB1.attach(19, COUNT_LOW, COUNT_HIGH);
  LB2.attach(21, COUNT_LOW, COUNT_HIGH);
  RB1.attach(22, COUNT_LOW, COUNT_HIGH);
  RB2.attach(23, COUNT_LOW, COUNT_HIGH);

  Serial.println();
  // put your setup code here, to run once:
    if(!SPIFFS.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
delay(5000);
readFile(SPIFFS, path);
}
int pos = 0;
void loop() {
  // put your main code here, to run repeatedly:
	

	//for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
	//	// in steps of 1 degree
	//	HEAD.write(pos);    // tell servo to go to position in variable 'pos'
	//	delay(15);             // waits 15ms for the servo to reach the position
	//}
	//for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
	//	HEAD.write(pos);    // tell servo to go to position in variable 'pos'
	//	delay(15);             // waits 15ms for the servo to reach the position
	//}
}
