#include "UtilityMethods.h"

/*

	This file has tool methods useful in various situations, but not specific
	to a particular file or purpose.  This is essentially a generic toolbox method
	collection.

*/


// method to round a double to 2 decimal places
// this is basically used to avoid loss of precision from adding the integration
// step repeatedly - it makes sure the timestep is updated properly without any
// precision errors
double round2Decimals(double toRound) {
	int round = (int) (toRound * 1000);
	// the if-else ensures standard rounding rules apply
	// i.e. 2.555 rounds to 2.56
	// and 2.554 rounds to 2.55
	if (round % 10 >= 5) 
		round = round/10 + 1;
	else
		round = round/10;
	return round/100. ;

}

// method to copy an array of doubles to a vector
std::vector<double> copyDoubleArray(const double toCopy[], int len) {
	std::vector<double> copiedArray;
	
	for(int i = 0; i < len; i ++)
		copiedArray.push_back(toCopy[i]);
	
	return copiedArray;
}

// method to compute the sum of an array of doubles
double sumDoubleArray(const double toSum[], int len) {
	double sum = 0;
	for (int i = 0; i < len; i ++)
		sum += toSum[i];
	return sum;
}



// implementation of the + operator overloading for commutative catenation 
// of strings with ints
// this is done using a stringstream 

std::string operator + (HackPls st1, int val) {
	std::stringstream sstm;
	sstm << st1.getCurStr() << val;
	return sstm.str();
}

std::string operator + (int val, HackPls st1) {
	std::stringstream sstm;
	sstm << val << st1.getCurStr();
	return sstm.str();
}