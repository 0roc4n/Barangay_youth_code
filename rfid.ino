/*
Barangay Inarado Youth Card Project

*/



#include <WiFi.h>
#include <SPI.h>
#include <HTTPClient.h>
#include <MFRC522.h>

#define SS_PIN 21   // SDA pin for SPI
#define RST_PIN 22  // RST pin for the MFRC522

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

#define ON_Board_LED 2  // On-board LED pin

const char* ssid = "thus";
const char* password = "123456789";

// ESP8266WebServer server(80);  // Server on port 80

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;

void setup() {
  Serial.begin(115200);
  SPI.begin();      
  mfrc522.PCD_Init(); 

  delay(500);

  WiFi.begin(ssid, password);
  Serial.println("");

  pinMode(ON_Board_LED, OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Please tag a card or keychain to see the UID !");
  Serial.println("");
}

void loop() {
  readsuccess = getid();
 
  if(readsuccess) {  
  digitalWrite(ON_Board_LED, LOW);
    HTTPClient http;    //Declare object of class HTTPClient
 
    String UIDresultSend, postData;
    UIDresultSend = StrUID;
   
    //Post Data
    postData = "rfid=" + UIDresultSend;
  
    http.begin("http://192.168.46.238:8000/api/send");  //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header
   
    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload
  
    Serial.println(UIDresultSend);
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    
    http.end();  //Close connection
    delay(1000);
  digitalWrite(ON_Board_LED, HIGH);
  }
}

int getid() {  
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
 
  Serial.print("THE UID OF THE SCANNED CARD IS : ");
  
  for (int i = 0; i < 4; i++) {
    readcard[i] = mfrc522.uid.uidByte[i];
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
  return 1;
}

void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}