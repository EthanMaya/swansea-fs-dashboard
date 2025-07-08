# swansea-fs-dashboard
Includes code for the Elecrow esp32 (screen), Pico Can B (transmits Canbus), and Pico Adafruit PiCowbell Canbus (receives Canbus)
Current contributors: David Nwobi, Ethan Maya.


For Dashboard Screen.

If you are trying to edit configurations (rpm, oil temperature etc) to upload to screen.
1:
open VSCode (the blue app at the bottom). Wait 20 seconds for extensions to load up.
click on explorer (top left below the blue vscode logo that looks like 2 pages).
click on the directory called include directory.
click on the file config.h.
all editable config is in there. 
If you need anything changing which is not in there, please contact either David or Ethan. 

2:
To upload your changes to the screen:
plug in the screens white usb A cable into the usb connecter which (sometimes) works (left side of computer).
There may be a large 4 wire connector attached to the screen. make sure that is not plugged in.
If it doesn't work the first time, go to step 3.
Inside VSCode, click on the right facing arrow -> on the bottom middle left which says platformio upload on it when you hover over it. 
Once you have uploaded, plug the 4 wire connector back together.

3:
if it's not working (will say something like Errno 2 could not open port /dev/ttyUSB0), this means the port is'nt registering so it isn't connecting properly. 
Open terminal (the black box next to VScode) and type in (don't copy) the exact following text including spaces and *.
ls /dev/tty*

Press Enter. you should see a bunch of /dev/tty0, /dev/tty1 ... see if /dev/ttyUSB0 is one of them. If it isn't, unplug and replug until it is. Each time you plug it in, you have to type ls /dev/tty* and enter.
Optionally, you can press the up arrow to get ls /dev/tty* back in the terminal. 
Try plugging and unplugging at least 10 times.
Once /dev/ttyUSB0 is visible, restart step 2 (don't do the replugging step inside 2).








If you are trying to build and run the code for the first time, steps to get running are:

Install VSCode
search for python in vscode extensions and install.
search for platformio in vscode extenstions and install.

copy esp32-s3-devkitc-1-myboard.json located in Resources\PlatformIO into 
Windows C:\Users\<username>.\platformio\platforms\espressif32\boards
Linux ~/.platformio/platforms/espressif32/boards


esp32-s3-devkitc-1-myboard.json located in Resources\PlatformIO  has been copied into 
Windows C:\Users\<username>\.platformio\platforms\espressif32\boards
Linux ~/.platformio/platforms/espressif32/boards

The Screen code is located in ElecrowScreen\ElecrowScreen_PlatformIO

open VSCode in ElecrowScreen\ElecrowScreen_PlatformIO
