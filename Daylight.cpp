#include "Daylight.h"

/**
	
	This file has the methods necessary for computing the daylight hours of a particular
	timestep, with 0 = January first.
	This is needed in the diapause computations.

*/

// method for converting degrees to radians
double toRadians(double deg) {
	return deg * M_PI / 180;
}

// method for converting radians to degrees
double toDegrees(double rad) {
	return rad * 180 / M_PI;
}

// method for computing the solstice day of a given year
// if no valid year is given, it is assumed to be the 21st (since in general this is the value) 
int getSolsticeDay(int year) {
	
	int dates[] = {21,21,22,22,21,21,22,22,21,21,21,22,21,21,21,22,21,21,21,22,21};
	if(year < 2000 || year > 2020)
		return 21; // instead of crashing the program, just return 21 (which seems to be the mode anyway)
	else 
		return dates[year - 2000];
}

// method to get the solstice offset
int getOffSet(int year) {
	int solstice = getSolsticeDay(year);
	return 31 - solstice;
}

// method to compute daylight hours of a given timestep in a given year, at a given latitude
// the value returned is the number of daylight hours at the timestep provided (date parameter)
// which is used in the diapause computations
double getDayLightHours(int year, int date, double latitude) {
	double AXIS = 23.439;  // earth's axis.
	
	double j = M_PI / 182.625;
	double m = 1 - tan( toRadians( latitude)) * tan( toRadians( AXIS) * cos( j * date));
	
	/*
	 * From http://www,gandraxa.com/length_of_day.xml
	 * (to avoid future NaN's...)
	 * 
	 * Adjust range:
	 * 1. if m is negative, then the sun never appears the whole day long (polar winter): m must be adjusted 
	 * to 0 (the sun can not shine less than 0 hours).
	 * 2. if m is larger than 2, the "sun circle" does not intersect with the planet's surface and the sun is 
	 * shining the whole day (polar summer): m must be adjusted to 2 (the sun can not shine for more than 24 hours).
	 */
	
	if (m > 2)
		m = 2;
	if (m < 0)
		m = 0;
	
	return ((toDegrees(acos(1 - m)) / 180) * 24);
}


