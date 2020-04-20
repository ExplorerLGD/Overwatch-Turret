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
}

void doAction(JSONVar AngleData) {
  Serial.println("doaction");  
  int t=int(AngleData["time"]);
  int rate=1000/int(AngleData["frameRate"]);
  for(int i=0;i<t;i++){
    LF1.write(90 - int(AngleData["LF1"][i]) + LF1Offset);
    Serial.print("i: ");
    Serial.println(i);
    Serial.print("LF1: ");
    Serial.println(90 - int(AngleData["LF1"][i]) + LF1Offset);
    LF2.write(90 + int(AngleData["LF2"][i]) + LF2Offset);
    RF1.write(90 - int(AngleData["RF1"][i]) + RF1Offset);
    RF2.write(90 + int(AngleData["RF2"][i]) + RF2Offset);
    LB1.write(90 - int(AngleData["LB1"][i]) + LB1Offset);
    LB2.write(90 - int(AngleData["LB2"][i]) + LB2Offset);
    RB1.write(90 - int(AngleData["RB1"][i]) + RB1Offset);
    RB2.write(90 - int(AngleData["RB2"][i]) + RB2Offset);
    HEAD.write(90 - int(AngleData["HEAD"][i]) + HEADOffset);
    delay(rate);
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
doAction(jsonData);
}
