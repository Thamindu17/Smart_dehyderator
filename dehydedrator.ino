#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <SoftwareSerial.h>
#include "time.h"
#include "esp_sntp.h"

#define MQ2pin 25   
#define fan 26
#define heater 15
#define ONE_WIRE_BUS 13
#define DHTPIN1 14    
#define DHTPIN2 4
#define LED1 18
#define LED2 19
#define Buzzer 23
#define servoPin 27
SoftwareSerial mySerial(17, 16);
#define DHTTYPE DHT22  

const char* ssid = "M12";
const char* password = "12345678";


const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 19800;  
const int daylightOffset_sec = 0; 
const char *time_zone = "IST-5:30";  

time_t startTime;
time_t finishTime;
bool timing = false;


DHT dht1(DHTPIN1, DHTTYPE);  
DHT dht2(DHTPIN2, DHTTYPE);  


OneWire oneWire(ONE_WIRE_BUS);

const int LCD_ADDRESS = 0x27;
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

Servo myservo;


unsigned long startMillis;            
DallasTemperature sensors(&oneWire); 

WebServer server(80);
String data1 = "";
String data2 = "";
float temperature = 0.0;  
long timeDuration=0;     
long durationInSeconds =0;
int sensorValue; 
String inputBuffer = "";



void displayWaitingUntil(time_t finishTime) {  
  initializeLCD("Waiting until:", 0, 0);    
  printTimeToLCD(finishTime);
}


void printTimeToLCD(time_t time) {
  struct tm *timeinfo;
  timeinfo = localtime(&time);
  char buffer[16];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo); 
  initializeLCD(buffer, 0, 1); 
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void timeavailable(struct timeval *t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void startTiming() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    startTime = mktime(&timeinfo);
    finishTime = startTime + durationInSeconds;  
    timing = true;
    Serial.println("Timing started at:");
    printLocalTime();
    Serial.println("Finishing time will be:");
    printTime(finishTime);
  }
}

void printTime(time_t time) {
  struct tm *timeinfo;
  timeinfo = localtime(&time);
  Serial.println(asctime(timeinfo));
}

void updateSerial() {
    
   delay(500);
    while (Serial.available()) {
        mySerial.write(Serial.read());
    }  
    while (mySerial.available()) {
        Serial.write(mySerial.read());
    }
}

void sendSMS(String message) {    
    mySerial.println("AT+CMGS=\"+94768633308\""); // Replace with the actual phone number
    updateSerial();
   
    mySerial.print(message); 
    updateSerial();
   
    mySerial.write(26);
    updateSerial();
   
}
void handleRoot() {
  if (server.hasArg("data1") && server.hasArg("data2")) {   
    data1 = server.arg("data1");
    data2 = server.arg("data2");
    temperature = data1.toFloat(); 
    timeDuration = data2.toInt(); 
    Serial.print("Received data1 (temperature): ");
    Serial.println(temperature);
    Serial.print("Received data2 (time duration): ");
    Serial.println(timeDuration);  
    server.send(200, "text/plain", "Data received and processed");
  } else {   
    server.send(200, "text/plain", "Send data with 'data1' and 'data2' parameters");
  }
}

float temp(){
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    if (temperatureC == DEVICE_DISCONNECTED_C) {
          Serial.println("Error: Could not read temperature data");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Temp read error");
        } else {
          Serial.print("Temp In: ");
          Serial.print(temperatureC);
          Serial.println(" °C");
          char tempStr[16]; 
          snprintf(tempStr, sizeof(tempStr), "Temp In:%.2f C", temperatureC);
          initializeLCD(tempStr, 0, 0); 
          return temperatureC;
}
}

void initializeLCD(const char* message, int col, int row) { 
  Wire.begin();  
  lcd.init();  
  lcd.backlight();  
  delay(1000);
  lcd.clear();
  lcd.setCursor(col, row);  
  lcd.print(message);  
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {    
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }
  sntp_set_time_sync_notification_cb(timeavailable);
  esp_sntp_servermode_dhcp(1);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  Serial.println("Connected to WiFi");
  initializeLCD("Connected to WiFi",0,0);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  String ipStr = WiFi.localIP().toString();
  char ips[16];
  ipStr.toCharArray(ips, 16);  
  initializeLCD(ips, 0, 0);   
  server.on("/", handleRoot);
  server.begin();
  pinMode(MQ2pin, INPUT); 
  Serial.println("MQ2 warming up!");
  initializeLCD("gas checking...",0,0);
  delay(20000); 
  myservo.attach(servoPin);
  myservo.write(0);
  delay(500); 
  pinMode(fan, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  dht1.begin();
  dht2.begin();
  sensors.begin();
  mySerial.begin(9600);
  Serial.println("Initializing...");
  delay(1000);   
  mySerial.println("AT"); 
  updateSerial();
  delay(100);    
  mySerial.println("AT+CMGF=1"); 
  updateSerial();  
  delay(1000); 
  while (true) {
    myservo.write(0);
    server.handleClient();   
    durationInSeconds=timeDuration*60;
    startTiming();
    if (temperature > 0.0 && timeDuration > 0) {        
      sensorValue = digitalRead(MQ2pin);       
      Serial.println(sensorValue);
      if (sensorValue==0) {
         digitalWrite(heater, LOW);
         Serial.println("Gas detected");
         initializeLCD("Gas detected!!!",0,0);          
         myservo.write(180);
         delay(1000); 
         digitalWrite(fan, HIGH);                 
      } else {
        myservo.write(0);  
        digitalWrite(fan, LOW);
        Serial.println("No gas detected");
        initializeLCD("Process starting...",0,0);
        Serial.println(temperature);  
        Serial.println(timeDuration);     
        while(true){          
               if (timing) {
        time_t now;
        struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      now = mktime(&timeinfo);
      if (now >= finishTime) {
        digitalWrite(heater, LOW);
        Serial.println("Finishing time reached!");
        initializeLCD("Finishing time ",0,0);
        initializeLCD("reached!",0,1);
 
        while(true){
          float dhtTemperatureC = dht1.readTemperature(); // temp outside          
          float humidity = dht1.readHumidity();// huminity outside              
          float humidityI = dht2.readHumidity()+5.0;// huminity inside
         if (isnan(dhtTemperatureC) || isnan(humidity)) {
            Serial.println("outside DHT22 error!");
            initializeLCD("outside DHT22 error!", 0, 0);
          } else {              
              Serial.println("Humidity outside: ");
              Serial.print(humidity);
              Serial.print(" %\t");
              Serial.print("DHT Temperature OUTSIDE: ");
              Serial.print(dhtTemperatureC);
              Serial.print(" °C ");
             
              char TStr[16];  
              snprintf(TStr, sizeof(TStr), "Temp out:%.2f ",dhtTemperatureC );
              initializeLCD(TStr, 0, 0);
            }
            //Inside Humidity           
            if (isnan(humidityI)) {
              Serial.println("Failed to read from inside  DHT sensor!");
              initializeLCD("inside DHT22 error!", 0, 0); 
            } else {
              Serial.print("Humidity inside: ");
              Serial.print(humidityI);
              Serial.print(" %\t");               
            }
              char HStr[16];  
              snprintf(HStr, sizeof(HStr), "H. out :%.2f ", humidity);
              initializeLCD(HStr, 0, 0);
              char HIStr[16]; 
              snprintf(HIStr, sizeof(HIStr), "H. in:%.2f ",humidityI );
              initializeLCD(HIStr, 0, 0);   
          
          float t3=temp();  
          if ((dhtTemperatureC + 3.0 < t3) || (humidityI > humidity)) {
            initializeLCD("System cool down...", 0, 0); 
            digitalWrite(heater, LOW);
            myservo.write(180);
            digitalWrite(fan, HIGH);
            //delay(5000);
          }else{
          myservo.write(0);  
          digitalWrite(heater, LOW);
          digitalWrite(fan, LOW);
          digitalWrite(LED1, HIGH);
          Serial.println("sending message");
          sendSMS("process over");
          //delay(15000); 
          while(true){
            initializeLCD("process over.....", 0, 0);
            digitalWrite(LED1, HIGH);
            digitalWrite(Buzzer, HIGH);
            delay(3000);
            digitalWrite(LED1, LOW);
            digitalWrite(Buzzer, LOW);
          }         
          }
        }
        timing = false;
      } else {
        Serial.print("Current time: ");
        printTime(now);
        Serial.print("Waiting until: ");        
        printTime(finishTime);
        displayWaitingUntil(finishTime);
        Serial.println("dehydrating on");
        initializeLCD("dehydrating on...", 0, 0);
        Serial.println("heating to:");        
        Serial.print(temperature);
        char Utemp[16]; 
        snprintf(Utemp, sizeof(Utemp), "Heating to:%.2f ",temperature );
        initializeLCD(Utemp, 0, 0); 
        float t2 = temp();
        if(t2>70){
              myservo.write(180);  
              digitalWrite(LED2, HIGH);
              digitalWrite(Buzzer, HIGH);              
              sendSMS("Over heating...");
              initializeLCD("Over heating", 0, 0);          
              exit(0);
        }
        else if(t2 < temperature) {
              myservo.write(0);  
              Serial.println("heater on....");
              initializeLCD("Heater on", 0, 0);  
              digitalWrite(heater, HIGH); 
            } else {
              myservo.write(0);  
              Serial.println("heater off....");
              initializeLCD("Heater off", 0, 0);
              digitalWrite(heater, LOW);             
            }
          } 
      }
    }  
      delay(1000); 
      }
    }
    delay(2000);
  }  
  delay(2000);
}
}
void loop(){
}