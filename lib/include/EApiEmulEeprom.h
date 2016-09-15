#ifndef _EAPIEMULEEPROM_H
#define _EAPIEMULEEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdint.h>

#define I2C_DMEC "i2c-dmec"
#define EEPROM_DEVICE  0x50
#define EEPROM_SIZE  512

struct i2c_adap {
	int nr;
	char *name;
};

struct i2c_adap *more_adapters(struct i2c_adap *adapters, int n);
struct i2c_adap *gather_i2c_busses(void);
void free_adapters(struct i2c_adap *adapters);
int find_eeprom(void);
uint8_t *eeprom_analyze(uint8_t *eeprom, uint8_t type, uint8_t
		reqIndex);

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULEEPROM_H */




