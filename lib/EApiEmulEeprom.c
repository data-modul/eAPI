#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <EApiI2c-dev.h>
#include <fcntl.h>
#include <errno.h>
#include "EApiEmulEeprom.h"


#define BUNCH 8
#define I2C_DMEC                "i2c-dmec"
#define EEPROM_SIZE             (4*1024)
#define VENDOR_SPECIFIC_BLOCK   0xf0

static uint8_t *eepromBuffer = NULL;

int userspaceBuffer_Cmd = 0;

struct i2c_adap *more_adapters(struct i2c_adap *adapters, int n)
{
    struct i2c_adap *new_adapters;

    new_adapters = realloc(adapters, (n + BUNCH) * sizeof(struct i2c_adap));
    if (!new_adapters) {
        free_adapters(adapters);
        return NULL;
    }
    memset(new_adapters + n, 0, BUNCH * sizeof(struct i2c_adap));

    return new_adapters;
}

struct i2c_adap *gather_i2c_busses(void)
{
    char s[120];
    struct dirent *de, *dde;
    DIR *dir, *ddir;
    FILE *f;
    char fstype[NAME_MAX], sysfs[NAME_MAX], n[NAME_MAX];
    int foundsysfs = 0;
    int count=0;
    struct i2c_adap *adapters;

    adapters = calloc(BUNCH, sizeof(struct i2c_adap));
    if (!adapters)
        return NULL;
    /* look in sysfs */
    /* First figure out where sysfs was mounted */
    if ((f = fopen("/proc/mounts", "r")) == NULL) {
        goto done;
    }
    while (fgets(n, NAME_MAX, f)) {
        sscanf(n, "%*[^ ] %[^ ] %[^ ] %*s\n", sysfs, fstype);
        if (strcasecmp(fstype, "sysfs") == 0) {
            foundsysfs++;
            break;
        }
    }
    fclose(f);
    if (! foundsysfs) {
        goto done;
    }
    /* Bus numbers in i2c-adapter don't necessarily match those in
       i2c-dev and what we really care about are the i2c-dev numbers.
       Unfortunately the names are harder to get in i2c-dev */
    strcat(sysfs, "/class/i2c-dev");
    if(!(dir = opendir(sysfs)))
        goto done;
    /* go through the busses */
    while ((de = readdir(dir)) != NULL) {
        if (!strcmp(de->d_name, "."))
            continue;
        if (!strcmp(de->d_name, ".."))
            continue;

        /* this should work for kernels 2.6.5 or higher and */
        /* is preferred because is unambiguous */
        sprintf(n, "%s/%s/name", sysfs, de->d_name);
        f = fopen(n, "r");
        /* this seems to work for ISA */
        if(f == NULL) {
            sprintf(n, "%s/%s/device/name", sysfs, de->d_name);
            f = fopen(n, "r");
        }
        /* non-ISA is much harder */
        /* and this won't find the correct bus name if a driver
           has more than one bus */
        if(f == NULL) {
            sprintf(n, "%s/%s/device", sysfs, de->d_name);
            if(!(ddir = opendir(n)))
                continue;
            while ((dde = readdir(ddir)) != NULL) {
                if (!strcmp(dde->d_name, "."))
                    continue;
                if (!strcmp(dde->d_name, ".."))
                    continue;
                if ((!strncmp(dde->d_name, "i2c-", 4))) {
                    sprintf(n, "%s/%s/device/%s/name",
                            sysfs, de->d_name, dde->d_name);
                    if((f = fopen(n, "r")))
                        goto found;
                }
            }
        }

found:
        if (f != NULL) {
            int i2cbus;
            char *px;

            px = fgets(s, 120, f);
            fclose(f);
            if (!px) {
                fprintf(stderr, "%s: read error\n", n);
                continue;
            }
            if ((px = strchr(s, '\n')) != NULL)
                *px = 0;
            if (!sscanf(de->d_name, "i2c-%d", &i2cbus))
                continue;

            if ((count + 1) % BUNCH == 0) {
                /* We need more space */
                adapters = more_adapters(adapters, count + 1);
                if (!adapters)
                    return NULL;
            }


            adapters[count].nr = i2cbus;
            adapters[count].name = strdup(s);
            if (adapters[count].name == NULL) {
                free_adapters(adapters);
                return NULL;
            }
            count++;
        }
    }
    closedir(dir);

done:
    return adapters;
}

void free_adapters(struct i2c_adap *adapters)
{
    int i;

    for (i = 0; adapters[i].name; i++)
        free(adapters[i].name);
    free(adapters);
}

int find_eeprom(void)
{
    struct i2c_adap *adapters = NULL;
    int count;
    int result = -1;
    adapters = gather_i2c_busses();
    if (adapters == NULL)
    {
        fprintf(stderr, "Error: Out of memory!\n");
        return result;
    }
    for (count = 0; adapters[count].name; count++) {
        if (!strncmp(adapters[count].name, I2C_DMEC,8))
        {
            result = adapters[count].nr +1;
            break;
        }
    }
    free_adapters(adapters);
    return result;
}

uint8_t *eeprom_analyze(uint8_t block, uint8_t type, uint8_t reqIndex)
{
    int startDBIndex =0, endDBIndex;
    int lenDBIndex =0;
    int reqLoc;
    int index;
    int counter = 0;
    int i =0;
    uint8_t *temp;

    fill_eepromBuffer(block, type);

    if (eepromBuffer == NULL)
        return NULL;

    temp = calloc (NAME_MAX, sizeof(uint8_t));
    if (!temp)
    {
        if(eepromBuffer)
            free(eepromBuffer);
        return NULL;
    }

    startDBIndex = 0;
    lenDBIndex = eepromBuffer[startDBIndex + 1] << 8;
    lenDBIndex |= eepromBuffer[startDBIndex + 2];
    lenDBIndex = lenDBIndex*2;
    endDBIndex = lenDBIndex + startDBIndex;

    reqLoc = eepromBuffer[startDBIndex + reqIndex];

    index = eepromBuffer[ startDBIndex + 4] + 3 + startDBIndex;
    counter = 0;
    while (index < endDBIndex)
    {
        i=0;
        memset(temp,'\0',NAME_MAX);
        while ((eepromBuffer[index] != 0x00) && (index < endDBIndex))
        {
            temp[i] = eepromBuffer[index];
            i++;
            index++;
        }
        if (i > 0)
            counter++;

        temp[i] = '\0';

        if (counter == reqLoc)
        {
            if(eepromBuffer)
                free(eepromBuffer);
            return temp;
        }
        index++;
    }
    if(temp)
        free(temp);
    if(eepromBuffer)
        free(eepromBuffer);
    return NULL;
}

EApiStatus_t fill_eepromBuffer(uint8_t block, uint8_t type)
{
    uint32_t  Cmd ;
    char devname[20];
    int i2cDescriptor = 0;
    int res = 0;
    int iRead = 0;
    uint8_t validation[5];
    uint8_t tempEeprom[3]={0,0,0};
    int iValidationRead = 0;
    int found = 0;
    int first_byte = 0;
    int second_byte = 0;
    int third_byte = 0;
    int forth_byte = 0;
    int iInsert = 0;
    int length = 0;

    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    /* ******************** EEPROM ************************** */
    if (eeprom_bus > 0) //means eeprom is available
    {
        /* open device */
        snprintf(devname,sizeof(devname),"/dev/i2c/%d",eeprom_bus);
        devname[sizeof(devname) - 1] = '\0';
        i2cDescriptor = open(devname,O_RDWR);
        if(i2cDescriptor < 0)
        {
            sprintf(devname,"/dev/i2c-%d",eeprom_bus);
            i2cDescriptor = open(devname,O_RDWR);
        }
        if(i2cDescriptor < 0)
        {
            snprintf(err,sizeof(err),"Unrecognised Eeprom Address: %s\n ",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_NOT_FOUND,
                        err);
        }
        /* set slave address : set working device */
        if(ioctl(i2cDescriptor, I2C_SLAVE, EEPROM_DEVICE) < 0)
        {
            snprintf(err,sizeof(err),"Cannot set Eeprom slave ddress: %s\n",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_NOT_FOUND,
                        err);
        }

        Cmd = EAPI_I2C_ENC_EXT_CMD(0x00);
        res = i2c_smbus_write_byte_data(i2cDescriptor,(Cmd >> 8) & 0x0ff, Cmd & 0x0ff); /* write 16bits add */
        if (res < 0)
        {
            snprintf(err,sizeof(err),"Cannot write into Eeprom: %s\n",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_WRITE_ERROR,
                        err);
        }

        /* check correctness of Eeprom*/
        while(iValidationRead < 5)
        {
            res = i2c_smbus_read_byte(i2cDescriptor);
            if (res < 0)
            {
                snprintf(err,sizeof(err),"Cannot read from Eeprom: %s\n",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            fill_eepromBuffer,
                            EAPI_STATUS_READ_ERROR,
                            err);
            }
            validation[iValidationRead] = res;
            iValidationRead++;
        }
        if (validation [1]!= '3' && validation[2] != 'P')
        {
            snprintf(err,sizeof(err),"Not Valid Eeprom\n");
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_READ_ERROR,
                        err);
        }
        iRead = validation[4] * 2;
        Cmd = EAPI_I2C_ENC_EXT_CMD(validation[4] * 2);
        res = i2c_smbus_write_byte_data(i2cDescriptor,(Cmd >> 8) & 0x0ff, Cmd & 0x0ff); /* write 16bits add */
        if (res < 0)
        {
            snprintf(err,sizeof(err),"Cannot write into Eeprom: %s\n",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_WRITE_ERROR,
                        err);
        }

        while (found == 0 && iRead < EEPROM_SIZE)
        {
            /* read 4 first bytes*/
            res = i2c_smbus_read_byte(i2cDescriptor);
            if (res < 0)
            {
                snprintf(err,sizeof(err),"Cannot read from Eeprom: %s\n",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            fill_eepromBuffer,
                            EAPI_STATUS_READ_ERROR,
                            err);
            }
            else
            {
                if (first_byte == 0)
                {
                    if (res == block)
                    {
                        tempEeprom[0] = (uint8_t) res;
                        first_byte =1;
                        if(block == VENDOR_SPECIFIC_BLOCK)
                            userspaceBuffer_Cmd = iRead;
                    }
                }
                else if (second_byte == 0 && first_byte == 1)
                {
                    second_byte = 1;
                    tempEeprom[1] = (uint8_t) res;
                }
                else if (third_byte == 0 && second_byte == 1 )
                {
                    third_byte =1;
                    tempEeprom[2] = (uint8_t) res;
                    length = tempEeprom[1] << 8;
                    length |= tempEeprom[2];
                    length = length*2;
                }
                else if (forth_byte == 0 && third_byte ==1)
                {
                    if (res == type)
                    {
                        eepromBuffer=(uint8_t *)malloc((length) * sizeof(uint8_t));
                        if (!eepromBuffer)
                        {
                            snprintf(err,sizeof(err),"Error in Eeprom Allocating Memory\n");
                            EAPI_LIB_RETURN_ERROR(
                                        fill_eepromBuffer,
                                        EAPI_STATUS_ALLOC_ERROR,
                                        err);
                        }
                       // if(block == VENDOR_SPECIFIC_BLOCK)
                         //   size_userspacebuf = length;
                        eepromBuffer[0] = tempEeprom[0];
                        eepromBuffer[1] = tempEeprom[1];
                        eepromBuffer[2] = tempEeprom[2];
                        iInsert =3;
                        eepromBuffer[iInsert] = (uint8_t) res;
                        forth_byte =1;
                        iInsert++;
                    }
                    else /* change to next block*/
                    {
                        first_byte = 0;
                        second_byte = 0;
                        third_byte = 0;
                        forth_byte = 0;
                        userspaceBuffer_Cmd = -1;

                        Cmd = EAPI_I2C_ENC_EXT_CMD(length + iRead - 3);
                        iRead = length + iRead - 4;

                        res = i2c_smbus_write_byte_data(i2cDescriptor,(Cmd >> 8) & 0x0ff, Cmd & 0x0ff); /* write 16bits add */
                        if (res < 0)
                        {
                            snprintf(err,sizeof(err),"Cannot write into Eeprom: %s\n",strerror(errno));
                            EAPI_LIB_RETURN_ERROR(
                                        fill_eepromBuffer,
                                        EAPI_STATUS_WRITE_ERROR,
                                        err);
                        }
                    }
                }
                else if (forth_byte == 1)
                {
                    if (iInsert < length)
                    {
                        eepromBuffer[iInsert] = (uint8_t) res;
                        iInsert++;
                    }
                    else
                        found =1;
                }
            }
            iRead++;
        }
        close(i2cDescriptor);
        EAPI_LIB_RETURN_SUCCESS(fill_eepromBuffer, "");
    }
    else
    {
        EAPI_LIB_RETURN_ERROR(
                    fill_eepromBuffer,
                    EAPI_STATUS_ERROR,
                    "No Eeprom Bus is found");
    }
    EAPI_LIB_ASSERT_EXIT
            return StatusCode;

}

uint8_t *eeprom_userSpace()
{
    uint8_t *temp_userspace;
    unsigned int size_userspacebuf = 0;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    StatusCode = fill_eepromBuffer(VENDOR_SPECIFIC_BLOCK, 0x11);
    if (StatusCode != EAPI_STATUS_SUCCESS || eepromBuffer == NULL)
        return NULL;

    size_userspacebuf = eepromBuffer[1] << 8;
    size_userspacebuf |= eepromBuffer[2];
    size_userspacebuf = size_userspacebuf*2;

    if (size_userspacebuf == 0 )
    {
        if(eepromBuffer)
            free(eepromBuffer);
        return NULL;
    }

    temp_userspace = calloc (size_userspacebuf, sizeof(uint8_t));
    if (!temp_userspace)
    {
        if(eepromBuffer)
            free(eepromBuffer);
        return NULL;
    }
    memcpy(temp_userspace, eepromBuffer, size_userspacebuf * sizeof(uint8_t));

    if(eepromBuffer)
        free(eepromBuffer);
    return temp_userspace;
}
