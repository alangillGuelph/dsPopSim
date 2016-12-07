#include "SWDSimulatorSingle.h"

/*

	Implementation of the SWDSimulatorSingle class - method declarations are 
	included in the SWDSimulatorSingle header file.  The overall class descriptions are also included
	in the header file, right above the class definition.

*/

// constructor taking the filename of a file to read parameters in from
// first, set the params to default params (that's what cell(Parameters()) does), 
// and then try reading from the file
// for any params missing from the file the default value is used
SWDSimulatorSingle::SWDSimulatorSingle(double dtNew, std::string fileName) : dt(dtNew), cell(Parameters()), injectFlies(false) {
	setConfigParams(fileName); // try to read from config.txt if it exists
}

// constructor taking a parameters object containing the params for the simulation
SWDSimulatorSingle::SWDSimulatorSingle(double dtNew, Parameters &params) : dt(dtNew), cell(params) , injectFlies(false) { }

// method to reset the simulation parameters to the default
void SWDSimulatorSingle::setDefaultParams() {
	Parameters params; // default params
	cell.resetCellParams(params, true); // reset the fruit parameters
}

// method to set the value of a single specified parameter to the specified value
// returns "Success!" if all goes well, or a descriptive error message if not
errormsg SWDSimulatorSingle::setSingleParameter(std::string &param, double newVal) {
	return cell.setSingleParameter(param, newVal);
}

// method to return the value of a specified parameter
double SWDSimulatorSingle::getSpecificParameter(std::string &param) const {
	return cell.getSpecificParameter(param);
}

// method to set the parameters to those listed in the specified file
// returns "Success!" if all goes according to plan, or a descriptive error message if not
errormsg SWDSimulatorSingle::setConfigParams(std::string &configFileName) { // returns an error message, or "Success!" if everything works
	
	Parameters params = Parameters(cell.getParams()); // current cell parameters

	errormsg toReturn = params.setConfigParams(configFileName);
	if (toReturn.compare("Success!") == 0)
		cell.resetCellParams(params, true); // apply changes to the cell, reset parameters to those read in (including fruit params)
	
	return toReturn;
}

// method to set the parameters to those contained in the specified map ((string paramname, double paramvalue) pairs in the map)
// same idea for errormsg as above
errormsg SWDSimulatorSingle::setMapParams(std::map<std::string, double> inputMap) {
	Parameters params = Parameters(cell.getParams()); // current cell parameters
	
	errormsg toReturn = params.setMapParams(inputMap, true);
	if (toReturn.compare("Success!") == 0)
		cell.resetCellParams(params, true); // reset fruit params in the single cell simulator
	
	return toReturn;
}

// method to run the simulation for a specified number of timesteps
// this runs the sim with a constant temperature (specified as temperature in the list of input params)
// so it's not really used.  more common/practical is the running with a list of per-day temperatures
// this run method is described below
void SWDSimulatorSingle::run(double temperature, double numTimeSteps, bool ignoreFruit, bool ignoreDiapause) { // run the model with a constant temperature
	
	if (numTimeSteps < 0)
		return; // no negative time

	// defaulting to startdate of 0
	cell.setAddInitPop(true);
	cell.readInitFlies();
	
	for (double i = 0; round2Decimals(i) < numTimeSteps; i += dt) { // run from time = 0 to the specified number of timesteps
		cell.stepForward(temperature, ignoreFruit, ignoreDiapause, dt, timeStep); // run the cell with specified temperature!
		timeStep += dt;
	}
}

// method to run the simulation for a specified number of timesteps, with the daily temperatures specified
// as an array (the temperatures vector).
// whether or not to ignore diapause/fruit submodel bools are also passed in as arguments,
// and the day to add the flies at (startDay)
void SWDSimulatorSingle::run(std::vector<double> temperatures, double numTimeSteps, bool ignoreFruit, bool ignoreDiapause, int startDay) { 
	
	if (numTimeSteps < 0)
		return; // no negative time!
	
	int index = ((int) timeStep) % temperatures.size(); // current timestep of the cell, to match with the corresponding temperature value in the list
	
	if (startDay >= 0)
		cell.setAddInitPop(true); // if valid startday, set variable to ensure that init pop isn't added before injection date, regardless of diapause
	
	for (double i = 0; round2Decimals(i) < numTimeSteps; i += dt) {
		if ((int) index >= temperatures.size()) // if there is not enough temperature data, it reloops to the beginning
			index = 0;
		if ((int) timeStep == startDay && !injectFlies) {
			injectFlies = true;
			cell.readInitFlies(); // read in initial populations on the chosen date
		}
		cell.stepForward(temperatures[(int) index], ignoreFruit, ignoreDiapause, dt, timeStep); // assume temperature given daily

		timeStep += dt;
		index = ((int) timeStep) % temperatures.size();
	}
}

// method to reset the simulation to timestep 0
void SWDSimulatorSingle::resetTime() {
	timeStep = 0;
	cell.resetTime(); // reset the cell to timestep 0
	injectFlies = false;
}


