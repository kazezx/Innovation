#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         D3 
#define SS_PIN          D8 

MFRC522 mfrc522(SS_PIN, RST_PIN);

//WIFI SETUP
const char* ssid = "Zuhayr" ;
const char* password = "Zuhayr007" ;
WiFiUDP DroidPort;
unsigned int mcuport = 2807 ;
char packetBuffer[255];
boolean checker = false ;
String zx = " ";
String goal = "";

void setup( ) {
 Serial.begin(115200);                                         
 SPI.begin();                                                 
 mfrc522.PCD_Init();
 delay(2500) ;
 Serial.print("SSID : ");
 Serial.println(ssid) ;
 WiFi.begin(ssid, password) ;
 Serial.println("Connected");
 while (WiFi.status( ) != WL_CONNECTED) {
 delay(50) ;
 Serial.print(".") ;
 }
 Serial.print("IP : ");
 Serial.println(WiFi.localIP());
 DroidPort.begin(mcuport);
 Serial.print("Port : ");
 Serial.println(mcuport);
 delay(1500) ;
}
void loop() {
  delay(100) ;
  Serial.println("Waiting for Card..");
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  byte block;
  byte len;
  MFRC522::StatusCode status;
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  byte buffer2[18];
  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer2[i] );
  }
  delay(100);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  String eid_user = String((char*)buffer2);
  Serial.println("Sending Packet");
  packet_sender(eid_user);
  delay(100);
  String pass = entry_receiver();
  Serial.println(pass);
 }

void packet_sender(String eid_user)  {
 char eid_packet[(eid_user.length()+1)];
 if (eid_user != "") 
 {
   eid_user.toCharArray(eid_packet, (eid_user.length() + 1));
   DroidPort.beginPacket("192.168.100.16", 55244) ;
   DroidPort.write(eid_packet);
   DroidPort.endPacket();
   delay(100);
 }
}

String entry_receiver() {
  Serial.println("Receiving Packet");
 String myData = "";
 int packetSize = DroidPort.parsePacket();
 if (packetSize) {
 int len = DroidPort.read(packetBuffer, 64);
 for (int i = 0; i < packetSize; i++) {
 myData += (char)packetBuffer[i];
 }
 }
 return myData;
}
