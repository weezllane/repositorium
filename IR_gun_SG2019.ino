
#include "SevenSegmentTM1637.h" // Digitial Display
#include "SevenSegmentExtended.h"
#include "SevenSegmentFun.h"
#include <IRremote.h> // IR emitter
#include <SoftwareSerial.h> // Set up the Serial MP3 Player module
#define ARDUINO_RX 5// Connect to TX of Serial MP3 Player module
#define ARDUINO_TX 6// Connect to RX of Serial MP3 Player module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);
static int8_t Send_buf[8] = {0} ;

#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY_W_INDEX 0X03 // DATA IS REQUIRED (number of song)
#define CMD_PLAY_W_VOL 0X22 // Data is needed  0x7E 06 22 00 xx yy EF;(xx volume)(yy number of song)
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_PREVIOUS 0X02
#define CMD_NEXT 0X01

// Set up the display
const int CLK = 9; // Set the CLK pin connection to the display
const int DIO = 8; // Set the DIO pin connection to the display
SevenSegmentFun    display(CLK, DIO);

IRsend irsend;

//int resetBut = 10; // Reset the game for next player. I decided against implementing this
int SwitchPin = 12;
int bullet = 20;
int randTrackNo;
int PrevTrackNo;

void setup()
{
  mySerial.begin(9600);
  Serial.begin(9600);
  display.begin();            // Initializes the display
  display.setCursor(0, 2);    // Set position of digits
  display.setBacklight(100);  // Set the brightness to 100 %
  //pinMode(resetBut, INPUT); // Ended up not using my final design
  //digitalWrite(resetBut, HIGH); // Ended up not using my final design
  pinMode(SwitchPin, INPUT);
  digitalWrite(SwitchPin, HIGH);
  display.print(bullet);

  delay(500);
  sendCommand(CMD_SEL_DEV, DEV_TF);// Select the SD card for Audio
  delay(200);
  sendCommand(CMD_PLAY_W_VOL, 0X1E01);// Play first audio clip. Used to play intro music
}

void loop() {
  //Serial.println(digitalRead(SwitchPin));
  for (bullet; bullet > 0;) {
    if (digitalRead(SwitchPin) == 0) {
      randTrackNo = random(2, 6); // Randomizing the gun sound FX. I had a few different gun shot sounds on the SD card to chose from
      while (randTrackNo == PrevTrackNo) { // Checks for repeating numbers. Prevents playing the same sound twice in a row
        randTrackNo = random(2, 6);
      }
      PrevTrackNo = randTrackNo;
      sendCommand(0x03, randTrackNo); // Play gunshot sound FX
      irsend.sendNEC(0xFFE01F, 32); // Sony TV power code

      bullet--; // Decrease bullet count by 1
      display.clear();
      if (bullet >= 10) {
        display.setCursor(0, 2);
      } else {
        display.setCursor(0, 3);
      }
      display.print(bullet); // Update display
      delay(800); // Time between each instance the trigger can be pulled

    }
    if (bullet <= 0) {
      sendCommand(CMD_PLAY_W_VOL, 0X1E06);// Play 'Game Over' audio
      gameOver();
    }
  }
  // I coded the following section when I was going to use a reset button. I nixed this in the final implementation

  //for (bullet; bullet <= 0;) { // Once all ammo is used, user can either wait or press second button to start again. This begins the countdown
  //   for (int counter = 0; counter < 45;) {
  //   counter++;
  //Serial.println("Waiting");
  //Serial.println(digitalRead(resetBut));
  //if (digitalRead(resetBut) == 0) //Second button to reset ammo.
  //{
  //Serial.println("Ready");
  //bullet = 20;
  //display.clear();
  //display.setCursor(0, 2);
  //display.print(bullet);
  // }

  //    delay(200);
  //  }
  // bullet = 20; //If timer runs out, the ammo is reset.
  //   display.clear();
  //   display.setCursor(0, 2);
  //   display.print(bullet);
  // }
}

void gameOver() {

  for (int i = 0; i <= 8; i++) { // Blinking display while the 'Game Over' audio played
    display.clear();
    display.setCursor(0, 0);
    display.print("0000");
    delay(200);
    display.clear();
    delay(200);
  }
  display.clear();
  display.setCursor(0, 0);
  display.print("-gg-"); // gg = good game
  delay(10000);
  bullet = 20; //Reset bullet count
  display.clear();
  display.setCursor(0, 2);
  display.print(bullet); // Ready for a new game
}

void sendCommand(int8_t command, int16_t dat) // Used to communicate with Serial MP3 player module
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
