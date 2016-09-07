#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "EApiEmulEeprom.h"

#define BUNCH 8

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
            result = adapters[count].nr;
            free_adapters(adapters);
            break;
        }
    }
    return result;
}

uint8_t *eeprom_analyze(uint8_t *eeprom,uint8_t type, uint8_t reqIndex)
{
    int startDBIndex =0, endDBIndex;
    int lenDBIndex =0;
    int reqLoc;
    int index;
    int counter = 0;
    int i =0;
    uint8_t *temp;

    if (eeprom == NULL)
        return NULL;

    if (eeprom [1]!= '3' && eeprom[2] != 'P')
        return NULL;

    temp = calloc (100, sizeof(uint8_t));
    if (temp == NULL)
        return NULL;

    startDBIndex = eeprom[4] * 2;
    lenDBIndex = eeprom[startDBIndex + 1] << 8;
    lenDBIndex |= eeprom[startDBIndex + 2];
    lenDBIndex = lenDBIndex*2;
    endDBIndex = lenDBIndex + startDBIndex;

    do {
        if(eeprom[startDBIndex + 3] == type)
            reqLoc = eeprom[startDBIndex + reqIndex];
        else
        {
            startDBIndex = endDBIndex;
            lenDBIndex = eeprom[startDBIndex + 1] << 8;
            lenDBIndex |= eeprom[startDBIndex + 2];
            lenDBIndex = lenDBIndex*2;
            endDBIndex = lenDBIndex + startDBIndex;
            continue;
        }

        index = eeprom[ startDBIndex + 4] + 3 + startDBIndex;
        counter = 0;
        while (index < endDBIndex)
        {
            i=0;
            memset(temp,'\0',100);
            while ((eeprom[index] != 0x00) && (index < endDBIndex))
            {
                temp[i] = eeprom[index];
                i++;
                index++;
            }
            if (i > 0)
                counter++;

            temp[i] = '\0';

            if (counter == reqLoc)
            {
                return temp;
            }
            index++;
        }

        //move to next DBlock
        startDBIndex = endDBIndex;
        lenDBIndex = eeprom[startDBIndex + 1] << 8;
        lenDBIndex |= eeprom[startDBIndex + 2];
        lenDBIndex = lenDBIndex*2;
        endDBIndex = lenDBIndex + startDBIndex;
    } while (endDBIndex < EEPROM_SIZE);

    printf("NOT found!!!\n");
    memset(temp,'\0',100);
    return temp;
}
