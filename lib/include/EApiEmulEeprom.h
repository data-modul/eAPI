#ifndef _EAPIEMULEEPROM_H
#define _EAPIEMULEEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdint.h>
#include <EApiLib.h>


#define I2C_DEV_EEPROM8    8  // EEPROM 8bit addressing, <= 24C16
#define I2C_DEV_EEPROM16  16  // EEPROM 16bit addressing, > 24C16
#define I2C_DEV_EEPROM24  24  // EEPROM 24bit addressing, ???
#define I2C_DEV_EEPROM32  32  // EEPROM 32bit addressing, ???

#define EEPROM_DEVICE           0xA0

extern int userspaceBuffer_Cmd;

struct i2c_adap {
	int nr;
	char *name;
};


struct i2c_adap *more_adapters(struct i2c_adap *adapters, int n);
struct i2c_adap *gather_i2c_busses(void);
void free_adapters(struct i2c_adap *adapters);
EApiStatus_t fill_eepromBuffer(uint8_t block, uint8_t type);
void find_eeprom(void);
uint8_t *eeprom_analyze(uint8_t block, uint8_t type, uint8_t
        reqIndex);
uint8_t *eeprom_userSpace();

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULEEPROM_H */




