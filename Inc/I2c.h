#ifndef I2C_H_
#define I2C_H_
void I2c_int(void);
void I2c_Start(void);
void I2c_Write(uint8_t data);
void I2c_Address(uint8_t Address);
void I2c_Stop(void);
void I2c_WriteMultiByte(uint8_t *data, uint8_t n);
void I2c_Read(uint8_t Address, uint8_t *buffer, uint8_t size);
#endif /* I2C_H_ */
