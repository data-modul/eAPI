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

extern struct gpiohandle_request *req;
extern unsigned int gpioLines;
extern int gpiofd;
extern int gpioEnabled;

/* PWM  */
typedef struct _PWM_CONFIG
{
	uint8_t   initialized;
    uint8_t   exported;
	uint8_t   enabled;
    uint8_t   mode;
	uint8_t   alignment;
	uint8_t   polarity;
	uint16_t  period;
	uint16_t  duty;
    uint32_t  granularity;
} PWM_HW_CONFIG;

extern PWM_HW_CONFIG  pwmChannel[2];
extern char *pathPwmChannel0;
extern char *pathPwmChannel1;
extern char *pathPwmConfigure;
extern char *pwmchip;
/* end PWM */


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
