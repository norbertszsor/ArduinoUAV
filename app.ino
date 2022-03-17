
#include <Wire.h>
#include <TinyGPS.h>
#include "i2c.h"
#include "i2c_BMP280.h"
#include <ArduinoJson.h>
#include <NeoSWSerial.h>

//Device Serial number
int deviceSerial = 0;

//SoftwareSerial variables
static const int gpsRX = 11, gpsTX = 12;
static const uint32_t gpsBaud = 9600;
static const int simRX = 6, simTX = 7;
static const uint32_t simBaud = 9600;

//SoftwareSerial instances
NeoSWSerial gpsSerial(gpsRX,gpsTX);
NeoSWSerial simSerial(simRX,simTX);

//Other modules instances
BMP280 bmp280;
TinyGPS gps;

//Api web address
String ipAddress = "";
//Apn settings
String APN = "";

//Describes json format buffer
StaticJsonDocument<300> data;
String jsonContainer = "";

//Local vearibles
bool newData = false, newDataBMP = false;
unsigned long chars,age;
unsigned short sentences, failed;
float flat, flon, altx, temp, pascal;
static float meters, metersold, tmpAltitude;
long altitude;     

void setup(){

    //serial communication initliarizer
    Serial.begin(9600);
    gpsSerial.begin(gpsBaud);
    simSerial.begin(simBaud);

    //gsm moudle manual initializer
    simSerial.write("init");
    Serial.println("Sim800L initialization...");

    //static waiting for sim connection
    delay(20000);
    Serial.println("Done");

    // switch to other module, arduino can't communicate with both in same time
    simSerial.listen(); 
    initializeGPRS();
    Serial.println("The GPRS connection enabled");

    //bmp280 initializer
    bmp280.setEnabled(0);
    gpsSerial.listen(); 
}

void loop(){

    delay(700);

    while(gpsSerial.available())
    {
        char c = gpsSerial.read();
        gps.encode(c);
        newData=true;
    }

    if(bmp280.initialize() == true)
    newDataBMP = true;

    if(newData){    
        altitude = gps.altitude();
        altx = altitude;
        gps.f_get_position(&flat, &flon, &age);    
        data["latitude"] = flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
        data["longitude"] = flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;
    
        if(gps.altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE){
            data["altitudeGPS"] = 0;
        }
        else{
            data["altitudeGPS"] =  altx/100,3;
        }

        if(newDataBMP){
            bmp280.triggerMeasurement();     
            tmpAltitude = meters;
            bmp280.awaitMeasurement();
            bmp280.getTemperature(temp);
            bmp280.getPressure(pascal);
            bmp280.getAltitude(meters);

            data["altitudeBMP"] = meters;
            data["airTemperature"] = temp;
            //data["pressure"] = pascal;
            data["verticalSpeed"]= (meters - tmpAltitude)*3.6;
            data["speed"] = gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2;
        }
        serializeJsonPretty(data, Serial);
    }
        serializeJsonPretty(data, Serial);
        serializeJson(data, jsonContainer);
        sendData(jsonContainer);
}

// function that initialize GPRS connection
void initializeGPRS(){
    simSerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\""); //Connection type: GPRS
    delay(50);
    simSerial.println("AT+SAPBR=3,1,\"APN\",\"" + APN + "\""); //We need to set the APN which our internet provider gives us
    delay(50);
    simSerial.println("AT+SAPBR=1,1"); //Enable the GPRS
    delay(50);
    simSerial.println("AT+HTTPINIT"); //Enabling HTTP mode
    delay(50);
}

// function that send request to api
void sendData(String data){
    simSerial.listen();
    //The line below sets the URL we want to connect to
    simSerial.println("AT+HTTPPARA=\"URL\", \"https://" + ipAddress +  "api/GeoLocation?serial="+deviceSerial);
    delay(50);
    //GO
    simSerial.println("AT+HTTPACTION=0");
    delay(250);
    Serial.println("data sent");
    gpsSerial.listen();
}