//setup for mp3 og RFID
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

//oppretter istans av MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);
  
//serial for aa kommunisere med mp3
SoftwareSerial mySoftwareSerial(5, 6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//div variabler
int button = 2;

boolean riktigKort = false;
int randomTall = 0;


void setup(){

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  pinMode(button, INPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

  //Serial til mp3  
  mySoftwareSerial.begin(9600);

  //Serial til konsoll for debug
  Serial.begin(115200);
  
  Serial.println(F("Initialiserer DFPlayer..."));

  //Errormelding
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  //setter volum til maks
  myDFPlayer.volume(30); 
}

//lister over spornummer

//ant dyr
int gris1 = 1;
int gris2 = 2;
int ku1 = 3;
int ku2 = 4;
int frosk1 = 5;
int frosk2 = 6;
int sau1 = 7;
int sau2 = 8;

//historier
int grisStory = 11;
int kuStory = 12;
int froskStory = 13;
int sauStory = 14;

//array med ant dyr
int griser[2] = {1, 2};
int kuer[2] = {3, 4};
int frosker[2] = {5, 6};
int sauer[2] = {7, 8};

//RFID UID
String sau1UID = ("0E A8 31 B5");
String sau2UID = ("0E A8 31 B5");//endres ved bruk av flere kort

String frosker1UID = ("BE A7 31 B5");
String frosker2UID = ("BE A7 31 B5");

String griser1UID = ("6E A7 31 B5");
String griser2UID = ("6E A7 31 B5");

String kuer1UID = ("1E A7 31 B5");
String kuer2UID = ("1E A7 31 B5");

//arrays med UID
String kortSauer[2] = {sau1UID, sau2UID};
String kortFrosker[2] = {frosker1UID, frosker2UID};
String kortGriser[2] = {griser1UID, griser2UID};
String kortKuer[2] = {kuer1UID, kuer2UID};


/*Sporliste:
  0001 gris1
  0002 gris2
  0003 ku1 
  0004 Ku2
  0005 frosk1
  0006 frosk2
  0007 sau1
  0008 sau2 
  0009 Riktig
  0010 Feil
  0011 story1
  0012 story2
  0013 story3
  0014 story4
 */

void loop() {

  //1. runde med griser
  randomTall = random(-1,2); 
  Serial.println(randomTall);
  kjor(kortGriser[randomTall], griser[randomTall], 11, 1);
  Serial.println();

  //2. runde med kuer
  randomTall = random(-1,2);
  Serial.println(randomTall);
  kjor(kortKuer[randomTall], kuer[randomTall], 12, 2);
  Serial.println();

  //3. runde med frosker
  randomTall = random(-1,2);
  Serial.println(randomTall);
  kjor(kortFrosker[randomTall], frosker[randomTall], 14, 3);
  Serial.println();

  //4. runde med sauer
  randomTall = random(-1,2);
  Serial.println(randomTall);
  kjor(kortSauer[randomTall], sauer[randomTall], 13, 4);
  Serial.println();
  
  Serial.println("Loop ferdig");
  
}

void kjor(String cardId, int trackNum, int storySpor, int storyNum){
  //venter på knappetrykk
  Serial.println("Venter på knappetrykk");
  
  while(true){
    delay(100);
    if(digitalRead(button) == HIGH){
      Serial.println("Knapp trykket");
      //gaar ut av uendelig loop
      break;
    }
  }
  
  Serial.print("Spiller av historiespor ");
  Serial.println(storyNum);
  myDFPlayer.playMp3Folder(storySpor);
  delay(8000);
  
  Serial.print("Spiller av dyrespor ");
  Serial.println(trackNum);
  myDFPlayer.playMp3Folder(trackNum);
  delay(4000);
  
  lesKortOgSpillAv(cardId, trackNum);
}

void lesKortOgSpillAv(String cardId, int trackNum){
  //looper til riktig kort er scannet
  while(true){
    // sjekker for nye kort
    Serial.println();
    Serial.println("Venter på kort");
    
    while(!mfrc522.PICC_IsNewCardPresent());
    
    while(!mfrc522.PICC_ReadCardSerial());
    
    //kjorer gjennom innholdet i kortet
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++){
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    
    //sjekker om content == cardId
    if (content.substring(1) == cardId){
      //riktig kort
      digitalWrite(ledGreen, HIGH);
      Serial.println("Riktig kort! Spill spor nr 9");
      myDFPlayer.playMp3Folder(10);
      delay(4000);
      //breaker ut av uendelig while
      break;
    } else {

      //feil kort
      digitalWrite(ledRed, HIGH);
      Serial.println("Feil kort! Spill spor nr 10");
      myDFPlayer.playMp3Folder(9);
      delay(3000);
      //gjentar ant dyr
      Serial.print("Repetisjon: Spill av dyrespor ");
      Serial.println(trackNum);
      myDFPlayer.playMp3Folder(trackNum);
      delay(3000);
    }
  }
}
