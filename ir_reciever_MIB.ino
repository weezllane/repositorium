#include <IRremote.h>

int relayPinLight = 2; // Relay for LED in MIB (Monster in box) prop
int relayPin = 3; // Relay to trigger pneumatic solenoid for MIB
int ledTarget = 5; // Target LED. Each prop has a lighted target
int RECV_PIN = 6; // IR receiver pin. Reads the IR emitter from gun
int randTrackNo; // Used for Audio
int PrevTrackNo; // Used for Audio

IRrecv irrecv(RECV_PIN);
decode_results results; // decode_results class is defined in IRremote.h

#include <SoftwareSerial.h> //Audio
#define ARDUINO_RX 11//should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 12//connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);
static int8_t Send_buf[8] = {0} ;

#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY_W_INDEX 0X03 //DATA IS REQUIRED (number of song)
#define CMD_PLAY_W_VOL 0X22 //data is needed  0x7E 06 22 00 xx yy EF;(xx volume)(yy number of song)
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_PREVIOUS 0X02
#define CMD_NEXT 0X01

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(relayPinLight, OUTPUT);
  digitalWrite(relayPinLight, LOW);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  pinMode(ledTarget, OUTPUT);
  digitalWrite(ledTarget, HIGH);
  delay(500);
  sendCommand(CMD_SEL_DEV, DEV_TF); // Selects the SD card for Audio
}

void loop() {
  if (irrecv.decode(&results)) { // Looking for IR signal
    Serial.println(results.value, HEX); // Used for monitoring
    quickPlay(relayPin); // If IR signal is detected, trigger relay and audio
  }
  delay (50); // Small delay to prevent reading errors

}

int quickPlay(int x) {
  digitalWrite(ledTarget, LOW); //Turn off Target LED
  randTrackNo = random(1, 4);
  while (randTrackNo == PrevTrackNo) { // Checks for repeating numbers. Prevents playing the same 'monster sound' twice in a row
    randTrackNo = random(1, 4);
  }
  PrevTrackNo = randTrackNo;
  sendCommand(0x03, randTrackNo); // Second number selects track
  digitalWrite(x, HIGH); // Close circuit. Triggers solenoid. Prop activates
  digitalWrite(relayPinLight, HIGH); // LED for MIB. Lights up prop
  delay(3000); // 1000 = 1 second
  digitalWrite(relayPinLight, LOW); // Turns off LED for MIB
  delay(2000);
  digitalWrite(x, LOW); // Reopen cicuit. Deactivates prop
  delay(8000); // Delay before prop is active again
  irrecv.resume(); // Resets IR receiver
  digitalWrite(ledTarget, HIGH); // Turn on Target LED
}

void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for (uint8_t i = 0; i < 8; i++) //
  {
    mySerial.write(Send_buf[i]) ;
  }
}
