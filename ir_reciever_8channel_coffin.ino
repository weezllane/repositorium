#include <IRremote.h>
int ledTarget = 3; // Target LED
int RECV_PIN = 2; // IR receiver pin
int cycleNum = 1; // Variable for coffin lid shake
int bounceCount = 0; // Variable for bouncing skull in coffin

IRrecv irrecv(RECV_PIN);
decode_results results; // decode_results class is defined in IRremote.h

#include <SoftwareSerial.h> // Audio
#define ARDUINO_RX 13// Should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 12// Connect to RX of the module
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
  pinMode(ledTarget, OUTPUT);
  digitalWrite(ledTarget, HIGH); // Turn on Target LED
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH); // Motor for coffin lid. This prop used an 8 Channel relay. Each channel was set to 'HIGH' to be turned off, 'LOW' for on
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
  delay(500);
  sendCommand(CMD_SEL_DEV, DEV_TF); //selects the SD card for Audio
}

void loop() {
  if (irrecv.decode(&results)) { // Listening for IR signal
    Serial.println(results.value, HEX); // Used for monitoring
    quickPlay(cycleNum);
  }
  delay (50); // Small delay to prevent reading errors

}

int quickPlay(int x) {
  digitalWrite(ledTarget, LOW); // Turn off LED
  if (x == 1) { // First Groan. This prop just had the lid shake for the first three shots. The fourth shot activated a 'disco'. Coffin lid opened, lights, music, etc...
    digitalWrite(4, LOW);
    sendCommand(0x03, 2);
    delay(5000);
    digitalWrite(4, HIGH);
  } else if (x == 2) { // Second Groan
    digitalWrite(4, LOW);
    sendCommand(0x03, 4);
    delay(4000);
    digitalWrite(4, HIGH);
  } else if (x == 3) { // Third Groan
    digitalWrite(4, LOW);
    sendCommand(0x03, 5);
    delay(5000);
    digitalWrite(4, HIGH);
  } else if (x == 4) { // That's it! Lid opens, music play, lights!
    delay(100);
    digitalWrite(5, LOW); // Pneumatic cylinder for coffin lid
    delay(100);
    digitalWrite(6, LOW); // Disco Lights. Battery powered
    delay(100);
    digitalWrite(7, LOW); // Strobes. Battery powered
    delay(100);
    digitalWrite(8, LOW); // Ended up not using
    delay(100);
    digitalWrite(9, LOW); // Ended up not using
    sendCommand(0x03, 1);
    delay(1500);
    while (bounceCount <= 20) { // This controlled another pneumatic cylinder that made a skull bounce up and down (almost in time with the music)
      digitalWrite(10, LOW);
      delay(220);
      digitalWrite(10, HIGH);
      delay(220);
      bounceCount++;
    }
    bounceCount = 0;
    digitalWrite(5, HIGH); // Turn off all relays
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    cycleNum = 0;
  }

  delay(3000); // Delay for prop goes active again
  irrecv.resume();
  cycleNum++; // After 3rd shot, coffin plays 'Disco'
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
