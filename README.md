# ArduinoUAV General Info

This mini-code is a part of my engineering thesis project. Gps based location tracker with attitude meter.

## Installation

You can use any C++ code editor to edit this code. I recommend using the arduino IDE to upload the code to the Board: https://www.arduino.cc/en/software

## Usage
Before uploading, make sure you have the proper configuration on your board's pins.  
I recommend using the following modules:
- Arduino-nano
- BMP280
- Gps neo-6m
- SIM800L

Pin configuration:
```C++
//SoftwareSerial variables
static const int gpsRX = 11, gpsTX = 12;

static const uint32_t gpsBaud = 9600;

static const int simRX = 6, simTX = 7;

static const uint32_t simBaud = 9600;

```

Api address configuration: 
```C++
//Api web address
String ipAddress = "";

//Apn settings
String APN = "";
```

## Contributing
Pull requests are welcome.
