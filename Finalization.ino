#include <Arduino.h>

#include <DHT.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <ThingSpeak.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHT11);

LiquidCrystal_I2C lcd(0x27,16,2);

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "Bender";
const char* password = "Rorschak42";

int myChannelNumber = 659365;
const char * myWriteAPIKey = "V51TL035LOE4EK7A";

WiFiClient  client;

int wifiStatus;

// Initialize our values
String myStatus = "";

//leds
const int rouge = 13;
const int vert = 12;
const int bleu = 14;

//led RVB
const int r = 15;
const int v = 0;
const int b = 16;

int cr;
int cv;
int cb;

int temperature;
int humidity;

void setup() {
  Serial.begin(115200);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Bender", "Rorschak42");    
     
     
  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Your are connecting to;");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  pinMode(rouge, OUTPUT);  
  pinMode(vert, OUTPUT);
  pinMode(bleu, OUTPUT);

  pinMode(r, OUTPUT);  
  pinMode(v, OUTPUT);
  pinMode(b, OUTPUT);
  
  //demarrage du capteur DHT11
  dht.begin();

  lcd.init();
  lcd.backlight();
}

void loop() {
  delay(15000);
  
  wifiStatus = WiFi.status();
  
  float t = dht.readTemperature(); //mesure temperature (en ° celsius)
  float h = dht.readHumidity();   //mesure l'humidité (en %)
  temperature = (int)t;
  humidity = (int)h;
  
  //test si valeurs ont été récup
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!"); //affiche message d'erreur
    return;                                            //quitte pour retenter lecture
  }else{
    if(t>31 & t<40){
      cr = 255/225*1023;
      cv = 255-(t/8*255)/255*1023;
      cb = 0;
    }
    if(t>23 & t<32){
      cv = 255/225*1023;
      cr = 255-(t/8*255)/255*1023;
      cb = 0;
    }
    if(t>15 & t<24){
      cv = 255/225*1023;
      cb = 255-(t/8*255)/255*1023;
      cr = 0;
    }
    if(t>7 & t<16){
      cb = 255/225*1023;
      cv = 255-(t/8*255)/255*1023;
      cr = 0;
    }
    if(t<8 & t>0){
      cb = 255/225*1023;
      cr = 255-(t/8*255)/255*1023;
      cv = 0;
    }
    if(h>0 & h<50){
      digitalWrite(rouge, LOW);
      digitalWrite(vert, LOW);
      digitalWrite(bleu, HIGH);
    }
    if(h>49 & h<70){
      digitalWrite(rouge, LOW);
      digitalWrite(vert, HIGH);
      digitalWrite(bleu, LOW);
    }
    if(h>69 & h<120){
      digitalWrite(rouge, HIGH);
      digitalWrite(vert, LOW);
      digitalWrite(bleu, LOW);
    }
   analogWrite(r,cr);
   analogWrite(v,cv);
   analogWrite(b,cb);
  }

  // set the fields with the values
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  if(wifiStatus == WL_CONNECTED){
    
    Serial.println("");
    Serial.println("Your ESP is connected!");  
    Serial.println("Your IP address is: ");
    Serial.println(WiFi.localIP());
  
   
    lcd.setCursor(0,0);
    lcd.print("I'm       ");
    lcd.setCursor(12,0);
    lcd.print("* *");
    lcd.setCursor(0,1);
    lcd.print("connected!");
    lcd.setCursor(13,1);
    lcd.print("W");
    
    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
 
    JSONencoder["temperature"] = t;
    JSONencoder["humidite"] = h;
    
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
 
    HTTPClient http;    //Declare object of class HTTPClient
 
    http.begin("http://51.75.126.63/DHT11/ok.php");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
 
    int httpCode = http.POST(JSONmessageBuffer);   //Send the request
    String payload = http.getString();                                        //Get the response payload
 
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
 
    http.end();
   }else{
    
    Serial.println("");
    Serial.println("WiFi not connected");

    lcd.setCursor(0,0);
    lcd.print("I'm not");
    lcd.setCursor(12,0);
    lcd.print("* *");
    lcd.setCursor(0,1);
    lcd.print("connected ! ");
    lcd.setCursor(13,1);
    lcd.print("W");
   }
   
  delay(2000);
  lcd.setCursor(0,0);
  lcd.print("Temp:    *C");
  lcd.setCursor(6,0);
  lcd.print(temperature);
  lcd.setCursor(0,1);
  lcd.print("Humi:    % ");
  lcd.setCursor(6,1);
  lcd.print(humidity);
  delay(3583000);
}
