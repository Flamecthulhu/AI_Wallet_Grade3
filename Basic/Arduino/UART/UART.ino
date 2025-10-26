const int txbuf[8] = {1,2,3,4,5,6,7,8};

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 9, 10);

  Serial.println("Test");
}

void loop()
{
  if (Serial2.available())
  {
    int buffer[8];
    for (int i = 0; i < 8; i++)
    {
      buffer[i] = Serial2.read();
      Serial.print(buffer[i]);
    }
    Serial.println();
  }
  for (int i = 0; i < 8; i++)
  {
    Serial2.print("HAL_OK");
  }
  
}