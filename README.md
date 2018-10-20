## Project Summary

This is the firmware code for the SEN Trigger - A Smart Wireless Camera Trigger.

Initially written to run on an Adafruit Huzzah 32 based board. Additional Chipsets and prototype boards may be added in future.

## Notes on Compiling

This should compile easily with the Arduino IDE.

However a **Device ID** needs to be configured at compile time, to be loaded onto the device.

 1. Create a file named **Build_DeviceID.h**
 2. Place the file in the same folder as the arduino sketch file.
 3. A single line is required as follows:
 **#define BLE_DEVICE_ID               "Example Device ID"**
