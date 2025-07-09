// config.h
#ifndef CONFIG_H
#define CONFIG_H

#define RPM_DISPLAY_MIN 0      // Minimum RPM value that can be shown on the slider.

#define RPM_DISPLAY_MAX 1000   // Maximum RPM value that can be shown on the slider.

#define RPM_MIN 250            // Starts flashing when RPM drops *below* this value (shift down).

#define RPM_MAX 750            // Starts flashing when RPM exceeds *above* this value (shift up).

#define TEMP_MAX 100           // Startsts flashing when oil temperature rises *above* this value. Celcius.

#define PRESSURE_MIN 60        // Starts flashing when oil pressure falls *below* this value. kPa.

#define VOLTAGE_MIN 12         // Starts flashing when voltage drops *below* this value.
                               
                               
#define BAR_HEIGHT 060         // Height of Red, Blue, Green Bar and Slider. maximum is 480 (height of screen).

/*
If you have any issues, please contact Ethan Maya at ethanmayaemail@gmail.com or if it's urgent, preferably whatsapp me or give me a ring on 07598 413640.
*/

#endif // CONFIG_H
