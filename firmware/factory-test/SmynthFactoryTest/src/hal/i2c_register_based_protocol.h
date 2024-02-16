#ifndef __I2C_REGISTER_BASED_PROTOCOL_H__
#define __I2C_REGISTER_BASED_PROTOCOL_H__

void i2c_regp_write_register(char addr, char reg, char value);
void i2c_regp_write_register(char addr, char reg, char *value, unsigned char len);

int i2c_regp_read_register(char addr, char reg);
int i2c_regp_read_register(char addr, char reg, char *out, unsigned char len);

#endif