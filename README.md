# swansea-fs-dashboard
Includes code for the Elecrow esp32 (screen), Pico Can B (transmits Canbus), and Pico Adafruit PiCowbell Canbus (receives Canbus)
Current contributors: David Nwobi, Ethan Maya.

For Dashboard Screen
If you are trying to build and run the code for the first time, make sure:

VSCode is installed

platformio is installed as a package on vscode

esp32-s3-devkitc-1-myboard.json located in Resources\PlatformIO  has been copied into  C:\Users\<username>\.platformio\espressif32\boards

The Screen code is located in ElecrowScreen\ElecrowScreen_PlatformIO

open VSCode in ElecrowScreen\ElecrowScreen_PlatformIO

add conf.h to .pio directory.
