#include "Pin.h"
#include "GPIOSystem.h"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <algorithm>

#define PATH_ADC "/sys/bus/iio/devices/iio:device0/in_voltage"

using namespace std;

int readAnalog(int number){
   stringstream ss;
   ss << PATH_ADC << number << "_raw";
   fstream fs;
   fs.open(ss.str().c_str(), fstream::in);
   fs >> number;
   fs.close();
   return number;
}

void bbb_joystick_read(char & direction, bool & photo, bool & button) {
   int potentiomer_value;
   Pin pin_button {"P9_27", GPIOSystem::Direction::IN, GPIOSystem::Value::LOW};

   while (true) {
		// Read values
        potentiomer_value = readAnalog(1);
        usleep(50000);
		// Set Directions
        if (abs(readAnalog(1) - potentiomer_value) > 10) {
                if (readAnalog(1) - potentiomer_value > 0) direction = 'l';
                else if (readAnalog(1) - potentiomer_value < 0) direction = 'r';
        } else direction = 's';
        if (abs(potentiomer_value - 4096) <= 3) direction = 'l';
        else if (potentiomer_value <= 7) direction = 'r';
		// Set shot
	    photo = (readAnalog(3) <= 35);
		// Set button
		button = (pin_button.getValue() == GPIOSystem::Value::HIGH);	
   }
}