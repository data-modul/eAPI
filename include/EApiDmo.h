//
// EApiDmo.h
// EApi Data Modul Extensions
//

#ifndef EAPIDMO_H
#define EAPIDMO_H


#define EAPI_PNPID_DM EAPI_CREATE_PNPID('D','M','O')
#define EAPI_DMO_ID_BOARD_MANUFACTURING_DATE_STR EAPI_CREATE_CUST_ID('D', 'M', 'O', 0)
#define EAPI_DMO_ID_BOARD_ID_STR EAPI_CREATE_CUST_ID('D', 'M', 'O', 1)

#define BBW6  1
#define CBS6  2
#define UNKNOWN 0

#define HWMON_PATH "/sys/class/hwmon/"
#define HWMON_NAME "nct7802"

extern uint8_t *eepromBuffer;
extern int borad_type;
extern char *hwname;


#endif // EAPIDMO_H
