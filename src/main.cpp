#include <ArduinoJson.h>
#include "Keypad.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
LiquidCrystal_I2C lcd(0x27,20,4); 

#define pinDir 16
#define pinStep 17
const char* ssid ="VNPT";
const char* password = "123456788";
 
// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "HuyDQ/syringepump/H2811";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

//button
uint8_t Count = 1;
uint8_t Flag = 0;
uint8_t Flag1 = 0;
uint8_t Flag_ss = 0 ;
uint8_t Flag_Start = 0;
//
int MaSo = 0;
int LieuLuong = 0;
int TocDo = 0;
//tiền khai báo
void mqtt_JSON();
void lcd_setup();
void button_setup();
int read_button();
void step_setup();
void setup_pinTest();
void mqtt_setup();
void Start_Setup();
void Setup_InF();
void Led_setup();
//cac ham reset
//covert


void setup() {
  Serial.begin(9600);//bật serial, baudrate 9600
  mqtt_setup();
  lcd_setup();
  button_setup();
  lcd_setup();
  step_setup();
  setup_pinTest();
  Led_setup();
  digitalWrite(5,HIGH);
  digitalWrite(26,HIGH);
  digitalWrite(33,LOW);
  int i; 
  digitalWrite(16,LOW); //
  for(i = 0  ; digitalRead(18)==1; i++) // Cho chay 1 vong
  {
    digitalWrite(17,HIGH); // Output high
    delayMicroseconds(1200); // chờ
    digitalWrite(17,LOW); // Output low
    delayMicroseconds(1200); // chờ
    if(i >8100)
    {
      Flag1 = 1;
      break;
    }
  }
  delay(100);
  digitalWrite(16,HIGH); //
  for(i = 0  ;digitalRead(19)==1; i++) // Cho chay 1 vong
  {
    digitalWrite(17,HIGH); // Output high
    delayMicroseconds(1200); // chờ
    digitalWrite(17,LOW); // Output low
    delayMicroseconds(1200); // chờ
     if(i >8100)
    {
      Flag1 = 1;
       break;
    }
  }

}

void loop(){
 
  if(Flag1)
  {
    digitalWrite(26,LOW);
    digitalWrite(33,HIGH);
    delay(2000);
    digitalWrite(33,LOW);
    delay(1000);
  }
  else
  {
  digitalWrite(5,LOW);
  Setup_InF();
  while (Count == 4)
  {
    
    if(read_button()==5)
    {
      Start_Setup();
      delay(1000);
      LieuLuong = 0;
      MaSo = 0;
      TocDo = 0;
      Count = 1;
    }
  }
  }
  

}
void lcd_setup()
{
  lcd.init();                    
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(" Trang thai  : Trong");
  lcd.setCursor(1,1);
  lcd.print("Ma So      :");
  lcd.setCursor(1,2);
  lcd.print("Lieu luong :");
  lcd.setCursor(1,3);
  lcd.print("Toc do     :");
}

void button_setup()
{
  pinMode( 4,INPUT);
  pinMode( 13,INPUT);
  pinMode( 15,INPUT);
  pinMode( 25,INPUT);
  pinMode( 27,INPUT);
}
int read_button()
{
  if(!digitalRead(4))
  {
    delay(10);
    if(!digitalRead(4))
    {
      return 1;
    }
  }
  if(!digitalRead(25))
  {
    delay(10);
    if(!digitalRead(25))
    {
      return 2;
    }
  }
  if(!digitalRead(27))
  {
    delay(10);
    if(!digitalRead(27))
    {
      return 3;
    }
  }
  if(!digitalRead(13))
  {
    
    delay(10);
    if(!digitalRead(13))
    {
      return 4;
    }
  }
  if(!digitalRead(15))
  {
    
    delay(10);
    if(!digitalRead(15))
    {
      return 5;
    }
  }
  else return 0;
}
void step_setup()
{
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
}
void Start_Setup()
{
  int Sovong =LieuLuong*324;
  if(Sovong >8100){
    lcd.setCursor(14,2);
    lcd.print("Error");
    Flag_Start = 1;
  }
  int Vantoc =15432/TocDo;
  if(Vantoc <1200){
    lcd.setCursor(3,0);
    lcd.print("Error");
    Flag_Start = 1;
  }
  if(Flag_Start == 1)
  {
    lcd.setCursor(13,0);
    lcd.print("      ");
    lcd.setCursor(14,0);
    lcd.print("Error");
    return ;
  }
  Serial.print ("socvong :");
  Serial.println(Sovong);
  Serial.print ("vantoc :");
  Serial.println(Vantoc);
  lcd.setCursor(14,0);
  lcd.print("      ");
  lcd.setCursor(14,0);
  lcd.print("setup");
  int i = 0;
  digitalWrite(16,LOW); // 
  for(i = 0  ; i <Sovong; i++) // Cho chay 1 vong
  {
    digitalWrite(17,HIGH); // Output high
    delayMicroseconds(1200); // chờ
    digitalWrite(17,LOW); // Output low
    delayMicroseconds(1200); // chờ
  }
  digitalWrite(33,HIGH);
  delay(500);
  digitalWrite(33,LOW);
  Serial.println("Pause");
  lcd.setCursor(14,0);
  lcd.print("Ready");
  while(read_button()!= 4
  );
  lcd.setCursor(14,0);
  lcd.print("      ");
  lcd.setCursor(14,0);
  lcd.print("      ");
  lcd.setCursor(14,0);
  lcd.print("Run");
  i = 0;
  digitalWrite(16,HIGH); // 
  for(i = 0  ; i <Sovong; i++) // Cho chay 1 vong
  {
    digitalWrite(17,HIGH); // Output high
    delayMicroseconds(Vantoc); // chờ
    digitalWrite(17,LOW); // Output low
    delayMicroseconds(Vantoc); // chờ
  }
  digitalWrite(33,HIGH);
  delay(500);
  digitalWrite(33,LOW);
  Serial.println("XONG");
  mqtt_JSON();
  lcd.setCursor(14,0);
  lcd.print("      ");
  lcd.setCursor(14,0);
  lcd.print("Xong");

}
void setup_pinTest()
{
  pinMode(18,INPUT);
  pinMode(19,INPUT);
}
void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}
void mqtt_setup()
{
  // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.publish(topic, "Hi EMQX I'm ESP32 ^^");
 client.subscribe(topic);
} 

void mqtt_JSON(){
StaticJsonDocument<256> doc;
delay(100);
doc["TenThuoc"] = MaSo;
doc["LieuLuong"] = LieuLuong;
doc["TocDo"] = TocDo;
//doc["data"]=data;
// Generate the minified JSON and send it to the Serial port.
//
char out[128];
int b =serializeJson(doc, out);
Serial.print("bytes = ");
Serial.println(b,DEC);
client.publish(topic, out);
// The above line prints:
// {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
}
void Setup_InF()
{
  lcd.setCursor(0,1);
  lcd.print("*");
  lcd.setCursor(14,0);
  lcd.print("      ");
  lcd.setCursor(15,0);
  lcd.print("Trong");
  lcd.setCursor(12,2);
  lcd.print(":");
  Serial.println(Count);
  lcd.setCursor(14,1);
  lcd.print("      ");
  lcd.setCursor(14,2);
  lcd.print("      ");
  lcd.setCursor(14,3);
  lcd.print("      ");
  while(Count == 1)
  {
    if(read_button()== 2)
    {
      delay(200);
      MaSo++;
      if(MaSo == 1000)
      {
        MaSo == 0;
      }
    lcd.setCursor(14,1);
    lcd.print("      ");
    lcd.setCursor(14,1);
    lcd.print(MaSo);
    }
    if(read_button()== 3)
    {
      delay(200);
      MaSo--;
      if(MaSo == 0)
      {
        MaSo = 1000;
      }
      lcd.setCursor(14,1);
      lcd.print("      ");
      lcd.setCursor(14,1);
      lcd.print(MaSo);
    }
    if(read_button()== 1) 
    {
      delay(300);
      Count++;
      break;
    }
  }
  
  lcd.setCursor(0,1);
  lcd.print(" ");
  lcd.setCursor(0,2);
  lcd.print("*");
  Serial.println(Count);
  lcd.setCursor(14,2);
  lcd.print("      ");
  lcd.setCursor(14,2);
  
  while(Count == 2)
  {
    if(read_button()== 2)
    {
      LieuLuong++;
      delay(300);
      if(LieuLuong > 25)
      {
        LieuLuong = 0;
      }
      lcd.setCursor(14,2);
      lcd.print("      ");
      lcd.setCursor(14,2);
      lcd.print(LieuLuong);
    }
    
    if(read_button()== 3)
    {
      delay(300);
      LieuLuong--;
      if(LieuLuong <= 0)
      {
        LieuLuong = 25;
      
      }
        lcd.setCursor(14,2);
        lcd.print("      ");
        lcd.setCursor(14,2);
        lcd.print(LieuLuong);
      
    }
     if(read_button()== 1) 
    {
      delay(300);
      Count++;
      break;
    }
  }

  lcd.setCursor(0,2);
  lcd.print(" ");
  lcd.setCursor(0,3);
  lcd.print("*");
  Serial.println(Count);
  lcd.setCursor(14,3);
  lcd.print("      ");
  lcd.setCursor(14,3);
  
  while(Count == 3)
  {
    if(read_button()== 2)
    {
      delay(300);
      TocDo++;
      if(TocDo > 10)
      {
        TocDo = 0;
      }
      lcd.setCursor(14,3);
      lcd.print("      ");
      lcd.setCursor(14,3);
      lcd.print(TocDo);
    }
    if(read_button()== 3)
    {
      delay(300);
      TocDo--;
      if(TocDo <= 0)
      {
        TocDo = 10;
      }
      lcd.setCursor(14,3);
      lcd.print("      ");
      lcd.setCursor(14,3);
      lcd.print(TocDo);
    }
     if(read_button()== 1) 
    {
      delay(300);
      Count++;
      break;
    }
  }
  lcd.setCursor(0,3);
  lcd.print(" ");
}
void Led_setup()
{
  pinMode(5,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(33,OUTPUT);
}