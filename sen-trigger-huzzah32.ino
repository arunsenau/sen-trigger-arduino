/*
	Author: Arun Sen
	Description: Arduino firmware code for the SEN Trigger Smart Wireless Camera Trigger

  Notes: This code is designed to run on the Adafruit ESP32 Huzzah Board


    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "Elapsed.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define BLE_DEVICE_ID               "SEN Trigger ST0001"

#define SVC_UUID_SIMPLESHOOT        "18753475-6c26-4974-b4c9-98ccc7074602"
#define CHAR_UUID_SIMPLESHOOT_CMD   "1817b56c-bfae-43fa-801d-a1e90bf35381"

#define SVC_UUID_GLOBALCANCEL       "8fe04fa7-a889-4ea4-918c-9bce10ac2920"
#define CHAR_UUID_CANCEL_CMD        "4928aa54-0d8d-4a7f-b600-f61f8f199d42"

#define SVC_UUID_TOGGLESHOOT        "b9b3baea-6140-4aa7-adce-887de140d5b4"
#define CHAR_UUID_TOGGLESHOOT_CMD   "98906cc8-d907-4d0d-8965-cfaf1767f7b0"

#define SVC_UUID_PULSESHOOT         "49a2b2ae-4806-4f4d-be0c-185ae901790d"
#define CHAR_UUID_PULSE_INITDELAY   "9e1468ee-db91-47b2-bc76-0a01b2987a3c"
#define CHAR_UUID_PULSE_SHOTNUM     "0596a76d-f6ca-43d6-bf3f-702be97dfde1"
#define CHAR_UUID_PULSE_SHUTTERTIME "cfa2cc5a-ae20-4c3f-8867-1778bf120a86"
#define CHAR_UUID_PULSE_GAPTIME     "e0f93d24-ab6b-4bdb-94de-43227b6ea92d"
#define CHAR_UUID_PULSE_EXECPULSE   "68618322-8209-4a63-bd73-81a0eceff9e1"



const int   shutterPin = 12; // Analog output pin that the shutter release is attached to
int   triggerPulseLength = 150; //set default Trigger Pulse Length to 150ms
bool  globalCancel = false;
bool shutterOn = false; // Initially set shutter to Off

long simpleDelayTime = 0; // Simple timer time (ms.)

long pulseInitDelay = 0; // Pulse mode: Initialise Delay time (ms.)
int pulseShotNum = 0; // Pulse mode: Number of Shots
long pulseShutterTime = 0; // Pulse mode: Shutter time (ms.)
long pulseGapTime = 0; // Pulse mode: Gap time between shots (ms.)
bool pulseExecPulse = false; // Flag to execute Pulse Mode Shooting

static Elapsed simpleDelay_Timer, pulseShutter_Timer, pulseGap_Timer;

boolean isNumeric(std::string str) {
    unsigned int stringLength = str.length(); 
    if (stringLength == 0) {
        return false;
    }    
    boolean seenDecimal = false;     
    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str[i])) {
            continue;
        }
 
        if (str[i] == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}

class SimpleShootService: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("New value: " + (String) value.c_str() );        
        
        // Reset any cancels
        globalCancel = false;
                
        if (isNumeric(value)) {                    
          simpleDelayTime = atol(value.c_str());          
        } else if (value == "pulsenow") {
          digitalWrite (shutterPin, HIGH);  // turn on the shutter
          Serial.println("Pulse for " + (String) triggerPulseLength + "ms.");
          delay(triggerPulseLength);
          digitalWrite (shutterPin, LOW); // turn off the shutterED          
        }
        
      }
    }
};


class ToggleShootService: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("New value: " + (String) value.c_str() );        
        
        // Reset any cancels
        globalCancel = false;        
        if (value == "toggleOn") {
          digitalWrite (shutterPin, HIGH);  // turn on the shutter
          Serial.println("Toggle shutter ON");
        } else if (value == "toggleOff") {
          digitalWrite (shutterPin, LOW);  // turn on the shutter
          Serial.println("Toggle shutter OFF");
        }
        
      }
    }
};

class GlobalCancelService: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        
        Serial.println("New value: " + (String) value.c_str() );        
        
        if (value == "yescancel") {          
          globalCancel = true;
          Serial.println("Performing Global cancel.");
          digitalWrite (shutterPin, LOW);  // turn Off the shutter
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  simpleDelay_Timer.reset();

  // setup pin as a digital output pin
  pinMode (shutterPin, OUTPUT);  

  BLEDevice::init(BLE_DEVICE_ID);
  BLEServer *pServer = BLEDevice::createServer();

  // Global Cancel Service:
  BLEService *pServiceCancel = pServer->createService(SVC_UUID_GLOBALCANCEL);
  BLECharacteristic *pCharCancel = pServiceCancel->createCharacteristic(
                                         CHAR_UUID_CANCEL_CMD,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharCancel->setCallbacks(new GlobalCancelService());
  pServiceCancel->start();    

  // Simple Shoot Service:
  BLEService *pServiceSimpleShoot = pServer->createService(SVC_UUID_SIMPLESHOOT);
  BLECharacteristic *pCharSimpleShootCmd = pServiceSimpleShoot->createCharacteristic(
                                         CHAR_UUID_SIMPLESHOOT_CMD,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharSimpleShootCmd->setCallbacks(new SimpleShootService());
  pServiceSimpleShoot->start();

  // Toggle Shoot Service:
  BLEService *pServiceToggleShoot = pServer->createService(SVC_UUID_TOGGLESHOOT);
  BLECharacteristic *pCharToggleCmd = pServiceToggleShoot->createCharacteristic(
                                         CHAR_UUID_TOGGLESHOOT_CMD,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharToggleCmd->setCallbacks(new ToggleShootService());
  pServiceToggleShoot->start();

  // Pulse Shoot Service:
  BLEService *pServicePulseShoot = pServer->createService(SVC_UUID_PULSESHOOT);
  // =========> Define CHaracteristics & Callbacks below ======>>>>
  pServicePulseShoot->start();




  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {

  // Handle simple delay timer, if requested by callback
  if ((simpleDelayTime > 0) && !globalCancel) {
    // Delay detected, turning on shutter:
    if (!shutterOn) {      
      Serial.print("Begin Shutter - ON..." + (String) simpleDelayTime + "ms. ");
      digitalWrite (shutterPin, HIGH);  // turn on shutter
      shutterOn = true;
      simpleDelay_Timer.reset();
    }
    
    if (simpleDelay_Timer.intervalMs () > simpleDelayTime) {
      digitalWrite (shutterPin, LOW); // turn off the shutter
      shutterOn = false;
      simpleDelay_Timer.reset ();
      simpleDelayTime = 0;
      
      Serial.print("...");      
      Serial.println("End Shutter - OFF.");      
      Serial.println ("interval: "+ (String) simpleDelay_Timer.intervalMs() ); 
      Serial.println ("simpleDelayTime: "+ (String) simpleDelayTime ); 
      Serial.println ("tick"); 
    }
  }
  
}
