
## Install the library

Add the entry in hte platformio.ini file

```ini
[env]
lib_deps =
  https://github.com/Debuguear/AirQualityPMS.git

```

## Usage



### PMS5003T Sensor

```c++
#include "PMS5003T.h"

SoftwareSerial pmsSerial(2, 3);
debuguear::PMS5003T_PROCESSOR_T processor = debuguear::pms5003TProcessor(pmsSerial);

void observerFunction(debuguear::AirQualityModel_PMS5003T* data) {
    Serial.println("Observer called!");
    Serial.println(data->toString());
}

void setup() {
    // setup your handlers.
    processor.addObserver(observerFunction);
}

void loop() {
    // Run the on each loop.
    processor.loop();
}


```


### Run tests

```shell
pio test -e test
```