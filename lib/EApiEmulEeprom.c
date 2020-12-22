#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <EApiI2c-dev.h>
#include <fcntl.h>
#include <errno.h>
#include "EApiEmulEeprom.h"
#include <EeeP.h>


#define BUNCH 8
#define I2C_DMEC                "i2c-dmec"

static uint8_t *eepromBuffer = NULL;

uint8_t ExtCmd = I2C_DEV_EEPROM8;
uint32_t EepromSize = 0;
int firstDynamicBlock_Cmd = 0;
int userspaceBuffer_Cmd = 0;

extern int Initialized;

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

void find_eeprom(void)
{
    struct i2c_adap *adapters = NULL;
    int count;

    uint32_t Cmd = 0;
    uint8_t EeepCmnHdr[8] = {0};
    EeePCmn_t *pEeepCmnHdr;
    int res = 0;
    adapters = gather_i2c_busses();
    if (adapters == NULL)
    {
        fprintf(stderr, "Error: Out of memory!\n");
        return;
    }

    for (count = 0; adapters[count].name; count++) {

        /* we can't assume to have a 16bit CMD EEPROM and therefore it is    */
        /* not allowed to do a 16bit CMD without checking the EEPROM header. */
        /* so we have to read out the header with 8bit CMD.                  */
        /* in order to read out the header from a 16bit EEPROM with 8bit CMD,*/
        /* the EEPROM pointer already has to be set to offset 0x00 and block */
        /* transfers must be supported by I2C_FUNC_I2C (not I2CFUNC_SMBUS).  */
        /* i2c_access will fail if I2C_FUNC_I2C is not supported.            */
        /* If I2C_FUNC_I2C is not supported, there is no way to auto detect  */
        /* a 16bit CMD EEPROM by it's header as we can't block read and byte */
        /* read will always return the first byte.                           */

        if (EApiI2CReadTransfer(adapters[count].nr, EEPROM_DEVICE, Cmd, &EeepCmnHdr[0] + 1, sizeof(EeepCmnHdr) - 1, sizeof(EeePCmn_t)))
        {
            continue;            
        }

        /* check common header.                                              */
        /* NOTE: when using 16bit addressing on an 8bit EEPROM, the first    */
        /* byte willbe missing since the internal address pointer has been   */
        /* incremented by one (second address byte is interpreted as byte    */
        /* write)                                                            */
        pEeepCmnHdr = (EeePCmn_t *)(&EeepCmnHdr[0] + 1); // assume 16bit eEEPROM
        res = eeprom_check_header(&EeepCmnHdr[0] + 1);
        if (res) 
        {   /* not 16bit? */
            res = eeprom_check_header(EeepCmnHdr);
            pEeepCmnHdr = (EeePCmn_t *)&EeepCmnHdr[0];
        }
        
        if (res)
        {
            /* try to set eeprom pointer back to 0 */
            EApiI2CWriteReadEmul(adapters[count].nr, EEPROM_DEVICE, &Cmd, 2, 1, NULL, 0);
        }
        else {

            /* found! update part of eeprom information */
            if (pEeepCmnHdr->DeviceDesc & EEEP_DEVICE_EXT_INDEX) {
                ExtCmd = I2C_DEV_EEPROM16;
                Cmd = EAPI_I2C_ENC_EXT_CMD(Cmd);
            }

            /* try to set eeprom pointer back to 0 */
            EApiI2CWriteReadEmul(adapters[count].nr, EEPROM_DEVICE, &Cmd, ExtCmd/I2C_DEV_EEPROM8 + 1, ExtCmd/I2C_DEV_EEPROM8, NULL, 0);

            /* if it is dmec i2c-8-mux skip, as it might me switched to other mux */
            /* take unmuxed bus instead */            
            if (!strncmp(adapters[count].name, I2C_DMEC,8)) 
            {
                /* it is muxed bus. reset eeprom info */
                ExtCmd = I2C_DEV_EEPROM8;
                Cmd = 0;
                continue;
            }
            else
            {                
                /* it is not muxed bus. update rest of eeprom information */
                eeprom_bus = adapters[count].nr;
                EepromSize = 256 << (pEeepCmnHdr->DeviceDesc & EEEP_DEVICE_SIZE_MASK);
                firstDynamicBlock_Cmd = pEeepCmnHdr->BlkOffset * 2;
                break;
            }
        }

    }
    free_adapters(adapters);

    return;

}

int eeprom_check_header(uint8_t *buffer)
{
    EeePCmn_t *pEEEP;
    
    pEEEP = (EeePCmn_t*)buffer;
    
    return memcmp(pEEEP->EepId, EEEP_EEPROM_MARKER,  sizeof(EEEP_EEPROM_MARKER)- 1);
    
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
    uint32_t Cmd = 0;

    int iRead = 0;
    uint8_t validation[6];
    int found = 0;
    int length = 0;

    uint8_t EeepCmnHdr[8] = {0};
    EeePCmn_t *pEeepCmnHdr;
    
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    
    if (block != EEEP_BLOCK_ID_SMBIOS 
        && block != EEEP_BLOCK_ID_VENDOR_SPECIFIC) 
    {
        snprintf(err,sizeof(err),"Block 0x%02X not supported\n",block);
        EAPI_LIB_RETURN_ERROR(
                    fill_eepromBuffer,
                    EAPI_STATUS_READ_ERROR,
                    err);
    }

    if (eeprom_bus < 0)
    {
        find_eeprom();
    }

    if (eeprom_bus >= 0) //means eeprom is available
    {
               
        /* get first dynamic block */
        iRead = firstDynamicBlock_Cmd;
 
        while (found == 0 && iRead < EepromSize)
        {
            Cmd = iRead;
            if(ExtCmd==I2C_DEV_EEPROM16)
                Cmd = EAPI_I2C_ENC_EXT_CMD(Cmd);

            StatusCode = EApiI2CReadTransfer(eeprom_bus, EEPROM_DEVICE, Cmd, validation, sizeof(validation), sizeof(validation));
            if (StatusCode) 
            {
                snprintf(err,sizeof(err),"Can not read from Eeprom: 0x%08X\n",StatusCode);
                EAPI_LIB_RETURN_ERROR(
                            fill_eepromBuffer,
                            EAPI_STATUS_READ_ERROR,
                            err);
            }

            length = ((DBlockIdHdr_t*)&validation)->DBlockLength[0] << 8;
            length |= ((DBlockIdHdr_t*)&validation)->DBlockLength[1];
            length = length*2;

            switch(block) 
            {
                case EEEP_BLOCK_ID_VENDOR_SPECIFIC:
                    if (((DBlockIdHdr_t*)&validation)->DBlockId == EEEP_BLOCK_ID_VENDOR_SPECIFIC)
                    {
                        if ((*(uint16_t*)((EeePVendBlockHdr_t*)&validation)->VendId == 0xAF11) && (validation[5] == type))
                        {
                            if(ExtCmd==I2C_DEV_EEPROM16)
                                userspaceBuffer_Cmd = EAPI_I2C_ENC_EXT_CMD(iRead);
                            else
                                userspaceBuffer_Cmd = iRead;                                
                            found = 1;
                        }
                    }
                    break;
                case EEEP_BLOCK_ID_SMBIOS:
                    if (((DBlockIdHdr_t*)&validation)->DBlockId == EEEP_BLOCK_ID_SMBIOS)
                    {
                        if (((EeePSmbiosHdr_t*)&validation)->Type == type)
                        {
                            found = 1;
                        }
                    }
                    break;
                default:
                    break;
            }
            
            if (found) 
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

                StatusCode = EApiI2CReadTransfer(eeprom_bus, EEPROM_DEVICE, Cmd, eepromBuffer, length, length);
                if (StatusCode) 
                {
                    snprintf(err,sizeof(err),"Can not read from Eeprom: 0x%08X\n",StatusCode);
                    EAPI_LIB_RETURN_ERROR(
                                fill_eepromBuffer,
                                EAPI_STATUS_READ_ERROR,
                                err);
                }
            }
            else
            {
                iRead += length;
            }
        }
        
        /* try to set eeprom pointer back to 0 */
        Cmd = 0;
        if(ExtCmd==I2C_DEV_EEPROM16)
            Cmd = EAPI_I2C_ENC_EXT_CMD(Cmd);
        EApiI2CWriteReadEmul(eeprom_bus, EEPROM_DEVICE, &Cmd, ExtCmd/I2C_DEV_EEPROM8+1, ExtCmd/I2C_DEV_EEPROM8, NULL, 0);
            
        if (found)
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

    StatusCode = fill_eepromBuffer(EEEP_BLOCK_ID_VENDOR_SPECIFIC, 0x00);
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
