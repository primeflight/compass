// Compass Serial - HMC5883L (GY-273) -> JSON na serial
// Envia {"x":..,"y":..,"z":..,"h":..,"m":..} a cada 200ms
// A pagina web local (compass.html) le via Web Serial API.

#include <Wire.h>

#define HMC_ADDR 0x1E
#define REG_CONF_A 0x00
#define REG_CONF_B 0x01
#define REG_MODE 0x02
#define REG_DATA_X_H 0x03

void i2cWrite(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(HMC_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

bool leituraXYZ(int16_t &x, int16_t &y, int16_t &z)
{
  Wire.beginTransmission(HMC_ADDR);
  Wire.write(REG_DATA_X_H);
  if (Wire.endTransmission(false) != 0)
    return false;
  if (Wire.requestFrom(HMC_ADDR, (uint8_t)6) != 6)
    return false;
  x = (Wire.read() << 8) | Wire.read();
  z = (Wire.read() << 8) | Wire.read();
  y = (Wire.read() << 8) | Wire.read();
  return true;
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ;
  }

  Wire.begin();
  Wire.setClock(100000);
  Wire.beginTransmission(HMC_ADDR);
  if (Wire.endTransmission() != 0)
  {
    Serial.println(F("{\"erro\":\"HMC5883L not found!\"}"));
    while (true)
    {
      delay(1000);
    }
  }
  i2cWrite(REG_CONF_A, 0x70); // media 8, 15 Hz
  i2cWrite(REG_CONF_B, 0x20); // ganho +-1.3 Ga
  i2cWrite(REG_MODE, 0x00);   // modo continuo
}

void loop()
{
  int16_t x, y, z;
  if (leituraXYZ(x, y, z))
  {
    float heading = atan2((float)y, (float)x) * 180.0 / PI;
    if (heading < 0)
      heading += 360.0;
    float mag = sqrt((float)x * x + (float)y * y + (float)z * z) * 0.92;

    Serial.print(F("{\"x\":"));
    Serial.print(x);
    Serial.print(F(",\"y\":"));
    Serial.print(y);
    Serial.print(F(",\"z\":"));
    Serial.print(z);
    Serial.print(F(",\"h\":"));
    Serial.print(heading, 1);
    Serial.print(F(",\"m\":"));
    Serial.print(mag, 0);
    Serial.println(F("}"));
  }
  delay(200);
}
