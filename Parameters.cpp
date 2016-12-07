#include "Parameters.h"

/*

	Implementation of the Parameters class methods.  Class definition and accessors are 
	included in the Parameters header file.  The overall class description is also included
	in the header file, right above the class definition.

*/

std::string Parameters::names[] = {"eggs", "instar1", "instar2", "instar3", "pupae", "males", "females" }; // lifestage names
std::string Parameters::mortalityParams[] = {"max", "min temp", "max temp", "tau", "beta0", "beta1", "beta2", "beta3" }; // mortality parameter names 
std::string Parameters::fruitParams[] = {"n", "m", "time lag", "base temp", "gt multiplier", "harvest cutoff", "harvest drop" }; // fruit model parameter names

// lengths of the above arrays
int Parameters::NUM_NAMES = 7; 
int Parameters::NUM_MORT_PARAMS = 8;
int Parameters::NUM_FRUIT_PARAMS = 7;

// useful method: splitting strings by a specified delimiter char
/*
    Function to split a given string by a specified character.  Returns a vector where the elements are
    consecutive delimited sections of the string to split (delimiter not included).
*/
std::vector<std::string> splitBy(const char delim, const std::string toSplit) {
    std::vector<std::string> div;

    std::string temp = "";
    for (int i = 0; i < toSplit.size(); ++ i) {
        if (toSplit[i] != delim) // if the current character is not the delimiter, it's part of the current section
            temp += toSplit[i];
        if ((toSplit[i] == delim || i == toSplit.size() - 1) && temp != "") {
            // if the current character is the delimiter, or it's the last character, and the current section is non-empty,
            // then add it to the vector and reset the current section to ""
            div.push_back(temp);
            temp = "";
        }
    }
    return div;
}

// default constructor - uses default parameters
Parameters::Parameters() {
	setDefaultParams();
}

// constructor taking a filename to read the parameters in from
// first set default parameters, then read in the file and if there are any
// parameters missing the default values for these parameters will be used
Parameters::Parameters(const std::string fileName) {
	setDefaultParams(); 
	setConfigParams(fileName); 
}

// constructor taking another parameters object to read parameters from
Parameters::Parameters(const Parameters &params) {
	setDefaultParams();
	setCopyParams(params, true); // yes copy the fruit params
}

// method to set the parameters to the default parameters
void Parameters::setDefaultParams() {
	// fruit parameters
	paramMap["fruit n"] = 4.;
	paramMap["fruit m"] = 0.75;
	paramMap["fruit time lag"] = 50.;
	paramMap["fruit base temp"] = 4.;
	paramMap["fruit gt multiplier"] = 4.;
	paramMap["fruit harvest cutoff"] = 0.95;
	paramMap["fruit harvest drop"] = 0.1;
	
	// diapause parameters
	paramMap["diapause critical temp"] = 18.;
	paramMap["diapause daylight hours"] = 10.;
	
	// general parameters
	paramMap["time"] = 100.;
	paramMap["constant temp"] = 15.;
	paramMap["male proportion"] = 0.5;
	paramMap["latitude"] = 45.7; // Clark's latitude
	
	// initial populations
	paramMap["initial eggs"] = 0.; // 3.;
	paramMap["initial instar1"] = 0.; //5.;
	paramMap["initial instar2"] = 0.; //7.;
	paramMap["initial instar3"] = 0.; //4.;
	paramMap["initial pupae"] = 0.; //8.;
	paramMap["initial males"] = 0.; //5.;
	
	for (int i = 0; i < 7; i ++) { // initial populations of each female lifestage 
		std::string stageStr = "initial females";
		int val = i + 1;
		std::stringstream sstm;
		sstm << val;
		stageStr += sstm.str();
		paramMap[stageStr] = 0.; //4.;
	}

	paramMap["initial females1"] = 10.;

	// fertility parameters
	
	paramMap["fertility tmax"] = 30.;
	
	// stage-specific parameters
	for (int i = 0; i < 13; i ++) {
		std::string stage = "females";
		int val = i - 5;
		std::stringstream sstm;
		sstm << val;
		stage += sstm.str();
		if (i < 6)
			stage = names[i];
		
		// mortality parameters
		paramMap[stage + " mortality min temp"] = 3.;
		paramMap[stage + " mortality max temp"] = 33.;
		paramMap[stage + " mortality tau"] = 8.1776;
		paramMap[stage + " mortality beta1"] = -0.0077;
		paramMap[stage + " mortality beta2"] = 0.00032;
		paramMap[stage + " mortality beta3"] = -0.000002;		
		
		paramMap[stage + " mortality due to predation"] = 0.; // stage-specific mortality rate due to predation
	}
	
	paramMap[names[0] + " development max"] = 0.72; // eggs
	paramMap[names[0] + " mortality max"] = 0.3288;
	paramMap[names[0] + " mortality beta0"] = 0.1602;
	
	paramMap[names[1] + " development max"] = 0.94; // inst1
	paramMap[names[1] + " mortality max"] = 0.2688;
	paramMap[names[1] + " mortality beta0"] = 0.1402;
	
	paramMap[names[2] + " development max"] = 0.68; // inst2
	paramMap[names[2] + " mortality max"] = 0.1020;
	paramMap[names[2] + " mortality beta0"] = 0.0846;
	
	paramMap[names[3] + " development max"] = 0.32; // inst3
	paramMap[names[3] + " mortality max"] = 0.1068;
	paramMap[names[3] + " mortality beta0"] = 0.0862;
	
	paramMap[names[4] + " development max"] = 0.17; // pupae
	paramMap[names[4] + " mortality max"] = 0.0303;
	paramMap[names[4] + " mortality beta0"] = 0.0607;
	
	paramMap[names[5] + " mortality max"] = 0.1398; // males
	paramMap[names[5] + " mortality beta0"] = 0.0972;
	
	paramMap["females1 development max"] = 1./80; // fem1
	paramMap["females1 mortality max"] = 0.0537; 
	paramMap["females1 mortality beta0"] = 0.0685;
	
	paramMap["females2 development max"] = 1./10; // fem2
	paramMap["females2 mortality max"] = 0.1200; 
	paramMap["females2 mortality beta0"] = 0.0906;
	
	paramMap["females3 development max"] = 1./10; // fem3
	paramMap["females3 mortality max"] = 0.4500; 
	paramMap["females3 mortality beta0"] = 0.2006;
	
	paramMap["females4 development max"] = 1./5; // fem4
	paramMap["females4 mortality max"] = 0.0; 
	paramMap["females4 mortality beta0"] = 0.0506;
	
	paramMap["females5 development max"] = 1./4; // fem5
	paramMap["females5 mortality max"] = 0.7500; 
	paramMap["females5 mortality beta0"] = 0.3006;
	
	paramMap["females6 development max"] = 1./5; // fem6
	paramMap["females6 mortality max"] = 0.6000; 
	paramMap["females6 mortality beta0"] = 0.2506;
	
	paramMap["females7 mortality max"] = 0.8367; // fem7
	paramMap["females7 mortality beta0"] = 0.3295;
	
	// female-stage specific egg viabilities
	paramMap["females1 egg viability"] = 0.832;
	paramMap["females2 egg viability"] = 0.807;
	paramMap["females3 egg viability"] = 0.763;
	paramMap["females4 egg viability"] = 0.556;
	paramMap["females5 egg viability"] = 0.324;
	paramMap["females6 egg viability"] = 0.257;
	paramMap["females7 egg viability"] = 0.;
}

// method to set the parameters to those specified in the file
// errormsg is returned with Success! if nothing went wrong, or specifying what the error was if one occurred
errormsg Parameters::setConfigParams(const std::string configFileName) {
	int lineNumber = 1; 
	bool changed = false; // check if any valid params are in the specified file (if nothing changed, file is invalid)
	
	std::map<std::string, double> newParams = getMap();

	std::string status = "";

	if (&configFileName == NULL) {
		status += "No file chosen, using previous parameters"; // file chooser closed with no file chosen
		return status;
	}
	
	std::ifstream fileIn(configFileName.c_str());
	if (fileIn.bad()) {
		status += "File not found, using default parameters.";
		return status;
	}
	std::string line;

	while (std::getline(fileIn, line)) // read file line-per-line
	{
	    std::vector<std::string> splitLine = splitBy(':', line); // has to be name : value
	    if (splitLine.size() < 2) {
	    	//status += "Input error - value not present - on line " + lineNumber;
	    	continue;
	    }

	    std::string key = splitLine[0];
	    double val;
	    std::istringstream iss(splitLine[1]);
	    if (! (iss >> val)) {
	    	status += "Input error - invalid value - on line " + lineNumber;
	    	break;
	    }

	    if (newParams.count(key) > 0) { // this is the reason params must already exist 
	    	newParams[key] = val;       // they are replaced by the new value if the key of the new one is valid
	    	changed = true;             // this way you don't end up with polluted map of invalid parameter names 
	    }
	    lineNumber ++;
	}

	fileIn.close();

	if (!changed) {
		status += "No parameters were changed (no valid input in this file).";
	}
	else {
		std::string validMessage = checkMap(newParams, true, true); // check all params
		if (!(validMessage.compare("Success!") == 0))
			return validMessage;
		
		setMapParams(newParams, true); // if it's reached this point, the parameters are all ok and the paramMap datafield can be reset

		status += "Success!";
	}

	return status;
}

// method to reset the parameters to those in the specified map
// can specify whether or not to reset the fruit parameters
// same error message idea as for setting params from a file
errormsg Parameters::setMapParams(std::map<std::string, double> inputMap, bool resetFruitParams) {
	std::string validMessage = checkMap(inputMap, resetFruitParams, true); // check fruit params if resetting

	if (! (validMessage.compare("Success!") == 0)) {
		return validMessage;
	}

	// if this point has been reached, all the parameters are fine
	for (std::map<std::string, double>::iterator iter = paramMap.begin(); iter != paramMap.end(); iter ++) {
		std::string key = iter->first;
		if (!(splitBy(' ', key)[0].compare("fruit") == 0) || resetFruitParams) // if it's not a fruit parameter, or if we're resetting the fruit parameters
			if (paramMap.count(key) > 0) 
				paramMap[key] = inputMap[key]; 
	}
	return validMessage; // at this point it'll be "Success!", otherwise would've bailed at 1st if statement
}

// same as resetting parameters from a map, but taking a parameters object 
// calls the setMapParams method with the map of the paramater object passed in
errormsg Parameters::setCopyParams(const Parameters &params, bool resetFruitParams) {
	return setMapParams(params.getMap(), resetFruitParams);
}

// method to return the map of params
// a const method, to avoid unsolicited edits of the map
// (this allows memory-saving shallow copy without worrying about data being edited improperly)
std::map<std::string, double> Parameters::getMap() const {
	return paramMap;
}

// method to return a single parameter, specified by name
double Parameters::getParameter(std::string parameter) const {
	return paramMap.find(parameter)->second;
}

// method to set the value of a specified parameter, to that specified
// same errormsg idea as before (error is returned if the specified parameter is invalid)
errormsg Parameters::setParameter(const std::string &parameter, double newValue) {
	std::map<std::string, double> thisMap = getMap(); 

	std::string validMessage = "Invalid parameter";
	
	if (thisMap.count(parameter) == 0) 
		return validMessage;
	
	thisMap[parameter] = newValue; 
	
	validMessage = checkMap(thisMap, true, true); // check all parameters (for invalid values)
	
	if (validMessage.compare("Success!") == 0) 
		paramMap[parameter] = newValue;
	
	return validMessage;
}

// method to set the max mortality of a specified stage to a specified value
// this can also be done with the setParameter method, but there is a specific method for 
// it since it is called more commonly 
errormsg Parameters::setMaxMort(int stage, double newMaxMort) {
	std::string status = "";
	// check for potential errors
	if (stage > 12) {
		status += "Invalid stage!";
		return status;
	}
	if (newMaxMort < 0) {
		status += "mortality max is positive";
		return status;
	}
	
	// if nothing went wrong, update the map
	
	if (stage < 6) // not a female stage
		paramMap[names[stage] + " mortality max"] = newMaxMort;
	else  // female stage
		paramMap["females" + (stage - 5) + std::string(" mortality max")] = newMaxMort;
	
	status += "Success!";
	return status;
}

// method to reset only the fruit params, to those specifed in the map passed in
// same error message idea as before
errormsg Parameters::resetFruitParams(std::map<std::string, double> fruitMap) {
	std::string validMessage = checkMap(fruitMap, true, false); // check the fruit params but not the other params

	if (validMessage.compare("Success!") == 0) {
		for (int i = 0; i < NUM_FRUIT_PARAMS; i ++) {
			if (fruitMap.count("fruit " + fruitParams[i]) > 0) {
				paramMap["fruit " + fruitParams[i]] = fruitMap["fruit " + fruitParams[i]];
			} else {
				validMessage = "Not all parameters were found!";
				return validMessage;
			}
		}
	}
	return validMessage; // either broken from 1st if, or still "Success!"
}

// method to get a vector of the initial female stage-specific populations
std::vector<double>  /*pointer to start of array*/ Parameters::getInitialFemales() const {
	std::vector<double> initialFem; // there are 7 female lifestages
	
	for (int i = 0; i < 7; i ++)
		initialFem.push_back(paramMap.find("initial females" + (i + 1))->second);
	
	return initialFem;
}

// method to get a vector of the beta values (see mortality equation) for a specified stage
std::vector<double> Parameters::getMortBetas(std::string &stage) const {
	std::vector<double> betas;
		
	for (int i = 0; i < 4; i ++)
		betas.push_back(paramMap.find(stage + " mortality beta" + i)->second);
	
	return betas;
}

// method to get the string name corresponding to the int stage specified
// Invalid Stage! is returned if the stage is invalid (i.e. < 0 or > 12)
std::string Parameters::getStage(int stage) const {
	std::string stageStr = "";
	if (stage > 12) {
			stageStr += "Invalid stage!";
	} else {
		
		if (stage < 6) // not a female stage
			stageStr += names[stage];
		else { // female stage
			int val = stage - 5;
			stageStr += "females";
			std::stringstream sstm;
			sstm << val;
			stageStr += sstm.str();
		}
	}

	return stageStr;
}

// method to print the fruit parameters to the output filestream specified
void Parameters::printFruitParamsToFile(std::ofstream &fileOut) const {
	for (int i = 0; i < NUM_FRUIT_PARAMS; i ++)
			fileOut << "fruit " << fruitParams[i] << ": " << paramMap.find("fruit " + fruitParams[i])->second << "\n";
}

// method to print all the parameters to the output filestream specified
// can specify with a boolean whether or not to print the fruit parameters too
void Parameters::printToFile(std::ofstream &fileOut, bool printFruitParams) const {
	if (printFruitParams) { // if the user wanted to print the fruit parameters
		printFruitParamsToFile(fileOut);
	}
	
	fileOut << "\n";
	
	// diapause parameters
	fileOut << "diapause critical temp: " << paramMap.find("diapause critical temp")->second << "\n";
	fileOut << "diapause daylight hours: " << paramMap.find("diapause daylight hours")->second << "\n";
	
	fileOut << "\n";
	
	// male proportion
	fileOut << "male proportion: " << paramMap.find("male proportion")->second << "\n";
	fileOut << "\n";
	
	// latitude
	fileOut << "latitude: " << paramMap.find("latitude")->second << "\n";
	fileOut << "\n";
	
	// lifestage-specific parameters
	for (int i = 0; i < 13; i ++) {
		std::string stage = getStage(i); // current stage
		
		fileOut << "initial " << stage << ": " << paramMap.find("initial " + stage)->second << "\n"; // initial populations
		
		if (i != 5 && i != 12) { // recall: no development rate for adult males or females7
			// development parameters
			fileOut << stage << " development max: " << paramMap.find(stage + " development max")->second << "\n"; 
		}
		
		// mortality parameters
		for (int j = 0; j < NUM_MORT_PARAMS; j ++)  
			fileOut << stage << " mortality " << mortalityParams[j] << ": " << paramMap.find(stage + " mortality " + mortalityParams[j])->second << "\n";
		
		fileOut << stage << " mortality due to predation: " << paramMap.find(stage + " mortality due to predation")->second << "\n"; 
		
		if (i > 5) { // so, if the female stages have been reached
			fileOut << stage << " egg viability: " << paramMap.find(stage + " egg viability")->second << "\n"; // egg viability for each female stage
		}
		
		fileOut << "\n";
	}
}

// method to check the validity of the parameters in the specified map
// can specify whether or not to check the fruit params and/or the non fruit params
// an error message is returned specifying the invalid value, or Success! if every parameter is valid
// this is mostly used to check a map before updating it with a new value specified by a param updating method
errormsg Parameters::checkMap(std::map<std::string, double> toCheck, bool checkFruitParams, bool checkNonFruitParams) const {
	std::string status = "";
	if (checkFruitParams) { // if the user wants to check fruit parameters
		if (toCheck["fruit m"] < 0 || toCheck["fruit m"] > 1) {
			status += "m is between 0 and 1 inclusive";
			return status;
		}
		if (toCheck["fruit time lag"] < 0) {
			status += "time lag is positive";
			return status;
		}
		if (! (0 <= toCheck["fruit harvest cutoff"] && toCheck["fruit harvest cutoff"] <= 1)) {
			status += "fruit harvest cutoff is between 0 and 1 inclusive";
			return status;
		}
		if (! (0 <= toCheck["fruit harvest drop"] && toCheck["fruit harvest drop"] <= 1)) {
			status += "fruit harvest drop is between 0 and 1 inclusive";
			return status;
		}
	}
	
	if (checkNonFruitParams) { // if the user wants to check non-fruit parameters
		
		if (toCheck["diapause daylight hours"] < 0 || toCheck["diapause daylight hours"] > 24 ) {
			status += "diapause daylight hours is between 0 and 24 inclusive";
			return status;
		}
		
		if (toCheck["male proportion"] < 0 || toCheck["male proportion"] > 1) {
			status += "male proportion is between 0 and 1 inclusive";
			return status;
		}
		
		for (int i = 0; i < 13; i ++) { // stage-specific parameters
			std::string stage = getStage(i); // current stage
			
			if (toCheck["initial " + stage] < 0) {
				status += "initial populations are positive";
				return status;
			}
			
			if (toCheck[stage + " mortality max"] < 0) {
				status += "mortality max is positive";
				return status;
			}
			if (toCheck[stage + " mortality due to predation"] < 0) {
				status += "mortality due to predation is positive";
				return status;
			}
			
			if (i != 5 && i != 12) {
				if (toCheck[stage + " development max"] < 0) {
					status += "development max is positive";
					return status;
				}
			}
			if (i > 5) {
				if (toCheck[stage + " egg viability"] < 0) {
					status += "egg viability is positive";
					return status;
				}
			}
			
		}
	}
	
	status += "Success!";
	return status; // if nothing was returned yet, then there were no errors and the parameters are all fine
}

// method to get a set of specified parameters (i.e. parameters which occur in a set) as an array
// the options are: initial populations, mortality parameters, development parameters, or egg viabilities
std::vector<double> Parameters::getArrayParameters(std::string parameterName) const {
	std::string type = splitBy(' ', parameterName)[0]; // first word in the specified parameter
	
	std::vector<double> toReturn;
	int toRetLength = 0;
	
	if (type.compare("mortality") == 0 || type.compare("initial") == 0) // mortality parameters and initial populations are applicable to all lifestages
		toRetLength = 13;
	else if (type.compare("development") == 0) // development parameters only applicable for the non-advanced adult stages
		toRetLength = 11;
	else if (type.compare("egg") == 0) // egg viability (only applicable for the female stages)
		toRetLength = 7;
	else if (type.compare("constant") == 0) // constant development rate, only applicable for the first 6 female stages (i.e. not the last stage)
		toRetLength = 6;
	else
		return toReturn; // if the type was none of the above, then it's invalid
	
	if (toRetLength == 13 || toRetLength == 5 || toRetLength == 11) { // mortality, initial populations, or development
		for (int i = 0; i < 6; i ++) {
			if (toRetLength != 5 || i < 5) { // development parameters not applicable for adult stages
				if (type.compare("initial") == 0)
					toReturn.push_back(paramMap.find(parameterName + " " + names[i])->second);
				else if (type.compare("development") == 0) {
					if (i < 5)
						toReturn.push_back(paramMap.find(names[i] + " " + parameterName)->second);
				}
				else
					toReturn.push_back(paramMap.find(names[i] + " " + parameterName)->second);
			}
		}
	}
	
	if (toRetLength > 5) { // mortality, initial populations, constant dev rate, or egg viabilities
		for (int i = 0; i < 7; i ++) {
			if (toRetLength == 13) { // mortality or initial populations
				if (type.compare("initial") == 0) {
					std::string stageStr = parameterName + " females";
					int val = i + 1;
					std::stringstream sstm;
					sstm << val;
					stageStr += sstm.str();
					toReturn.push_back(paramMap.find(stageStr)->second);
				}
				else {
					std::string stageStr = "females";
					int val = i + 1;
					std::stringstream sstm;
					sstm << val;
					stageStr += sstm.str();
					stageStr += std::string(" ") + parameterName; 
					toReturn.push_back(paramMap.find(stageStr)->second);
				}
			}
			else { // constant dev rate or egg viabilities
				if (toRetLength != 6 || i < 6) { // egg viabilities not applicable for female stage 7
					if (type.compare("development") == 0) {
						if (i < 6) {
							std::string stageStr = "females";
							int val = i + 1;
							std::stringstream sstm;
							sstm << val;
							stageStr += sstm.str();
							stageStr += std::string(" ") + parameterName; 
							toReturn.push_back(paramMap.find(stageStr)->second);
						}
					}
					else {
						std::string stageStr = "females";
						int val = i + 1;
						std::stringstream sstm;
						sstm << val;
						stageStr += sstm.str();
						stageStr += std::string(" ") + parameterName; 
						toReturn.push_back(paramMap.find(stageStr)->second);
					}
				}
			}
		}
			
	}
	
	return toReturn; 
}
