

//Check to see if there is any new data available
//Read the contents of the incoming packet into the shtpData array
boolean receivePacketSPI(void)
{
  //HINT line should be high before we begin
  if (digitalRead(INT) == LOW)
  {
    Serial.println("Int should not be low - SPI failed");
    testINT = false; //Fail
    return(false);
  }

  digitalWrite(PS0_WAKE, LOW);

  for (int x = 0 ; x < 100 ; x++)
  {
    //HINT line should go low indicating sensor is ready for data
    if (digitalRead(INT) == LOW)
    {
      Serial.println("Int low 2 - Good! Int works");
      testINT = true;
      break;
    }
    delay(1);
  }
  digitalWrite(PS0_WAKE, HIGH);

  digitalWrite(SPI_CS, LOW); //Select sensor

  byte packetLSB;
  byte packetMSB;
  byte channelNumber;
  byte sequenceNumber;

  for (int x = 0 ; x < 1 ; x++)
  {
    //Get the first four bytes, aka the packet header
    packetLSB = SPI.transfer(0x00);
    packetMSB = SPI.transfer(0x00);
    channelNumber = SPI.transfer(0x00);
    sequenceNumber = SPI.transfer(0x00);

    uint16_t dataLength = ((uint16_t)packetMSB << 8 | packetLSB);
    //Serial.print("dataLength: ");
    //Serial.println(dataLength);
    Serial.print("packetLSB: 0x");
    Serial.println(packetLSB, HEX);
    Serial.print("packetMSB: 0x");
    Serial.println(packetMSB, HEX);
    Serial.print("channelNumber: 0x");
    Serial.println(channelNumber, HEX);
    Serial.print("sequenceNumber: 0x");
    Serial.println(sequenceNumber, HEX);
  }

  //HINT should release and go back to high
  if (digitalRead(INT) == LOW)
  {
    Serial.println("Int low 3 - Int should not be low here");
    testINT = false; //INT may be jumpered low
  }


  digitalWrite(SPI_CS, HIGH); //Deselect sensor

  uint16_t dataLength = ((uint16_t)packetMSB << 8 | packetLSB);
  if (dataLength != 0xFFFF)
    return (true); //We heard something

  return (false); //We failed to hear anything
}



