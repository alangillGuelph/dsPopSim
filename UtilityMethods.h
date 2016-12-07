#ifndef UTILITY_METHODS_H
#define UTILITY_METHODS_H

#include <vector>
#include <string>
#include <sstream>

/*

	This is the header file for UtilityMethods.cpp - it contains the method declarations
	for the methods defined in UtilityMethods.cpp.  The comments/explanations for these methods
	are included with their definitions.
	This file also includes the class definition for a utility class HackPls.  The purpose of
	this class is described below.

*/

// methods defined in UtilityMethods.cpp

double round2Decimals(double toRound);

std::vector<double> copyDoubleArray(const double toCopy[], int len);

double sumDoubleArray(const double toSum[], int len);



// the singular purpose of this class is to allow the easy catenation of strings
// and ints, which isn't possible just with the + operator in C++
// the idea is to make a wrapper object for strings which has a one-argument constructor
// to allow for autocasting, and override the + operator for left and right string 
// catenation.  Then, strings and ints can be added Java-style with the expected result
class HackPls {
	std::string curStr;

public:
	HackPls(const char* pls) : curStr(pls) {}
	HackPls(const std::string pls) : curStr(pls) {}
	std::string getCurStr() { return curStr.c_str(); }

};

// implementation details in the UtilityMethods.cpp file 

std::string operator + (HackPls st1, int val);

std::string operator + (int val, HackPls st1);

#endif