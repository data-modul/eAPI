#ifndef _EAPIEMULEEPROM_H
#define _EAPIEMULEEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdint.h>
#include <EApiLib.h>

#define EEPROM_DEVICE           0x50

extern int userspaceBuffer_Cmd;

struct i2c_adap {
	int nr;
	char *name;
};

struct i2c_adap *more_adapters(struct i2c_adap *adapters, int n);
struct i2c_adap *gather_i2c_busses(void);
void free_adapters(struct i2c_adap *adapters);
EApiStatus_t fill_eepromBuffer(uint8_t block, uint8_t type);

int find_eeprom(void);
uint8_t *eeprom_analyze(uint8_t block, uint8_t type, uint8_t
        reqIndex);
uint8_t *eeprom_userSpace();

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULEEPROM_H */




