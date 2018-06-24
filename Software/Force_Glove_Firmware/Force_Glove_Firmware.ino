/*
 *  File Name: Force_Glove_Firmware.ino
 *  
 *  Application: Force Glove Light Activation
 *  
 *  Description: This code is for the DFRobot CurieNano device.  This
 *  will trigger MQTT responses to activate and deactivate smart plugs.
 *  
 *  Author: M. Sperry - http://www.youtube.com/misperry
 *  Date: 06/10/2018
 *  Revision: 1.0
 *  
 *  
 */
#include <Wire.h>
#include <DFRobot_QMC5883.h>
#include "CurieIMU.h"
#include "Filter.h"

int lastOrientation = - 1; // previous orientation (for comparison)

int orientationCount = 0;

bool orientationSet = false;


DFRobot_QMC5883 compass;

void setup()
{
  Serial.begin(115200);

  pinMode(2,OUTPUT);
  digitalWrite(3,LOW);
  

  // initialize device
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
  
}

void loop()
{
  int orientation = - 1;   // the board's orientation
  String orientationString; // string for printing description of orientation

  // read accelerometer:
  int x = CurieIMU.readAccelerometer(X_AXIS);
  int y = CurieIMU.readAccelerometer(Y_AXIS);
  int z = CurieIMU.readAccelerometer(Z_AXIS);
  
  // calculate the absolute values, to determine the largest
  int absX = abs(x);
  int absY = abs(y);
  int absZ = abs(z);

  //check accelerometers to see what orientation the device is in
  if ( (absZ > absX) && (absZ > absY)) {
    // base orientation on Z
    if (z > 0) {
      orientationString = "up";
      orientation = 0;  
    } else {
      orientationString = "down";
      orientation = 1;
    }
  } else if ( (absY > absX) && (absY > absZ)) {
    // base orientation on Y
    if (y > 0) {
      orientationString = "digital pins up";
      orientation = 2;
    } else {
      orientationString = "analog pins up";
      orientation = 3;
    }
  } else {
    // base orientation on X
    if (x < 0) {
      orientationString = "connector up";
      orientation = 4;
    } else {
      orientationString = "connector down";
      orientation = 5;
    }
  }
  
  // if the orientation has changed, print out a description:
  if (orientation != lastOrientation) {
    Serial.println(orientationString);
    lastOrientation = orientation;
  }

  Serial.print("Orientation:");
  Serial.println(orientation);

  //check to see if it is the correct orientation to 
  //activate the pin for the ESP8266
  if (orientation == 2 && orientationSet == false)
  {
    digitalWrite(2,HIGH);
    Serial.println("line toggle");
    orientationSet = true;
  }
  else if (orientation == 1 && orientationSet == true)
  {
    orientationSet = false;
    Serial.println("Line Clear");
    digitalWrite(2,LOW);
  }
    delay(300);
}
