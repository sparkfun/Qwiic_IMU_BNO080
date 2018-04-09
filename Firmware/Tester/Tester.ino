/*
  Jig testing the BNO080
  By: Nathan Seidle
  SparkFun Electronics
  Date: December 21st, 2017
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14586

  Hardware Connections:
  Attach the Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the sensor onto the shield
  Serial.print it out at 9600 baud to serial monitor.

  Available:

  We want rotation vector! 3.5degree dynamic max, 2 degree static max error in heading, in practice it's 5 degrees?
  400Hz is max available rate of rotation vector
  Approx 11mA at 100Hz

  Stability classifier, is it on table, stable(in hand), or in motion
  Tap detector: Single or double tap and the axis that tap was detected.
  Step detector: either on wrist or all other uses (in pocket),
  Step counter: same thing just more analyzation along the way, more accurate
  Activity classification: are you running, walking, or still?
  Shake detector

  Calibrate sensor, orientations, sit still, rotate 180

  Set feature command = page 27 is 17 bytes, 21 total bytes with header

*/

#include <Wire.h>

#include <SoftwareSerial.h>
SoftwareSerial mySerial(A4, A5); // RX, TX
//SoftwareSerial mySerial(12, 13); //?

#include <SPI.h>

#include "SparkFun_BNO080_Arduino_Library.h"
BNO080 myIMU;

//Test jig pinouts
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte PS0_WAKE = 5;
const byte PS1 = 4;
const byte INT = 3;
const byte RST = 2;

const byte SPI_CS = 10;
const byte ADR_MOSI = 11;
const byte SPI_MISO = 12;
const byte SPI_SCK = 13;

const byte TEST_BUTTON = A0;
const byte TEST_LED = A1;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

boolean testI2CA = false;
boolean testI2CB = false;
boolean testSPI = false;
boolean testUART = false;
boolean testINT = false;

void setup()
{
  pinMode(PS0_WAKE, OUTPUT);
  pinMode(PS1, OUTPUT);
  pinMode(INT, INPUT_PULLUP);
  pinMode(RST, OUTPUT);

  pinMode(SPI_CS, INPUT); //High impedance for now
  pinMode(ADR_MOSI, OUTPUT);
  pinMode(SPI_MISO, INPUT); //High impedance for now
  pinMode(SPI_SCK, INPUT); //High impedance for now

  pinMode(TEST_BUTTON, INPUT_PULLUP);
  pinMode(TEST_LED, OUTPUT);

  Serial.begin(9600);
  Serial.println();
  Serial.println("BNO080 Tester");
}

void loop()
{

  //Test I2C
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  Wire.begin();
  digitalWrite(ADR_MOSI, HIGH); //Set I2C address to 0x4B
  setMode(0b00); //Reset IC and set mode to I2C
  myIMU.begin(0x4B, Wire);
  if (myIMU.receivePacket() == true) //The BNO080 comes out of reset with 3 init packet announcements. Look for one.
  {
    Serial.println("I2CB good!");
    testI2CB = true;
  }

  digitalWrite(ADR_MOSI, LOW); //Set I2C address to 0x4A
  setMode(0b00); //Reset IC and set mode to I2C
  myIMU.begin(0x4A, Wire);
  if (myIMU.receivePacket() == true) //The BNO080 comes out of reset with 3 init packet announcements. Look for one.
  {
    Serial.println("I2CA good!");
    testI2CA = true;
  }

  if (digitalRead(INT) == LOW) //Sensor is trying to tell us data is available
  {
    testINT = true;
  }

  Wire.end();
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
  pinMode(A4, INPUT); //Release I2C line because they are also connected to SPI/UART lines
  pinMode(A5, INPUT);

  //Test UART
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  delay(1);

  mySerial.begin(115200);

  setMode(0b01); //Reset IC and set mode to UART-RVC

  //We can't correctly read the data (it's corrupt at 115200 with software serial)
  //but we can see a volume of it

  //Look for 10 characters in 250ms
  int dataCount = 0;
  int counter = 0;
  while (counter < 200)
  {
    if (mySerial.available())
    {
      byte incoming = mySerial.read();

      if (incoming != 0x00 && incoming != 0xFF) dataCount++;

      if (dataCount > 20) break;
    }
    counter++;
    delay(1);
  }

  if (dataCount < 20)
  {
    Serial.println("Serial FAIL");
  }
  else
  {
    Serial.println("Serial good!");
    testUART = true; //Good enough for me
  }

  mySerial.end();

  //Test SPI
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  pinMode(SPI_CS, OUTPUT);
  pinMode(ADR_MOSI, OUTPUT);
  pinMode(SPI_MISO, INPUT);
  pinMode(SPI_SCK, OUTPUT);

  SPI.begin();

  setMode(0b11); //Reset IC and set mode to SPI

  //The BNO080 comes out of reset with 3 init packet announcements. Look for one.
  if (receivePacketSPI() == true)
  {
    Serial.println("SPI good!");
    testSPI = true;
  }

  if (testI2CA == true && testI2CB == true)
  {
    if (testUART == true)
    {
      if (testSPI == true)
      {
        if (testINT == true)
        {
          Serial.println("Board is good!");
        }
      }
    }
  }

  if (testI2CA == false)
    Serial.println("I2CA fail");
  if (testI2CB == false)
    Serial.println("I2CB fail");
  if (testUART == false)
    Serial.println("UART fail");
  if (testSPI == false)
    Serial.println("SPI fail");
  if (testINT == false)
    Serial.println("INT fail");

  while (1);
}

//Pull WAKE pin low to indicate we are ready to talk to the sensor
//Wait for sensor to pull INT pin low indicating it's ready for our talking
void startTransfer()
{
  digitalWrite(PS0_WAKE, LOW); //Tell sensor we are ready to talk

  int counter = 0;
  while (digitalRead(INT) == HIGH) //Wait for sensor to confirm
  {
    delay(1);
    if (counter++ > 500)
    {
      Serial.println(F("Sensor never pulled INT low"));
      return;
    }
  }

  digitalWrite(PS0_WAKE, HIGH); //Release WAKE

  Serial.println(F("Sensor is ready to talk!"));
}

//Resets the IC and puts it into one of four modes: UART, UART-RVC, SPI, I2C
void setMode(byte mode)
{
  //Begin test with unit in reset
  digitalWrite(RST, LOW);

  delay(1);

  if (mode == 0) //I2C
  {
    digitalWrite(PS0_WAKE, LOW);
    digitalWrite(PS1, LOW);
  }
  else if (mode == 1) //UART-RVC
  {
    digitalWrite(PS0_WAKE, HIGH);
    digitalWrite(PS1, LOW);
  }
  else if (mode == 2) //UART
  {
    digitalWrite(PS0_WAKE, LOW);
    digitalWrite(PS1, HIGH);
  }
  else if (mode == 3) //SPI
  {
    digitalWrite(PS0_WAKE, HIGH);
    digitalWrite(PS1, HIGH);
  }

  //Then release reset
  digitalWrite(RST, HIGH);

  delay(1);
}

