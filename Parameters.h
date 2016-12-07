#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "UtilityMethods.h"

/*

	This object class represents all the parameters in the SWDCellSingle object.  The 
	parameters are stored in a map (they are all doubles) with String keys as the name
	of the parameter.
	There are various methods to access the parameters - the user can choose to get a 
	map of all the parameters, parameters in arrays, or individual parameters.  
	There are also methods to reset the values of the parameters through various means
	(reading from a file, a map, a Parameters object); the user can also specify to 
	not reset the fruit parameters, only reset the fruit parameters, or reset an 
	individual parameter.
	There is also a method to facilitate printing of the parameters to a file (in a 
	format so that it can later be read in).
	If there are errors in the input, the user is notified via a String describing the 
	error (if parameters are being reset) or an error is returned (if 
	parameters are being accessed).

	Methods described the implementation file, Parameters.cpp.

*/

typedef std::string errormsg; // the type for the messages returned from methods to indicate success or errors (really just a string)

class Parameters {

	std::map<std::string, double> paramMap; // string, double map of params (param name = key, double for the value)
	static std::string names[];
	static std::string mortalityParams[]; 
	static std::string fruitParams[];

	static int NUM_NAMES;
	static int NUM_MORT_PARAMS;
	static int NUM_FRUIT_PARAMS;

public:

	// constructors
	Parameters();
	Parameters(const std::string fileName);
	Parameters(const Parameters &params);

	// methods described in the implementation file
	errormsg setConfigParams(const std::string configFileName);
	void setDefaultParams();
	errormsg setMapParams(std::map<std::string, double> inputMap, bool resetFruitParams);
	errormsg setCopyParams(const Parameters& params, bool resetFruitParams);
	std::map<std::string, double> getMap() const;
	double getParameter(std::string parameter) const;
	errormsg setParameter(const std::string& parameter, double newValue);
	errormsg setMaxMort(int stage, double newMaxMort);
	errormsg resetFruitParams(std::map<std::string, double> fruitMap);
	std::vector<double>  /*pointer to start of array*/ getInitialFemales() const;
	std::vector<double> getMortBetas(std::string& stage) const; 
	std::string getStage(int stage) const;

	void printFruitParamsToFile(std::ofstream& fileOut) const;
	void printToFile(std::ofstream& fileOut, bool printFruitParams) const;
	errormsg checkMap(std::map<std::string, double> toCheck, bool checkFruitParams, bool checkNonFruitParams) const;

	std::vector<double> getArrayParameters(std::string parameterName) const;

};

#endif