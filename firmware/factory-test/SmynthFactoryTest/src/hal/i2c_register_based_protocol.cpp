#include "i2c_register_based_protocol.h"
#include <Wire.h>
#include "util/Log.h"


void i2c_regp_write_register(char addr, char reg, char *value, unsigned char len)
{
  log(5, "i2c_regp_write_register %x:%x val %x... (len=%d)", addr, reg, value[0], len);

  Wire.beginTransmission(addr);
  Wire.write(reg);
  while(len--) {
    Wire.write((*value)++);
  }
  int result = Wire.endTransmission();
  
  log(result == 0 ? 5 : 1, "i2c_regp_write_register %x:%x endTransmission: %d", addr, reg, result);
}

void i2c_regp_write_register(char addr, char reg, char value) {
  i2c_regp_write_register(addr, reg, &value, 1);
}

int i2c_regp_read_register(char addr, char reg)
{
  char buff;
  if(i2c_regp_read_register(addr, reg, &buff, 1) != 1) {
    return -1;
  } else {
    return buff;
  }
  return buff;
}

int i2c_regp_read_register(char addr, char reg, char *out, unsigned char len)
{
  log(5, "i2c_regp_read_register %x:%x (len=%d)", addr, reg, len);

  Wire.beginTransmission(addr);
  Wire.write(reg);
  int endTransmissionResult = Wire.endTransmission();
  
  log(endTransmissionResult == 0 ? 5 : 1, 
    "i2c_regp_write_register %x:%x endTransmission: %d", 
    addr, reg, endTransmissionResult);

  if(endTransmissionResult) {
    return -endTransmissionResult;
  }

  if(!Wire.requestFrom(addr, len)) {
    log(1, "i2c_regp_write_register %x:%x failure in requestFrom", addr, reg);
    return -90;
  }

  int numRead = 0;
  while (Wire.available() && numRead<len)
  {
    *out = Wire.read();
    log(5, "i2c_regp_write_register %x:%x read %d", addr, reg, *out);

    out++;
    numRead++;
  }

  if(numRead < len) {
    log(numRead < len ? 1 : 5, 
      "i2c_regp_write_register %x:%x read %d out of %d", addr, reg, numRead, len);    
  }

  return numRead;  
}
