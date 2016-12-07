#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <cmath>

/**
	
	Header file for the Daylight.cpp file
	For comments explaining the methods, refer to the Daylight.cpp 
	file, with the method implementations.

*/

double toRadians(double deg);

double toDegrees(double rad);

int getSolsticeDay(int year);

int getOffSet(int year);

double getDayLightHours(int year, int date, double latitude);

#endif