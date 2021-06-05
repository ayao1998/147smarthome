// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>
#include "application.h"
// This #include statement was automatically added by the Particle IDE.
#include <SparkFunRHT03.h>

// This #include statement was automatically added by the Particle IDE.
#include <Grove_LCD_RGB_Backlight.h>

#include "SparkFun_MMA8452Q.h"

// This #include statement was automatically added by the Particle IDE.
#include <Grove_LCD_RGB_Backlight.h>
#include <Wire.h>

unsigned int nextTime = 0; // Next time to contact the server
HttpClient http;
// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
// { "Content-Type", "application/json" },
// { "Accept" , "application/json" },
{ "Accept" , "*/*"},
{ NULL, NULL } // NOTE: Always terminate headers will NULL
};
http_request_t request;
http_response_t response;

const int lightSensor = A0;
int light_state = 0; // Off
int lightSensorThreshold = 250; // TODO: Fine tune

//const int soundSensor = A1;
//long soundVal = 0;

rgb_lcd lcd;
const int colorR = 161;
const int colorG = 2;
const int colorB = 250;

MMA8452Q accel;
int xAcceleration, yAcceleration, zAcceleration, rms, prev_rms;
int accelerationThreshold = 100;

//temperature and humidity sensor initialization
RHT03 tempSensor;
float humidity = 0;
float fahren = 0;

const float max_temp = 80.0;
const float min_temp = 70.0;
bool heater = false;
bool aircon = false;


int prev_mil = 0;


void setup()
{
    accel.init();
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    lcd.print("Demo");
    // prev_rms = 0;
    
    tempSensor.begin(D3);
    
    
    accel.read();
    xAcceleration = accel.x; // Read in raw x-axis acceleration data
    yAcceleration = accel.y; // Read in raw x-axis acceleration data
    zAcceleration = accel.z; // Read in raw x-axis acceleration data
    prev_rms = sqrt((xAcceleration * xAcceleration + yAcceleration * yAcceleration + zAcceleration * zAcceleration)/2);
    

    delay(1000);

}

void loop()
{
    
    if (nextTime > millis()) {
        return;
    }
    Serial.println(millis()-prev_mil);
    prev_mil = millis();
    nextTime = millis() + 5000;
    
    int sensorValue = analogRead(lightSensor);
    
    tempSensor.update();
    humidity = tempSensor.humidity();
    fahren = tempSensor.tempF();
    Serial.println("Temp: " + String(fahren));
    Serial.println("Humidity: " + String(humidity));
    
    if (fahren > max_temp && !aircon) //turn on AC if it's too hot
    {
        lcd.clear();
        lcd.print("AC on");
        Serial.println("AC on");
        aircon = true;
    }
    else if (fahren < min_temp && !heater) //turn on heater if it's too cold
    {
        lcd.clear();
        lcd.print("Heater on");
        Serial.println("Heater on");
        heater = true;
    }
    else if (fahren < max_temp && aircon) //turn off AC if it's on and cool enough
    {
        lcd.clear();
        lcd.print("AC off");
        Serial.println("AC off");
        aircon = false;
    }
    else if (fahren > min_temp && heater) //turn off heater if it's on and warm enough
    {
        lcd.clear();
        lcd.print("Heater off");
        Serial.println("Heater off");
        heater = false;
        
    }

    
    lcd.clear();
    lcd.print(String(int(fahren)) + "F");
    if(aircon) {
        lcd.print("   AC on");
    }
    else if(heater){
        lcd.print("   Heater on");
    }
    lcd.setCursor(0, 1);
    if (light_state) lcd.print("Lights on");
    else lcd.print("Lights off");
    
    
    accel.read();
    xAcceleration = accel.x; // Read in raw x-axis acceleration data
    // Serial.print("Acceleration on the x-axis is ");
    // Serial.println(xAcceleration);
  
    yAcceleration = accel.y; // Read in raw x-axis acceleration data
    // Serial.print("Acceleration on the y-axis is ");
    // Serial.println(yAcceleration);
  
    zAcceleration = accel.z; // Read in raw x-axis acceleration data
    // Serial.print("Acceleration on the z-axis is ");
    // Serial.println(zAcceleration);
    rms = sqrt((xAcceleration * xAcceleration + yAcceleration * yAcceleration + zAcceleration * zAcceleration)/2);
    Serial.print("RMS is ");
    Serial.println(rms);
    Serial.print("Prev_RMS is ");
    Serial.println(prev_rms);
    // print the number of seconds since reset:
    if (((sensorValue <= lightSensorThreshold) && (!light_state)) && (rms < (prev_rms - accelerationThreshold) || rms > (prev_rms + accelerationThreshold)))
    {
        light_state = 1;
        lcd.clear();
        //lcd.print("Demo");
        //lcd.setCursor(0, 1);
        lcd.print("Turn lights on");
        Serial.println("lights on");
        
    }
    else if (((sensorValue > lightSensorThreshold) && (!light_state)) && (rms < (prev_rms - accelerationThreshold) || rms > (prev_rms + accelerationThreshold)))
    {
        light_state = 0;
        lcd.clear();
        //lcd.print("Demo");
        //lcd.setCursor(0, 1);
        lcd.print("Already bright");
        Serial.println("already bright");
    }
   
    else if (((sensorValue > lightSensorThreshold) && (light_state)) && (rms < (prev_rms - accelerationThreshold) ||  rms > (prev_rms + accelerationThreshold)))
    {
        light_state = 0;
        lcd.clear();
        //lcd.print("Demo");
        //lcd.setCursor(0, 1);
        lcd.print("Turn lights off");
        Serial.println("lights off");
    }
    else // only change prev_rms if door hasn't opened. if door has opened, once we know the rms is different, we don't need it anymore.
    {
        prev_rms = rms;
    }
    Serial.println("Light value: " + String(sensorValue));
    
    //soundVal = analogRead(soundSensor);
    //Serial.println("SOUND: " + String(soundVal));
    
    Serial.println();

    request.hostname = "18.117.149.154";
    request.port = 5000;

    
    request.path = "/add?ac="+String(aircon)+"&heat="+String(heater)+"&light="+String(light_state);
    http.get(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);
    //Serial.print("Application>\tHTTP Response Body: ");
    //Serial.println(response.body);
    
}