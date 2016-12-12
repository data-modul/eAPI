EAPI library provides common API for different DMO CPU modules. This provides
no necessary software modifications when DMO CPU  module is changed. 

About EAPI library

EAPI library provides at the moment the following features:

* Board information
* Read/Write i2c
* Get/Set GPIO pins
* Watchdog
* Get/Set backlight
* Read/Write defined user space at Eeprom

The EAPI library will be installed by default in **/usr/local** path. It can·
also be changed by using the environment variable **PREFIX**.

 ~~~bash
$ export PREFIX=/usr/local
$ make
$ make install
~~~

In addition to the installed version of the library(libEApi.so.1.0) in **PREFIX**,
the library is also located localy in **make/bin/Linux/x86_64/rel**.


