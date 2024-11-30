//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"
#include <ESP32_Servo.h> 

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
#define TURN_PIN 5
#define GO_PIN 3
#define MIN_TURN 700
#define MAX_TURN 2000
#define STOP_SPEED 1500
#define MAX_SPEED_GO 2000
#define MAX_SPEED_BACK 1000
Servo turn;
Servo sgo;
char command;
String string;
int svangle = 0;
int slideBarValue = 50;
int index_ = 0;
String aCmd;

int speeds = STOP_SPEED;
int gear = 0;

void carGo(int st){
  sgo.writeMicroseconds(st);
  Serial.println(st);
  delay(10);
}


void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }

  string = "";
  while(SerialBT.available() > 0)
  {
    command = ((byte)SerialBT.read());
    if(command == ':')
    {
      break;
    }
    else
    {
      string += command;
    }
    delay(1);
  }
  //if(string != "")  Serial.println(string);
  
  while( string.length() >= 3 ){
      aCmd = string.substring(0, 3);
      string = string.substring(3);
      Serial.println(" " + aCmd);

      index_ = aCmd.lastIndexOf("T");
      if( aCmd == "GOO"  ){
        // Move the car 
        carGo(MAX_SPEED_GO);
      } else if( aCmd == "STG" ){
        carGo(STOP_SPEED);
        // Stop the car
      } else if( aCmd == "BAC" ){
        // Move the car back
        carGo(MAX_SPEED_BACK);
      } else if( aCmd == "STB" ){
        // Stop the car
        carGo(STOP_SPEED);
      } else if( index_ == 0 ){
        // Turn left/right: cmd = "T<value from 0 to 100>"
          slideBarValue = aCmd.substring(index_+1).toInt();
          //Serial.println(slideBarValue );
          if( slideBarValue > 0 ){
            //turn.attach(TURN_PIN);
            svangle = map(slideBarValue, 0, 100, MIN_TURN, MAX_TURN);
            turn.writeMicroseconds(svangle);
          }
      } else if ( aCmd.lastIndexOf("S") == 0 ){
        speeds = aCmd.substring(1).toInt();
        if( speeds > 0 ){
          speeds -= 15;
          if( gear == 3 ){
            sgo.writeMicroseconds( map(speeds, 0, 100, STOP_SPEED, MAX_SPEED_GO) );
          } else if( gear == 1 ){
            sgo.writeMicroseconds( map(speeds, 0, 100, STOP_SPEED, MAX_SPEED_BACK) );
          }
          delay(10);
        }
      } else if ( aCmd.lastIndexOf("G") == 0 ){
        gear = aCmd.substring(1).toInt();
      }
  }
 
}
