/*
* EApiDmo.h
* EApi Data Modul Extensions
*/

#ifndef EAPIDMO_H
#define EAPIDMO_H


#define EAPI_PNPID_DM EAPI_CREATE_PNPID('D','M','O')
#define EAPI_DMO_ID_BOARD_MANUFACTURING_DATE_STR EAPI_CREATE_CUST_ID('D', 'M', 'O', 0)
#define EAPI_DMO_ID_BOARD_ID_STR EAPI_CREATE_CUST_ID('D', 'M', 'O', 1)

#define ACPIHWMON_PATH "/sys/bus/platform/drivers/dmec-acpi/"
#define RTM_PATH "/sys/bus/platform/drivers/dmec-rtm/"


extern int eeprom_bus;
extern uint8_t *eeprom_userSpaceBuf;

extern char *acpiHwmonName;
extern char *rtmname;

extern char err[256];

extern unsigned int gpioLines;
extern unsigned int gpioBase;
extern int *gpiofdExpert ;
extern int *gpiofdValue ;
extern int *gpiofdDirection ;
extern int gpioEnabled;



/*
 *
 *      B A C K L I G H T
 *
 */
/* IDS */
#define EAPI_DMO_ID_BACKLIGHT_1              EAPI_ID_BACKLIGHT_1

/*
 *
 *      G P I O
 *
 */
/* IDs */
#define EAPI_DMO_ID_GPIO_BANK    EAPI_ID_GPIO_BANK00
#define EAPI_DMO_ID_GPIO_GPI0    EAPI_GPIO_GPIO_ID(0)
#define EAPI_DMO_ID_GPIO_GPI1    EAPI_GPIO_GPIO_ID(1)
#define EAPI_DMO_ID_GPIO_GPI2    EAPI_GPIO_GPIO_ID(2)
#define EAPI_DMO_ID_GPIO_GPI3    EAPI_GPIO_GPIO_ID(3)
#define EAPI_DMO_ID_GPIO_GPO0    EAPI_GPIO_GPIO_ID(4)
#define EAPI_DMO_ID_GPIO_GPO1    EAPI_GPIO_GPIO_ID(5)
#define EAPI_DMO_ID_GPIO_GPO2    EAPI_GPIO_GPIO_ID(6)
#define EAPI_DMO_ID_GPIO_GPO3    EAPI_GPIO_GPIO_ID(7)

#endif /* EAPIDMO_H */
