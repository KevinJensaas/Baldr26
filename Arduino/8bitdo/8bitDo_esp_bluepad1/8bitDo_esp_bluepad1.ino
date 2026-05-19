#include "Arduino.h"
#include <SoftwareSerial.h>
#include <Bluepad32.h>
 
ControllerPtr myControllers[BP32_MAX_GAMEPADS];
SoftwareSerial MicroSerial(-1, 1);
 
int remapJoystick(int value) {
  value = constrain(value, -512, 512);
  return map(value, -512, 512, 0, 1023);
}
 
// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      //Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
     // Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
   //                 properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    //Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}
 
void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;
 
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      //Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }
 
  if (!foundController) {
    //Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}
 
/*
int lx = remapJoystick(ctl->axisX()),  // (0 - 1023) left X Axis
int ly = remapJoystick(ctl->axisY()),  // (0 - 1023) left Y axis
int rx = remapJoystick(ctl->axisRX())  // (0 - 1023) right X axis
*/
 
void dumpGamepad(ControllerPtr ctl) {
  /*
  Serial.printf("BUTTON: %d, LX: %d, LY: %d, RX: %d, RY: %d\n",
    ctl->buttons(),  // Bitmask of pressed buttons
    remapJoystick(ctl->axisX()),    // (-511 - 512) left X axis
    remapJoystick(ctl->axisY()),    // (-511 - 512) left Y axis
    remapJoystick(ctl->axisRX()),    // (-511 - 512) right X axis
    remapJoystick(ctl->axisRY())
  );
  */
}
 
void processGamepad(ControllerPtr ctl) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...
  int lx = remapJoystick(ctl->axisX());   // (0 - 1023) left X Axis
  int ly = remapJoystick(ctl->axisY());   // (0 - 1023) left Y axis
  int rx = remapJoystick(ctl->axisRX());  // (0 - 1023) right X axis
  int ry = remapJoystick(ctl->axisRY());
  int buttons = ctl->buttons();
  int8_t byte = 114;
  if (ctl->a()) {
    static int colorIdx = 0;
    // Some gamepads like DS4 and DualSense support changing the color LED.
    // It is possible to change it by calling:
    switch (colorIdx % 3) {
      case 0:
        // Red
        ctl->setColorLED(255, 0, 0);
        break;
      case 1:
        // Green
        ctl->setColorLED(0, 255, 0);
        break;
      case 2:
        // Blue
        ctl->setColorLED(0, 0, 255);
        break;
    }
    colorIdx++;
  }
 
  if (ctl->b()) {
    // Turn on the 4 LED. Each bit represents one LED.
    static int led = 0;
    led++;
    // Some gamepads like the DS3, DualSense, Nintendo Wii, Nintendo Switch
    // support changing the "Player LEDs": those 4 LEDs that usually indicate
    // the "gamepad seat".
    // It is possible to change them by calling:
    ctl->setPlayerLEDs(led & 0x0f);
  }
 
  //if (ctl->x()) {
    // Some gamepads like DS3, DS4, DualSense, Switch, Xbox One S, Stadia support rumble.
    // It is possible to set it by calling:
    // Some controllers have two motors: "strong motor", "weak motor".
    // It is possible to control them independently.
    //ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */,
    //                   0x40 /* strongMagnitude */);
  //}
 // MicroSerial.write(byte);
 //MicroSerial.printf("%4d\n", lx);
//MicroSerial.write(0x02);   // STX
MicroSerial.print(lx);
MicroSerial.print(',');
MicroSerial.print(ly);
MicroSerial.print(',');
MicroSerial.print(rx);
MicroSerial.print(',');
//MicroSerial.print(ry);
//MicroSerial.print(',');
MicroSerial.print(buttons);
MicroSerial.println();
//MicroSerial.write(0x03);   // ETX
  /*
  MicroSerial.printf("%4d, %4d, %4d, %4d\n", lx, ly, rx, ry);
  MicroSerial.println();
  MicroSerial.flush(); //
  */
  // Another way to query controller data is by getting the buttons() function.
  // See how the different "dump*" functions dump the Controller info.
  dumpGamepad(ctl);
}
 
void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      }
      /*
      else if (myController->isMouse()) {
        processMouse(myController);
      } else if (myController->isKeyboard()) {
        processKeyboard(myController);
      } else if (myController->isBalanceBoard()) {
        processBalanceBoard(myController);
      }
      */
      else {
        //Serial.println("Unsupported controller");
      }
    }
  }
}
 
 
 
// Arduino setup function. Runs in CPU 1
void setup() {
 // Serial.begin(115200);
  MicroSerial.begin(4800);
  //Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  //Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
 
  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);
 
  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  //BP32.forgetBluetoothKeys();
 
  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);
}
 
// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool dataUpdated = BP32.update();
  //if (dataUpdated)
    processControllers();
 
  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time
 
  //     vTaskDelay(1);
  delay(100);
}